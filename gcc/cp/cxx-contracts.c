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

contract_semantic
map_contract_semantic (const char *ident)
{
  if (strcmp (ident, "ignore") == 0)
    return CCS_IGNORE;
  else if (strcmp (ident, "assume") == 0)
    return CCS_ASSUME;
  else if (strcmp (ident, "enforce") == 0)
    return CCS_ENFORCE;
  else if (strcmp (ident, "observe") == 0)
    return CCS_OBSERVE;
  return CCS_INVALID;
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
      case CCS_ENFORCE: name = "enforce"; break;
      case CCS_OBSERVE: name = "observe"; break;
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

      tree loc = TREE_VALUE (label);
      tree id = tree_strip_any_location_wrapper (loc);
      tree contract_label = lookup_contract_label (id);
      if (!contract_label)
	{
	  error_at (EXPR_LOCATION (loc), "contract label %qE was not declared",
		    id);
	  return CCS_INVALID;
	}
      if (!COMPLETE_OR_OPEN_TYPE_P (contract_label))
	{
	  error_at (EXPR_LOCATION (loc), "contract label %qD is incomplete",
		    contract_label);
	  return CCS_INVALID;
	}

      tree value_type = lookup_qualified_name (contract_label, "value_type",
					       LOOK_want::TYPE, false);
      if (value_type && TREE_CODE (value_type) == TYPE_DECL)
	{
	  value_type = TYPE_CANONICAL (TREE_TYPE (value_type));
	  tree old = type_list_find (value_types, value_type);
	  if (old)
	    error_at (EXPR_LOCATION (loc),
		      "contract label %qD cannot combine with %qD because they "
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
	  error_at (EXPR_LOCATION (loc), "contract label support requires "
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
  if (val == 2) return CCS_ENFORCE;
  if (val == 3) return CCS_OBSERVE;

  /* TODO call all the other machinery from milestone 3 */
  return CCS_INVALID;
}

