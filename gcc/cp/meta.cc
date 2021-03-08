/* Perform the semantic phase of parsing, i.e., the process of
   building tree structure, checking semantic consistency, and
   building RTL.  These routines are used both during actual parsing
   and during the instantiation of template functions.

   Copyright (C) 1998-2020 Free Software Foundation, Inc.
   Written by Andrew Sutton (asutton@lock3software.com).

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "cp-tree.h"
#include "stmt.h"
#include "stor-layout.h"
#include "c-family/c-objc.h"
#include "intl.h"
#include "tree-iterator.h"
#include "omp-general.h"
#include "convert.h"
#include "stringpool.h"
#include "attribs.h"
#include "gomp-constants.h"
#include "print-tree.h"
#include "cxx-config.h"

/* Returns true if any operands are type-dependent or dependent types.  */

static bool
any_dependent_p (const_tree args)
{
  for (int i = 0; i != TREE_VEC_LENGTH (args); ++i)
    {
      tree t = TREE_VEC_ELT (args, i);
      if (TYPE_P (t) && TYPE_DEPENDENT_P (t))
	return true;
      else if (type_dependent_expression_p (t))
	return true;
    }
  return false;
}

/* Return a valid conversion of ARG to TYPE if it exists, or error_mark_node
   and issue a diagnostic if there is no valid conversion path.  */
static tree
perform_conversion (tree type, tree arg)
{
  tree from_type = TREE_TYPE (arg);
  conversion *c = good_conversion (type, from_type, arg, LOOKUP_IMPLICIT,
				   tf_warning_or_error);
  if (!c)
    {
      complain_about_bad_argument (EXPR_LOCATION (arg), from_type, type,
				   NULL_TREE, 0);
      return error_mark_node;
    }

  tree conv = perform_implicit_conversion (type, arg, tf_warning_or_error);
  return conv;
}

/* Check that metafunction operands correspond to their specification and
   return the expression's type.

   FIXME: We probably need to perform standard conversions on many of
   these operands.  If we actually perform conversions, we're going to be
   rewriting the argument vector.  */

static tree
check_metafunction (metafunction_kind k, tree args)
{
  switch (k)
    {
    case mfk_getenv:
      {
	gcc_assert (TREE_VEC_LENGTH (args) == 1);
	if (NULLPTR_TYPE_P (TREE_TYPE (TREE_VEC_ELT (args, 0))))
	  {
	    error ("operand must be non-null");
	    return error_mark_node;
	  }

	/* Ensure the argument is convertible to const char *.  */
	tree conv = perform_conversion (const_string_type_node, 
					TREE_VEC_ELT (args, 0));
	if (conv == error_mark_node)
	  return error_mark_node;
	TREE_VEC_ELT (args, 0) = conv;
	return const_string_type_node;
      }

    case mfk_maybe_getenv:
      {
	gcc_assert (TREE_VEC_LENGTH (args) == 3);

	if (NULLPTR_TYPE_P (TREE_TYPE (TREE_VEC_ELT (args, 0))))
	  {
	    error ("first operand must be non-null");
	    return error_mark_node;
	  }
	/* Ensure the argument is convertible to const char *.  */
	tree conv1 = perform_conversion (const_string_type_node,
					 TREE_VEC_ELT (args, 0));
	if (conv1 == error_mark_node)
	  return error_mark_node;
	TREE_VEC_ELT (args, 0) = conv1;

	tree t = TREE_VEC_ELT (args, 1);
	if (!INTEGRAL_OR_ENUMERATION_TYPE_P (t)
	    && !same_type_p (t, const_string_type_node))
	  {
	    error ("second operand must be an integral or enumeration type");
	    return error_mark_node;
	  }

	/* Ensure the default argument is convertible to the type T.  */
	tree conv2 = perform_conversion (t, TREE_VEC_ELT (args, 2));
	if (conv2 == error_mark_node)
	  return error_mark_node;
	TREE_VEC_ELT (args, 2) = conv2;
	return t;
      }

    default:
      break;
    }
  gcc_unreachable ();
}

/* Perform semantic analysis on a metafunction.  */

cp_expr
finish_metafunction_expression (location_t loc, metafunction_kind k, tree args)
{
  /* TODO: Check that these are only present in consteval functions,
     or initializers of constexpr and constinit variables.  */

  tree kind = build_int_cst (sizetype, k);

  tree expr;
  if (any_dependent_p (args))
    expr = build2 (METAFUNCTION_EXPR, NULL_TREE, kind, args);
  else
    {
      tree type = check_metafunction (k, args);
      if (type == error_mark_node)
	return error_mark_node;
      expr = build2 (METAFUNCTION_EXPR, type, kind, args);
    }

  protected_set_expr_location (expr, loc);
  if (any_dependent_p (args))
    return expr;

  /* Metafunction expressions are immediate invocations.  */
  bool immediate_invocation_p
    = cp_unevaluated_operand == 0
      && (current_function_decl == NULL_TREE
	  || !DECL_IMMEDIATE_FUNCTION_P (current_function_decl))
      && (current_binding_level->kind != sk_function_parms
	  || !current_binding_level->immediate_fn_ctx_p);
  if (!immediate_invocation_p)
    return expr;

  tree obj_arg = NULL_TREE;
  expr = cxx_constant_value (expr, obj_arg);

  return expr;
}

/* Instantiate the metafunction operand. T is either a type or expression.  */

static tree
tsubst_metafunction_operand (tree t, tree args,
			     tsubst_flags_t complain, tree in_decl)
{
  if (!TYPE_P (t))
    return tsubst_expr (t, args, complain, in_decl, false);
  else
    return tsubst (t, args, complain, in_decl);
}

/* Instantiate the metafunction expression.  */

tree
tsubst_metafunction_expression (tree t, tree args,
				tsubst_flags_t complain, tree in_decl)
{
  /* Substitute through arguments.  */
  tree olds = METAFUNCTION_EXPR_ARGS (t);
  tree news = make_tree_vec (TREE_VEC_LENGTH (olds));
  for (int i = 0; i < TREE_VEC_LENGTH (olds); ++i)
    TREE_VEC_ELT (news, i) =
      tsubst_metafunction_operand (TREE_VEC_ELT (olds, i), args,
				   complain, in_decl);

  /* Rebuild the node.  */
  location_t loc = EXPR_LOCATION (t);
  metafunction_kind k = METAFUNCTION_EXPR_KIND (t);
  return finish_metafunction_expression (loc, k, news);
}
