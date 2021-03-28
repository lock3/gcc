/* Definitions for C++ contract levels.  Implements functionality described in
   the working draft version of contracts, P1290, P1332, and P1429.
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

#ifndef GCC_CP_CONTRACT_H
#define GCC_CP_CONTRACT_H

#include "tree.h"
#include "hash-map.h"

/* The concrete semantics determine the behavior of a contract.  */

enum contract_semantic
{
  CCS_INVALID,
  CCS_IGNORE,
  CCS_ASSUME,
  CCS_ENFORCE,
  CCS_OBSERVE
};

/* True if the contract is unchecked.  */

inline bool
unchecked_contract_p (contract_semantic cs)
{
  return cs == CCS_IGNORE || cs == CCS_ASSUME;
}

/* True if the contract is checked.  */

inline bool
checked_contract_p (contract_semantic cs)
{
  return cs >= CCS_ENFORCE;
}

/* Map a source level semantic or level name to its value, or invalid.  */
extern contract_semantic map_contract_semantic	(const char *);

/* Check if an attribute is a cxx contract attribute.  */
extern bool cxx_contract_attribute_p (const_tree);

/* Replace any references in CONTRACT's CONDITION to SRC's parameters with
   references to DST's parameters.  */
extern void remap_contract (tree src, tree dst, tree contract);

/* Return the TYPE associated with a contract_label NAME, or NULL_TREE if no
   such associated type exists.  */
extern tree lookup_contract_label (tree name);

/* Associate TYPE with the contract_label NAME, erroring if NAME is already
   associated with a different type.  */
extern void define_contract_label (tree name, tree type);

/* Return an iterator into the contract_labels hash_map.  */
extern hash_map<tree, tree>::iterator contract_labels_begin ();
extern hash_map<tree, tree>::iterator contract_labels_end ();

/* Determine the final concrete semantic given an contract attribute kind and
   sequence of labels.  */

extern contract_semantic compute_contract_concrete_semantic (tree_code, tree);

#endif /* ! GCC_CP_CONTRACT_H */
