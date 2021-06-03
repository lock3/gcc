/* Definitions for C++ contract levels
   Copyright (C) 2020 Free Software Foundation, Inc.
   Contributed by Jeff Chapman II (jchapman@lock3software.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Design Notes

   A function is called a "guarded" function if it has pre or post contract
   attributes. A contract is considered an "active" contract if runtime code is
   needed for the contract under the current contract configuration.

   pre and post contract attributes are parsed and stored in DECL_ATTRIBUTES.
   assert contracts are parsed and wrapped in statements. When genericizing, all
   active and assumed contracts are transformed into an if block. An observed
   contract:

     [[ pre: v > 0 ]]

   is transformed into:

     if (!(v > 0)) {
       __on_contract_violation (true, // continue_
	 5, // line_number,
	 "main.cpp", // file_name,
	 "fun", // function_name,
	 "v > 0", // comment,
	 "default", // assertion_level,
	 "default", // assertion_role,
	 CCS_MAYBE, // continuation_mode
	 );
     }

   Here, __on_contract_violation is a shim used to actually construct the
   std::contract_violation and call the installed handler, finally terminating
   if the contract should not continue on violation. This prevents requiring
   including <contract> and simplifies building the call.

   Assumed contracts have a similar transformation that results the body of the
   if being __builtin_unreachable ();

   Parsing of pre and post contract conditions need to be deferred when the
   contracts are attached to a member function. The postcondition identifier
   cannot be used before the deduced return type of an auto function is used,
   except when used in a defining declaration in which case they conditions are
   fully parsed once the body is finished (see cpp2a/contracts-deduced{1,2}.C).

   A list of pre and post contracts can either be repeated in their entirety or
   completely absent in subsequent declarations. If contract lists appear on two
   matching declarations, their contracts have to be equivalent. In general this
   means that anything before the colon have to be token equivalent and the
   condition must be cp_tree_equal (primarily to allow for parameter renaming).

   Contracts on overrides must match those present on (all of) the overridee(s).

   Template specializations may have their own contracts. If no contracts are
   specified on the initial specialization they're assumed to be the same as
   the primary template. Specialization redeclarations must then match either
   the primary template (if they were unspecified originally), or those
   specified on the specialization.


   For non-cdtors two functions are generated for ease of implementation and to
   avoid some cases where code bloat may occurr. These are the DECL_PRE_FN and
   DECL_POST_FN. Each handles checking either the set of pre or post contracts
   of a guarded function.

     int fun(int v)
       [[ pre: v > 0 ]]
       [[ post r: r < 0 ]]
     {
       return -v;
     }

   The original decl is left alone and instead calls are generated to pre/post
   functions within the body:

     void fun.pre(int v)
     {
       [[ assert: v > 0 ]];
     }
     int fun.post(int v, int __r)
     {
       [[ assert: __r < 0 ]];
     }
     int fun(int v)
     {
       fun.pre(v);
       return fun.post(v, -v);
     }

   This sides steps a number of issues with having to rewrite the bodies or
   rewrite the parsed conditions as the parameters to the original function
   changes (as happens during redeclaration). The ultimate goal is to get
   something that optimizes well along the lines of

     int fun(int v)
     {
       [[ assert: v > 0 ]];
       auto &&__r = -v;
       goto out;
     out:
       [[ assert: __r < 0 ]];
       return __r;
     }

   With the idea being that multiple return statements could collapse the
   function epilogue after inlining the pre/post functions. clang is able
   to collapse common function epilogues, while gcc needs -O3 -Os combined.
   We're already doing this "manually" for cdtors due to the way they're already
   implemented, forcing DECL_CDTOR_NEEDS_LABLED_EXIT_P to be true when the
   cdtor has active contracts.

   Directly laying the pre contracts down in the function body doesn't have
   many issues. The post contracts may need to be repeated multiple times, once
   for each return, or a goto epilogue would need generated similarly to cdtors.
   For this initial implementation, generating function calls and letting
   later optimizations decide whether to inline and duplicate the actual
   checks or whether to collapse the shared epilogue was chosen.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "cp-tree.h"
#include "stringpool.h"
#include "diagnostic.h"
#include "options.h"
#include "contracts.h"
#include "tree.h"
#include "tree-inline.h"
#include "attribs.h"
#include "print-tree.h"

const int max_custom_roles = 32;
static contract_role contract_build_roles[max_custom_roles] = {
};

bool valid_configs[CCS_MAYBE + 1][CCS_MAYBE + 1] = {
  { 0, 0, 0, 0, 0, },
  { 0, 1, 0, 0, 0, },
  { 0, 1, 1, 1, 1, },
  { 0, 1, 1, 1, 1, },
  { 0, 1, 0, 0, 1, },
};

void
validate_contract_role (contract_role *role)
{
  gcc_assert (role);
  if (!unchecked_contract_p (role->axiom_semantic))
    error ("axiom contract semantic must be %<assume%> or %<ignore%>");

  if (!valid_configs[role->default_semantic][role->audit_semantic] )
    warning (0, "the %<audit%> semantic should be at least as strong as "
		"the %<default%> semantic");
}

contract_semantic
lookup_concrete_semantic (const char *name)
{
  if (strcmp (name, "ignore") == 0)
    return CCS_IGNORE;
  if (strcmp (name, "assume") == 0)
    return CCS_ASSUME;
  if (strcmp (name, "check_never_continue") == 0
      || strcmp (name, "never") == 0
      || strcmp (name, "abort") == 0)
    return CCS_NEVER;
  if (strcmp (name, "check_maybe_continue") == 0
      || strcmp (name, "maybe") == 0)
    return CCS_MAYBE;
  error ("'%s' is not a valid explicit concrete semantic", name);
  return CCS_INVALID;
}

/* Compare role and name up to either the NUL terminator or the first
   occurrence of colon.  */

static bool
role_name_equal (const char *role, const char *name)
{
  size_t role_len = strchrnul (role, ':') - role;
  size_t name_len = strchrnul (name, ':') - name;
  if (role_len != name_len)
    return false;
  return strncmp (role, name, role_len) == 0;
}

static bool
role_name_equal (contract_role *role, const char *name)
{
  if (role->name == NULL)
    return false;
  return role_name_equal (role->name, name);
}

contract_role *
get_contract_role (const char *name)
{
  for (int i = 0; i < max_custom_roles; ++i)
    {
      contract_role *potential = contract_build_roles + i;
      if (role_name_equal (potential, name))
	return potential;
    }
  if (role_name_equal (name, "default") || role_name_equal (name, "review"))
    {
      setup_default_contract_role (false);
      return get_contract_role (name);
    }
  return NULL;
}

contract_role *
add_contract_role (const char *name,
		   contract_semantic des,
		   contract_semantic aus,
		   contract_semantic axs,
		   bool update)
{
  for (int i = 0; i < max_custom_roles; ++i)
    {
      contract_role *potential = contract_build_roles + i;
      if (potential->name != NULL
	  && !role_name_equal (potential, name))
	continue;
      if (potential->name != NULL && !update)
	return potential;
      potential->name = name;
      potential->default_semantic = des;
      potential->audit_semantic = aus;
      potential->axiom_semantic = axs;
      return potential;
    }
  return NULL;
}

enum contract_build_level { OFF, DEFAULT, AUDIT };
static bool flag_contract_continuation_mode = false;
static bool flag_contract_assumption_mode = true;
static int flag_contract_build_level = DEFAULT;

static bool contracts_p1332_default = false, contracts_p1332_review = false,
  contracts_std = false, contracts_p1429 = false;

static contract_semantic
get_concrete_check ()
{
  return flag_contract_continuation_mode ? CCS_MAYBE : CCS_NEVER;
}

static contract_semantic
get_concrete_axiom_semantic ()
{
  return flag_contract_assumption_mode ? CCS_ASSUME : CCS_IGNORE;
}

void
setup_default_contract_role (bool update)
{
  contract_semantic check = get_concrete_check ();
  contract_semantic axiom = get_concrete_axiom_semantic ();
  switch (flag_contract_build_level)
    {
      case OFF:
	add_contract_role ("default", CCS_IGNORE, CCS_IGNORE, axiom, update);
	add_contract_role ("review", CCS_IGNORE, CCS_IGNORE, CCS_IGNORE, update);
	break;
      case DEFAULT:
	add_contract_role ("default", check, CCS_IGNORE, axiom, update);
	add_contract_role ("review", check, CCS_IGNORE, CCS_IGNORE, update);
	break;
      case AUDIT:
	add_contract_role ("default", check, check, axiom, update);
	add_contract_role ("review", check, check, CCS_IGNORE, update);
	break;
    }
}

contract_semantic
map_contract_semantic (const char *ident)
{
  if (strcmp (ident, "ignore") == 0)
    return CCS_IGNORE;
  else if (strcmp (ident, "assume") == 0)
    return CCS_ASSUME;
  else if (strcmp (ident, "check_never_continue") == 0)
    return CCS_NEVER;
  else if (strcmp (ident, "check_maybe_continue") == 0)
    return CCS_MAYBE;
  return CCS_INVALID;
}

contract_level
map_contract_level (const char *ident)
{
  if (strcmp (ident, "default") == 0)
    return CONTRACT_DEFAULT;
  else if (strcmp (ident, "audit") == 0)
    return CONTRACT_AUDIT;
  else if (strcmp (ident, "axiom") == 0)
    return CONTRACT_AXIOM;
  return CONTRACT_INVALID;
}


void
handle_OPT_fcontract_build_level_ (const char *arg)
{
  if (contracts_p1332_default || contracts_p1332_review || contracts_p1429)
    {
      error ("-fcontract-build-level= cannot be mixed with p1332/p1429");
      return;
    }
  else
    contracts_std = true;

  if (strcmp (arg, "off") == 0)
    flag_contract_build_level = OFF;
  else if (strcmp (arg, "default") == 0)
    flag_contract_build_level = DEFAULT;
  else if (strcmp (arg, "audit") == 0)
    flag_contract_build_level = AUDIT;
  else
    error ("-fcontract-build-level= must be off|default|audit");

  setup_default_contract_role ();
}

void
handle_OPT_fcontract_assumption_mode_ (const char *arg)
{
  if (contracts_p1332_default || contracts_p1332_review || contracts_p1429)
    {
      error ("-fcontract-assumption-mode= cannot be mixed with p1332/p1429");
      return;
    }
  else
    contracts_std = true;

  if (strcmp (arg, "on") == 0)
    flag_contract_assumption_mode = true;
  else if (strcmp (arg, "off") == 0)
    flag_contract_assumption_mode = false;
  else
    error ("-fcontract-assumption-mode= must be %<on%> or %<off%>");

  setup_default_contract_role ();
}

void
handle_OPT_fcontract_continuation_mode_ (const char *arg)
{
  if (contracts_p1332_default || contracts_p1332_review || contracts_p1429)
    {
      error ("-fcontract-continuation-mode= cannot be mixed with p1332/p1429");
      return;
    }
  else
    contracts_std = true;

  if (strcmp (arg, "on") == 0)
    flag_contract_continuation_mode = true;
  else if (strcmp (arg, "off") == 0)
    flag_contract_continuation_mode = false;
  else
    error ("-fcontract-continuation-mode= must be %<on%> or %<off%>");

  setup_default_contract_role ();
}

void
handle_OPT_fcontract_role_ (const char *arg)
{
  const char *name = arg;
  const char *vals = strchr (name, ':');
  if (vals == NULL)
    {
      error ("-fcontract-role= must be in the form role:semantics");
      return;
    }

  contract_semantic dess = CCS_INVALID, auss = CCS_INVALID, axss = CCS_INVALID;
  char *des = NULL, *aus = NULL, *axs = NULL;
  des = xstrdup (vals + 1);

  aus = strchr (des, ',');
  if (aus == NULL)
    {
      error ("-fcontract-role= semantics must include default,audit,axiom values");
      goto validate;
    }
  *aus = '\0'; // null terminate des
  aus = aus + 1; // move past null

  axs = strchr (aus, ',');
  if (axs == NULL)
    {
      error ("-fcontract-role= semantics must include default,audit,axiom values");
      goto validate;
    }
  *axs = '\0'; // null terminate aus
  axs = axs + 1; // move past null

  dess = lookup_concrete_semantic (des);
  auss = lookup_concrete_semantic (aus);
  axss = lookup_concrete_semantic (axs);
validate:
  free (des);
  if (dess == CCS_INVALID || auss == CCS_INVALID || axss == CCS_INVALID)
    return;

  bool is_defalult_role = role_name_equal (name, "default");
  bool is_review_role = role_name_equal (name, "review");
  bool is_std_role = is_defalult_role || is_review_role;
  if ((contracts_std && is_std_role) || (contracts_p1429 && is_defalult_role))
    {
      error ("-fcontract-role= cannot be mixed with std/p1429 contract flags");
      return;
    }
  else if (is_std_role)
    {
      contracts_p1332_default |= is_defalult_role;
      contracts_p1332_review |= is_review_role;
    }

  contract_role *role = add_contract_role (name, dess, auss, axss);

  if (role == NULL)
    {
      // TODO: not enough space?
      error ("-fcontract-level= too many custom roles");
      return;
    }
  else
    validate_contract_role (role);
}

void
handle_OPT_fcontract_semantic_ (const char *arg)
{
  if (!strchr (arg, ':'))
    {
      error ("-fcontract-semantic= must be in the form level:semantic");
      return;
    }

  if (contracts_std || contracts_p1332_default)
    {
      error ("-fcontract-semantic= cannot be mixed with std/p1332 contract flags");
      return;
    }
  contracts_p1429 = true;

  contract_role *role = get_contract_role ("default");
  if (!role)
    {
      error ("-fcontract-semantic= cannot find default role");
      return;
    }

  const char *semantic = strchr (arg, ':') + 1;
  contract_semantic sem = lookup_concrete_semantic (semantic);
  if (sem == CCS_INVALID)
    return;

  if (strncmp ("default:", arg, 8) == 0)
    role->default_semantic = sem;
  else if (strncmp ("audit:", arg, 6) == 0)
    role->audit_semantic = sem;
  else if (strncmp ("axiom:", arg, 6) == 0)
    role->axiom_semantic = sem;
  else
    error ("-fcontract-semantic= level must be default, audit, or axiom");
  validate_contract_role (role);
}

/* Convert a contract CONFIG into a contract_mode.  */

static contract_mode
contract_config_to_mode (tree config)
{
  if (config == NULL_TREE)
    return contract_mode (CONTRACT_DEFAULT, get_default_contract_role ());

  /* TREE_LIST has TREE_VALUE is a level and TREE_PURPOSE is role.  */
  if (TREE_CODE (config) == TREE_LIST)
    {
      contract_role *role = NULL;
      if (TREE_PURPOSE (config))
	role = get_contract_role (IDENTIFIER_POINTER (TREE_PURPOSE (config)));
      if (!role)
	role = get_default_contract_role ();

      contract_level level =
	map_contract_level (IDENTIFIER_POINTER (TREE_VALUE (config)));
      return contract_mode (level, role);
    }

  /* Literal semantic.  */
  gcc_assert (TREE_CODE (config) == IDENTIFIER_NODE);
  contract_semantic semantic =
    map_contract_semantic (IDENTIFIER_POINTER (config));
  return contract_mode (semantic);
}

/* Convert a contract's config into a concrete semantic using the current
   contract semantic mapping.  */

static contract_semantic
compute_concrete_semantic (tree contract)
{
  contract_mode mode = contract_config_to_mode (CONTRACT_MODE (contract));
  /* Compute the concrete semantic for the contract.  */
  if (!flag_contract_mode)
    /* If contracts are off, treat all contracts as ignore.  */
    return CCS_IGNORE;
  else if (mode.kind == contract_mode::cm_invalid)
    return CCS_INVALID;
  else if (mode.kind == contract_mode::cm_explicit)
    return mode.get_semantic ();
  else
    {
      gcc_assert (mode.get_role ());
      gcc_assert (mode.get_level () != CONTRACT_INVALID);
      contract_level level = mode.get_level ();
      contract_role *role = mode.get_role ();
      if (level == CONTRACT_DEFAULT)
	return role->default_semantic;
      else if (level == CONTRACT_AUDIT)
	return role->audit_semantic;
      else if (level == CONTRACT_AXIOM)
	return role->axiom_semantic;
    }
  gcc_assert (false);
}

/* Returns an invented parameter declration of the form 'TYPE ID' for the
   purpose of parsing the postcondition.
   
   We use a PARM_DECL instead of a VAR_DECL so that tsubst forces a lookup
   in local specializations when we instantiate these things later.  */

tree
make_postcondition_variable (cp_expr id, tree type)
{
  if (id == error_mark_node)
    return id;

  tree decl = build_lang_decl (PARM_DECL, id, type);
  DECL_ARTIFICIAL (decl) = true;
  DECL_SOURCE_LOCATION (decl) = id.get_location ();
  push_binding (id, decl, current_binding_level);
  return decl;
}

/* As above, except that the type is unknown.  */

tree
make_postcondition_variable (cp_expr id)
{
  return make_postcondition_variable (id, make_auto ());
}

/* Instantiate each postcondition with the return type to finalize the
   attribute.  */

void
rebuild_postconditions (tree decl, tree type)
{
  tree attributes = DECL_CONTRACTS (decl);

  /* If the return type is undeduced, defer until later.  */
  if (TREE_CODE (type) == TEMPLATE_TYPE_PARM)
    return;
  
  for (; attributes ; attributes = TREE_CHAIN (attributes))
    {
      if (!cxx_contract_attribute_p (attributes))
        continue;
      tree contract = TREE_VALUE (TREE_VALUE (attributes));
      if (TREE_CODE (contract) != POSTCONDITION_STMT)
	continue;
      tree condition = CONTRACT_CONDITION (contract);
      /* If any conditions are deferred, they're all deferred.  Note that
	 we don't have to instantiate postconditions in that case because
	 the type is available through the declaration.  */
      if (TREE_CODE (condition) == DEFERRED_PARSE)
	return;
      tree oldvar = POSTCONDITION_IDENTIFIER (contract);
      if (!oldvar)
	continue;

      /* "Instantiate" the result variable using the known type.  */
      tree newvar = copy_node (oldvar);
      TREE_TYPE (newvar) = type;

      /* Make parameters and result available for substitution.  */
      local_specialization_stack stack (lss_copy);
      for (tree t = DECL_ARGUMENTS (decl); t != NULL_TREE; t = TREE_CHAIN (t))
        register_local_identity (t);
      register_local_specialization (newvar, oldvar);

      ++cp_contract_operand;
      condition = tsubst_expr (condition, make_tree_vec (0),
			       tf_warning_or_error, decl, false);
      --cp_contract_operand;

      /* Update the contract condition and result.  */
      POSTCONDITION_IDENTIFIER (contract) = newvar;
      CONTRACT_CONDITION (contract) = finish_contract_condition (condition);
    }
}

static tree
build_comment (cp_expr condition)
{
  /* Try to get the actual source text for the condition; if that fails pretty
     print the resulting tree.  */
  char *str = get_source (condition.get_start (), condition.get_finish ());
  if (!str)
    {
      /* FIXME cases where we end up here
	 #line macro usage (oof)
	 contracts10.C
	 contracts11.C  */
      const char *str = expr_to_string (condition);
      return build_string_literal (strlen (str) + 1, str);
    }

  tree t = build_string_literal (strlen (str) + 1, str);
  free (str);
  return t;
}

/* Build a contract statement.  */

tree
grok_contract (tree attribute, tree mode, tree result, cp_expr condition,
	       location_t loc)
{
  tree_code code;
  if (is_attribute_p ("assert", attribute))
    code = ASSERTION_STMT;
  else if (is_attribute_p ("pre", attribute))
    code = PRECONDITION_STMT;
  else if (is_attribute_p ("post", attribute))
    code = POSTCONDITION_STMT;
  else
    gcc_unreachable ();

  /* Build the contract. The condition is added later.  In the case that
     the contract is deferred, result an plain identifier, not a result
     variable.  */
  tree contract;
  tree type = void_type_node;
  if (code != POSTCONDITION_STMT)
    contract = build3_loc (loc, code, type, mode, NULL_TREE, NULL_TREE);
  else
    contract = build4_loc (loc, code, type, mode, NULL_TREE, NULL_TREE, result);

  /* Determine the concrete semantic.  */
  set_contract_semantic (contract, compute_concrete_semantic (contract));

  /* If the contract is deferred, don't do anything with the condition.  */
  if (TREE_CODE (condition) == DEFERRED_PARSE)
    {
      CONTRACT_CONDITION (contract) = condition;
      return contract;
    }

  /* Generate the comment from the original condition.  */
  CONTRACT_COMMENT (contract) = build_comment (condition);

  /* The condition is converted to bool.  */
  condition = finish_contract_condition (condition);
  CONTRACT_CONDITION (contract) = condition;

  return contract;
}

/* Build the contract attribute specifier where IDENTIFIER is one of 'pre',
   'post' or 'assert' and CONTRACT is the underlying statement.  */
tree
finish_contract_attribute (tree identifier, tree contract)
{
  if (contract == error_mark_node)
    return error_mark_node;
    
  tree attribute = build_tree_list (build_tree_list (NULL_TREE, identifier),
				    build_tree_list (NULL_TREE, contract));


  /* Mark the attribute as dependent if the condition is dependent.

     TODO: I'm not sure this is strictly necessary. It's going to be marked as
     such by a subroutine of cplus_decl_attributes. */
  // FIXME because these are spliced out of order later, we may have to mark
  // _all_ contracts as dependent instead or we may not be able to match decls
  // properly
  tree condition = CONTRACT_CONDITION (contract);
  if (TREE_CODE (condition) == DEFERRED_PARSE
      || value_dependent_expression_p (condition))
    ATTR_IS_DEPENDENT (attribute) = true;

  return attribute;
}


/* Update condition of a late-parsed contract and postcondition variable,
   if any.  */

void
update_late_contract (tree contract, tree result, tree condition)
{
  if (TREE_CODE (contract) == POSTCONDITION_STMT)
    POSTCONDITION_IDENTIFIER (contract) = result;

  /* Generate the comment from the original condition.  */
  CONTRACT_COMMENT (contract) = build_comment (condition);

  /* The condition is converted to bool.  */
  condition = finish_contract_condition (condition);
  CONTRACT_CONDITION (contract) = condition;
}

/* Return TRUE iff ATTR has been parsed by the front-end as a c++2a contract
   attribute. */

bool
cxx_contract_attribute_p (const_tree attr)
{
  if (attr == NULL_TREE
      || TREE_CODE (attr) != TREE_LIST)
    return false;

  if (!TREE_PURPOSE (attr) || TREE_CODE (TREE_PURPOSE (attr)) != TREE_LIST)
    return false;
  if (!TREE_VALUE (attr) || TREE_CODE (TREE_VALUE (attr)) != TREE_LIST)
    return false;
  if (!TREE_VALUE (TREE_VALUE (attr)))
    return false;

  return (TREE_CODE (TREE_VALUE (TREE_VALUE (attr))) == PRECONDITION_STMT
      || TREE_CODE (TREE_VALUE (TREE_VALUE (attr))) == POSTCONDITION_STMT
      || TREE_CODE (TREE_VALUE (TREE_VALUE (attr))) == ASSERTION_STMT);
}

/* Replace any references in CONTRACT's CONDITION to SRC's parameters with
   references to DST's parameters.

   This is useful when the DECL_PARMs used to parse a contract aren't the
   final parms used for the function definition.  For example when an initial
   declaration has contracts listed but the definition does not.

   This is also used to reuse a parent type's contracts on virtual methods.  */

void
remap_contract (tree src, tree dst, tree contract)
{
  copy_body_data id;
  hash_map<tree, tree> decl_map;

  memset (&id, 0, sizeof (id));
  id.src_fn = src;
  id.dst_fn = dst;
  id.src_cfun = DECL_STRUCT_FUNCTION (src);
  id.decl_map = &decl_map;

  id.copy_decl = copy_decl_no_change;
  id.transform_call_graph_edges = CB_CGE_DUPLICATE;
  id.transform_new_cfg = false;
  id.transform_return_to_modify = false;
  id.transform_parameter = true;
  id.transform_lang_insert_block = NULL;

  /* Make sure not to unshare trees behind the front-end's back
     since front-end specific mechanisms may rely on sharing.  */
  id.regimplify = false;
  id.do_not_unshare = true;
  id.do_not_fold = true;

  /* We're not inside any EH region.  */
  id.eh_lp_nr = 0;

  bool do_remap = false;

  /* Insert paramater remappings.  */
  if (TREE_CODE (src) == FUNCTION_DECL) src = DECL_ARGUMENTS (src);
  if (TREE_CODE (dst) == FUNCTION_DECL) dst = DECL_ARGUMENTS (dst);

  for (tree sp = src, dp = dst; sp || dp;
      sp = DECL_CHAIN (sp), dp = DECL_CHAIN (dp))
    {
      gcc_assert (sp && dp);

      if (sp == dp) continue;
      insert_decl_map (&id, sp, dp);
      do_remap = true;
    }
  if (!do_remap)
    return;

  walk_tree (&CONTRACT_CONDITION (contract), copy_tree_body_r, &id, NULL);
}

/* Rewrite any references to SRC's PARM_DECLs to the corresponding PARM_DECL
   in DST in all of the contract attributes in CONTRACTS by calling
   remap_contract on each.  */

void
remap_contracts (tree src, tree dst, tree contracts)
{
  for (tree attr = contracts; attr; attr = CONTRACT_CHAIN (attr))
    {
      tree contract = CONTRACT_STATEMENT (attr);
      if (TREE_CODE (CONTRACT_CONDITION (contract)) != DEFERRED_PARSE)
	remap_contract (src, dst, CONTRACT_STATEMENT (attr));
    }
}

/* Helper to replace references to dummy this parameters with references to
   the first argument of the FUNCTION_DECL DATA.  */

static tree
remap_dummy_this_1 (tree *tp, int *, void *data)
{
  if (!is_this_parameter (*tp))
    return NULL_TREE;
  tree fn = (tree)data;
  *tp = DECL_ARGUMENTS (fn);
  return NULL_TREE;
}

/* Replace all references to dummy this parameters in EXPR with references to
   the first argument of the FUNCTION_DECL FN.  */

void
remap_dummy_this (tree fn, tree *expr)
{
  walk_tree (expr, remap_dummy_this_1, fn, NULL);
}

static int depth;
static const char* tabstr = "";

struct indentation
{
  indentation()
  {
    saved_depth = depth;
    ++depth;
    saved_tabstr = tabstr;
    tabstr = make_tab();
  }

  indentation(int d)
  {
    saved_depth = depth;
    depth = d;
    saved_tabstr = tabstr;
    tabstr = make_tab();
  }

  ~indentation()
  {
    if (depth != 0)
      free (const_cast<char*>(tabstr));
    tabstr = saved_tabstr;
    depth = saved_depth;
  }

  char const* make_tab()
  {
    if (depth == 0)
      return "";
    int len = 2 * depth;
    char* str = (char*)xmalloc(len + 1);
    memset(str, ' ', len);
    str[len] = 0;
    return str;
  }

  int saved_depth;
  char const* saved_tabstr;
};

static const char* tab()
{
  return tabstr;
}

void* node_id(tree t)
{
  return (void*)t;
}

struct node_info
{
  node_info (tree t)
  {
    make_string (t);
  }

  /* Creates node information for a node with a "fake name".  */
  node_info (tree t, const char* name)
  {
    make_string (t, name);
  }

  void make_string (tree t, const char* name)
  {
    if (!t)
      return build_string ("31", "null");
   
    void* ptr = t;
    if (t == error_mark_node)
      return build_string ("31", name, ptr);
    
    if (DECL_P (t))
      return build_string ("33", name, ptr);
    
    if (TYPE_P (t))
      return build_string ("34", name, ptr);

    // Expressions, statements, lists, etc.
    build_string ("32", name, ptr);
  }

  void make_string (tree t)
  {
    if (!t)
      return build_string ("31", "null");

    const char* name = get_tree_code_name (TREE_CODE (t));
    return make_string (t, name);
  }

  void build_string (const char* color, const char* name)
  {
    snprintf (buf, buflen, "\033[01;%sm%s\033[0m", color, name);
  }

  void build_string (const char* color, const char* name, void* ptr)
  {
    snprintf (buf, buflen, "\033[01;%sm%s\033[0m (\033[36m%p\033[0m)", color, name, ptr);
  }

  const char* str ()
  {
    return buf;
  }

  static constexpr int buflen = 128;
  char buf[buflen];
};

// Prints a bolded header name.
struct header
{
  header (const char* name)
  {
    sprintf (buf, "\033[01m%s\033[0m", name);
    verbatim ("%s%s", tab(), buf);
  }

  char buf[32];
};

void debug_type (tree t)
{
  node_info info (t);
  if (!t)
    verbatim ("%s%s type", tab(), info.str ());
  else
    verbatim ("%s%s %qT", tab(), info.str (), t);
}

void debug_expression (tree t)
{
  node_info info (t);
  verbatim ("%s%s %qE", tab(), info.str (), t);

  indentation indent;
  debug_type (TREE_TYPE (t));

  if (DECL_P (t))
    return;

  for (int i = 0; i < TREE_OPERAND_LENGTH (t); ++i)
    debug_expression (TREE_OPERAND (t, i));
}

void debug_function (tree t)
{
  node_info info (t);
  verbatim ("%s%s %qE", tab(), info.str (), t);

  indentation indent;

  debug_type (TREE_TYPE (t));

//   if (tree ti = DECL_TEMPLATE_INFO (t))
//     ; // FIXME: Print information about templates and arguments?

  header h ("parameters");
  debug_parameters (DECL_ARGUMENTS (t));

  if (tree contracts = DECL_CONTRACTS (t))
    {
      header h ("contracts");
      debug_contracts (contracts);
    }
}

void debug_type_declaration (tree t)
{
  node_info info (t);
  verbatim ("%s%s %qE", tab(), info.str (), t);
  indentation indent;
  debug_type (TREE_TYPE (t));
}

void debug_template_parameter (tree t)
{
  /* T is a pair combining the parameter with the default argument?  */
  debug_declaration (TREE_VALUE (t));

}

void debug_template_parameters (tree t)
{
  /* TODO: What should we do with the index? Is it superfluous?  */
  tree parms = TREE_VALUE (t);
  for (int i = 0; i < TREE_VEC_LENGTH (parms); ++i)
    debug_template_parameter (TREE_VEC_ELT (parms, i));
}

void debug_template (tree t)
{
  node_info info (t);
  verbatim ("%s%s %q#D", tab(), info.str (), t);

  indentation indent;

  header parms("template parameters");
  debug_template_parameters (DECL_TEMPLATE_PARMS (t));

  header result("result");
  debug_declaration (DECL_TEMPLATE_RESULT (t));
}

void debug_declaration (tree t)
{
  switch (TREE_CODE (t))
    {
    case TEMPLATE_DECL:
      return debug_template (t);
    case FUNCTION_DECL:
      return debug_function (t);
    case TYPE_DECL:
      return debug_type_declaration (t);
    default:
      debug_tree (t);
      gcc_unreachable ();
    }
}

void debug_parameters (tree t)
{
  for (; t; t = TREE_CHAIN (t))
    debug_parameter (t);
}

void debug_parameter (tree t)
{
  node_info info (t);
  verbatim ("%s%s %q#D", tab(), info.str (), t);
  indentation indent;
  debug_type (TREE_TYPE (t));
}

void debug_contracts (tree t)
{
  for (; t; t = TREE_CHAIN (t))
    debug_contract (t);
}

void debug_contract (tree t)
{
  /* See through attributes.  */
  if (TREE_CODE (t) == TREE_LIST)
    t = TREE_VALUE (t);
  if (TREE_CODE (t) == TREE_LIST)
    t = TREE_VALUE (t);
  
  tree condition = CONTRACT_CONDITION (t);
  
  node_info info (t);
  verbatim ("%s%s %qE", tab(), info.str (), condition);
  indentation indent;
  debug_expression (condition);
}

void debug_template_arguments (tree args)
{
  if (TMPL_ARGS_HAVE_MULTIPLE_LEVELS (args))
    gcc_unreachable ();

  node_info info (args, "template_argument_list");
  verbatim ("%s%s", tab(), info.str ());

  indentation indent;

  for (int i = 0; i < TREE_VEC_LENGTH (args); ++i)
    {
      tree arg = TREE_VEC_ELT (args, 0);
      node_info arg_info (arg);
      verbatim("%s%s index=%d", tab(), arg_info.str (), i);
    }
}