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

/* Assertion role info.

   FIXME: Why is this prefixed cpp?  */
struct cpp_contract_role
{
  const char *name;
  enum contract_semantic default_semantic;
  enum contract_semantic audit_semantic;
  enum contract_semantic axiom_semantic;
};

typedef cpp_contract_role contract_role;


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

  void set (contract_level level, contract_role *role = NULL)
  {
    kind = cm_dynamic;
    contract_configuration cc;
    cc.level = level;
    cc.role = role;
    u.config = cc;
  }

  void set_role (contract_role *role)
  {
    gcc_assert (kind == cm_dynamic);
    u.config.role = role;
  }

  void set (contract_semantic semantic)
  {
    kind = cm_explicit;
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

extern cpp_contract_role *cpp_get_contract_role	(const char *);
extern cpp_contract_role *cpp_add_contract_role	(const char *,
						 enum contract_semantic,
						 enum contract_semantic,
						 enum contract_semantic,
						 bool = true);
extern void cpp_validate_role			(cpp_contract_role *);
extern enum contract_semantic cpp_lookup_concrete_semantic (char *);
extern void cpp_setup_default_contract_role	(bool = true);

/* Returns the default role.  */

inline contract_role *
get_default_contract_role ()
{
  return cpp_get_contract_role ("default");
}

#endif /* ! GCC_CP_CONTRACT_H */
