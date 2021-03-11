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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "cp-tree.h"
#include "stringpool.h"
#include "diagnostic.h"
#include "options.h"
#include "cxx-contracts.h"
#include "tree.h"
#include "tree-inline.h"
#include "cxx-config.h"

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
  else if (strcmp (ident, "enforce") == 0)
    return CCS_NEVER;
  else if (strcmp (ident, "observe") == 0)
    return CCS_MAYBE;
  /* FIXME these need nuked right? */
  else if (strcmp (ident, "check_never_continue") == 0)
    return CCS_NEVER;
  else if (strcmp (ident, "check_maybe_continue") == 0)
    return CCS_MAYBE;
  else if (strcmp (ident, "enforce") == 0)
    return CCS_NEVER;
  else if (strcmp (ident, "observe") == 0)
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
      || TREE_CODE (TREE_VALUE (TREE_VALUE (attr))) == ASSERTION_STMT
      || TREE_CODE (TREE_VALUE (TREE_VALUE (attr))) == ASSUMPTION_STMT);
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

static GTY(()) hash_map<tree, tree> contract_labels;

hash_map<tree, tree>::iterator
contract_labels_begin ()
{
  return contract_labels.begin ();
}

hash_map<tree, tree>::iterator
contract_labels_end ()
{
  return contract_labels.end ();
}

static tree
resolve_contract_label_type (tree scope, tree qual)
{
  tree val = TREE_VALUE (qual);
  if (TREE_CHAIN (qual))
    {
      tree ns = lookup_qualified_name (scope, val, LOOK_want::NAMESPACE, false);
      return resolve_contract_label_type (ns, TREE_CHAIN (qual));
    }
  tree decl = lookup_qualified_name (scope, val, LOOK_want::TYPE, false);
  return decl;
}

tree
lookup_contract_label (tree name)
{
  tree *contract_label = contract_labels.get (name);
  if (!contract_label)
    return NULL_TREE;
  if (TREE_CODE (*contract_label) == TREE_LIST)
    {
      /* Lazily loaded contract_lable from module.  */
      tree decl = resolve_contract_label_type (global_namespace, *contract_label);
      if (decl == error_mark_node)
	*contract_label = error_mark_node;
      else
	*contract_label = TREE_TYPE (decl);
    }
  return *contract_label;
}

void
define_contract_label (tree args, tree type)
{
  tree name = TREE_VALUE (args);

  tree *contract_label = contract_labels.get (name);
  if (contract_label && *contract_label != type)
    {
      error_at (EXPR_LOCATION (TREE_PURPOSE (args)),
		"redeclaration of contract label %qE", name);
      return;
    }

  contract_labels.put (name, type);
}

/* Determine the default semantic for a given contract attribute KIND.  */

static contract_semantic
contract_attribute_kind_default_semantic (tree_code kind)
{
  if (kind == PRECONDITION_STMT)
    return map_contract_semantic (lookup_knob ("pre"));
  if (kind == POSTCONDITION_STMT)
    return map_contract_semantic (lookup_knob ("post"));
  if (kind == ASSERTION_STMT)
    return map_contract_semantic (lookup_knob ("assert"));
  if (kind == ASSUMPTION_STMT)
    return map_contract_semantic (lookup_knob ("assume"));
  gcc_assert (false);
}

/* Lookup and return the contracts namespace if it is included, NULL_TREE
   otherwise.  */

static tree
lookup_contracts_namespace ()
{
  tree std_namespace = lookup_qualified_name (global_namespace, "std",
					      LOOK_want::NAMESPACE, false);
  if (!std_namespace)
    return NULL_TREE;
  tree experimental_namespace
    = lookup_qualified_name (std_namespace, "experimental",
			     LOOK_want::NAMESPACE, false);
  if (!experimental_namespace)
    return NULL_TREE;
  return lookup_qualified_name (experimental_namespace, "contracts",
				LOOK_want::NAMESPACE, false);
}

/* Setup the global type_node trees for the std::experimental::contracts label
   support.  */

static void
setup_contracts_type_nodes ()
{
  if (contracts_attribute_type_node && contracts_semantic_type_node)
    return;

  tree contracts_namespace = lookup_contracts_namespace ();
  if (!contracts_namespace)
    return;

  contracts_attribute_type_node
    = lookup_qualified_name (contracts_namespace, "attribute", LOOK_want::TYPE,
			     false);
  if (DECL_P (contracts_attribute_type_node))
    contracts_attribute_type_node = TREE_TYPE (contracts_attribute_type_node);
  else
    contracts_attribute_type_node = NULL_TREE;

  contracts_semantic_type_node
    = lookup_qualified_name (contracts_namespace, "semantic", LOOK_want::TYPE,
			     false);
  if (DECL_P (contracts_semantic_type_node))
    contracts_semantic_type_node = TREE_TYPE (contracts_semantic_type_node);
  else
    contracts_semantic_type_node = NULL_TREE;
}

/* Return the corresponding std::experimental::contracts::attribute enum value
   for a given KIND of contract attribute.  */

static tree
map_contract_to_attribute (tree_code kind)
{
  if (!contracts_attribute_type_node)
    return NULL_TREE;
  const char *name = NULL;
  switch (kind)
    {
      case ASSERTION_STMT: name = "assertion"; break;
      case PRECONDITION_STMT: name = "precondition"; break;
      case POSTCONDITION_STMT: name = "postcondition"; break;
      case ASSUMPTION_STMT: name = "assumption"; break;
      default: break;
    }
  gcc_assert (name);
  return lookup_qualified_name (contracts_attribute_type_node, name,
				LOOK_want::NORMAL, false);
}

/* Return the corresponding std::experimental::contracts::semantic enum value
   for a given contract semantic SEM.  */

static tree
map_contract_semantic (contract_semantic sem)
{
  if (!contracts_semantic_type_node)
    return NULL_TREE;
  const char *name = NULL;
  switch (sem)
    {
      case CCS_IGNORE: name = "ignore"; break;
      case CCS_ASSUME: name = "assume"; break;
      case CCS_NEVER: name = "enforce"; break;
      case CCS_MAYBE: name = "observe"; break;
      default: break;
    }
  if (!name)
    return NULL_TREE;
  return lookup_qualified_name (contracts_semantic_type_node, name,
				LOOK_want::NORMAL, false);
}

/* Return true if a list of types already contains a given type.  */

static tree
type_list_find (tree list, tree type)
{
  if (!list || !type)
    return NULL_TREE;
  for (tree e = list; e; e = TREE_CHAIN (e))
    if (TREE_VALUE (e) == type)
      return e;
  return NULL_TREE;
}

/* Determine a contract's concrete semantic based on the default semantic for
   the attribute kind, and the labels specified, if any.  */

contract_semantic
compute_contract_concrete_semantic (tree_code kind, tree labels)
{
  if (!flag_contracts)
    return CCS_IGNORE;
  contract_semantic sem = contract_attribute_kind_default_semantic (kind);
  if (!labels)
    return sem;

  /* TODO: ensure any calls are outside the normal ptd workflow */
  temp_override<int> tmp_ptd(processing_template_decl, 0);

  setup_contracts_type_nodes ();
  tree attrarg = map_contract_to_attribute (kind);
  tree semarg = map_contract_semantic (sem);

  /* List of value_type members of previous labels. Currently stored as a tree
     list where the purpose is the contract_label type and the value is the
     value_type. Both the number of contract labels on a single contract
     attribute and the number of distinct value_types are expected to be low,
     but if that changes something besides a linear search may bee needed.  */
  tree value_types = NULL_TREE;

  for (tree label = labels;
      label;
      label = TREE_CHAIN (label))
    {
      if (label == error_mark_node)
	return CCS_INVALID; /* Diagnosed before.  */

      tree id = TREE_VALUE (label);
      tree contract_label = lookup_contract_label (id);
      // TODO better diagnostic locations
      if (!contract_label)
	{
	  error ("contract label %qE was not declared", id);
	  return CCS_INVALID;
	}
      if (!COMPLETE_OR_OPEN_TYPE_P (contract_label))
	{
	  error ("contract label %qD is incomplete", contract_label);
	  return CCS_INVALID;
	}

      tree value_type = lookup_qualified_name (contract_label, "value_type",
					       LOOK_want::TYPE, false);
      if (value_type && TREE_CODE (value_type) == TYPE_DECL)
	{
	  value_type = TYPE_CANONICAL (TREE_TYPE (value_type));
	  tree old = type_list_find (value_types, value_type);
	  if (old)
	    error ("contract label %qD cannot combine with %qD because they "
		   "share a value_type", contract_label, TREE_PURPOSE (old));
	  else
	    value_types = chainon (value_types, build_tree_list (contract_label,
								 value_type));
	}

      tree adjust_semantic = lookup_member (contract_label,
					    get_identifier ("adjust_semantic"),
					    /*protect=*/1,
					    /*want_type=*/false,
					    tf_warning_or_error,
					    /*afi=*/NULL);
      /* Not existing at all is not an error.  */
      if (adjust_semantic == NULL_TREE)
	continue;
      if (!attrarg || !semarg)
	{
	  error ("contract label support requires "
		 "%<#include <experimental/contracts>%>");
	  return CCS_INVALID;
	}

      vec<tree, va_gc> *args = make_tree_vector ();
      vec_safe_push (args, attrarg);
      vec_safe_push (args, semarg);

      tree call = finish_call_expr (adjust_semantic, &args,
				    /*disallow_virtual=*/true,
				    /*koenig_p=*/false,
				    /*complain=*/tf_warning_or_error);
      gcc_assert (call != error_mark_node);

      tree obj_arg = NULL_TREE;
      semarg = cxx_constant_value (call, obj_arg);
    }

  if (semarg == error_mark_node)
    return CCS_INVALID;

  /* No labels, so we still have the original CONST_DECL we looked up.  */
  if (TREE_CODE (semarg) == CONST_DECL)
    semarg = DECL_INITIAL (semarg);

  wi::tree_to_wide_ref val = wi::to_wide (semarg);
  if (val == 0) return CCS_IGNORE;
  if (val == 1) return CCS_ASSUME;
  if (val == 2) return CCS_NEVER;
  if (val == 3) return CCS_MAYBE;

  /* TODO call all the other machinery from milestone 3 */
  return CCS_INVALID;
}

