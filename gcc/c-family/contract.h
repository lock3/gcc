/* Definitions for C++ contract levels
   Copyright (C) 1987-2018 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@cygnus.com)

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

/* Contract levels approximate the complexity of the expression.  */

enum contract_level
{
  CONTRACT_INVALID,
  CONTRACT_DEFAULT,
  CONTRACT_AUDIT,
  CONTRACT_AXIOM
};

/* The concrete semantics determine the behavior of a contract.  */

enum contract_semantic
{
  CCS_INVALID,
  CCS_IGNORE,
  CCS_ASSUME,
  CCS_NEVER,
  CCS_MAYBE,
  CCS_ALWAYS
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
  return cs >= CCS_NEVER;
}

enum contract_continuation
{
  NEVER_CONTINUE,
  MAYBE_CONTINUE,
  ALWAYS_CONTINUE
};

/* Assertion role info.  */
struct contract_role
{
  const char *name;
  contract_semantic default_semantic;
  contract_semantic audit_semantic;
  contract_semantic axiom_semantic;
};

/* Information for configured contract semantics.  */

struct contract_configuration
{
  contract_level level;
  contract_role* role;
};

/* A contract mode contains information used to derive the checking 
   and assumption semantics of a contract. This is either a dynamic
   configuration, meaning it derives from the build mode, or it is 
   explicitly specified.  */

struct contract_mode
{
  contract_mode () : kind(cm_invalid) {}
  contract_mode (contract_level level, contract_role *role = NULL) : kind(cm_dynamic)
  {
    contract_configuration cc;
    cc.level = level;
    cc.role = role;
    u.config = cc;
  }
  contract_mode (contract_semantic semantic) : kind(cm_explicit)
  {
    u.semantic = semantic;
  }

  contract_level get_level () const
  {
    gcc_assert (kind == cm_dynamic);
    return u.config.level;
  }

  contract_role *get_role () const
  {
    gcc_assert (kind == cm_dynamic);
    return u.config.role;
  }

  contract_semantic get_semantic () const
  {
    gcc_assert (kind == cm_explicit);
    return u.semantic;
  }

  enum { cm_invalid, cm_dynamic, cm_explicit } kind;

  union
  {
    contract_configuration config;
    contract_semantic semantic;
  } u;
};

extern contract_role *get_contract_role	(const char *);
extern contract_role *add_contract_role	(const char *,
					 contract_semantic,
					 contract_semantic,
					 contract_semantic,
					 bool = true);
extern void validate_contract_role	(contract_role *);
extern void setup_default_contract_role	(bool = true);
extern contract_semantic lookup_concrete_semantic (char *);

/* Map a source level semantic or level name to its value, or invalid.  */
extern contract_semantic map_contract_semantic	(const char *);
extern contract_level map_contract_level	(const char *);

/* Returns the default role.  */

inline contract_role *
get_default_contract_role ()
{
  return get_contract_role ("default");
}

#endif /* ! GCC_CP_CONTRACT_H */
