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

