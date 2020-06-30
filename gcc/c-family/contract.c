#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "diagnostic.h"
#include "options.h"
#include "contract.h"

const int max_custom_roles = 32;
static cpp_contract_role contract_build_roles[max_custom_roles] = {
};

bool valid_configs[CCS_ALWAYS + 1][CCS_ALWAYS + 1] = {
  { 0, 0, 0, 0, 0, 0, },
  { 0, 1, 0, 0, 0, 0, },
  { 0, 1, 1, 1, 1, 1, },
  { 0, 1, 1, 1, 1, 1, },
  { 0, 1, 0, 0, 1, 0, },
  { 0, 1, 0, 0, 0, 1, },
};

void
cpp_validate_role (cpp_contract_role *role)
{
  gcc_assert (role);
  if (!unchecked_contract_p(role->axiom_semantic))
    error ("axiom contract semantic must be %<assume%> or %<ignore%>");

  if (!valid_configs[role->default_semantic][role->audit_semantic] )
    warning (0, "the %<audit%> semantic should be at least as strong as "
		"the %<default%> semantic");
}

contract_semantic
cpp_lookup_concrete_semantic (char *name)
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
  if (strcmp (name, "check_always_continue") == 0
      || strcmp (name, "always") == 0
      || strcmp (name, "continue") == 0)
    return CCS_ALWAYS;
  error ("'%s' is not a valid explicit concrete semantic", name);
  return CCS_INVALID;
}

cpp_contract_role *
cpp_get_contract_role (const char *name)
{
  for (int i = 0; i < max_custom_roles; ++i)
  {
    cpp_contract_role potential = contract_build_roles[i];
    if (potential.name == NULL) continue;
    if (strcmp (potential.name, name) != 0) continue;
    return contract_build_roles + i;
  }
  if (strcmp (name, "default") == 0 || strcmp (name, "review") == 0)
    {
      cpp_setup_default_contract_role (false);
      return cpp_get_contract_role (name);
    }
  return NULL;
}

cpp_contract_role *
cpp_add_contract_role (const char *name,
		       enum contract_semantic des,
		       enum contract_semantic aus,
		       enum contract_semantic axs,
		       bool update)
{
  for (int i = 0; i < max_custom_roles; ++i)
    {
      if (contract_build_roles[i].name != NULL
	  && strcmp (contract_build_roles[i].name, name) != 0)
	continue;
      if (contract_build_roles[i].name != NULL && !update)
        return contract_build_roles + i;
      contract_build_roles[i].name = name;
      contract_build_roles[i].default_semantic = des;
      contract_build_roles[i].audit_semantic = aus;
      contract_build_roles[i].axiom_semantic = axs;
      return contract_build_roles + i;
    }
  return NULL;
}

enum contract_build_level { OFF, DEFAULT, AUDIT };
extern bool flag_contract_continuation_mode;
extern bool flag_contract_assumption_mode;
extern int flag_contract_build_level;

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
cpp_setup_default_contract_role (bool update)
{
  contract_semantic check = get_concrete_check ();
  contract_semantic axiom = get_concrete_axiom_semantic ();
  switch (flag_contract_build_level)
  {
    case OFF:
      cpp_add_contract_role ("default", CCS_IGNORE, CCS_IGNORE, axiom, update);
      cpp_add_contract_role ("review", CCS_IGNORE, CCS_IGNORE, CCS_IGNORE, update);
      break;
    case DEFAULT:
      cpp_add_contract_role ("default", check, CCS_IGNORE, axiom, update);
      cpp_add_contract_role ("review",
			     flag_contract_continuation_mode
			       ? CCS_ALWAYS
			       : CCS_NEVER,
			     CCS_IGNORE, CCS_IGNORE, update);
      break;
    case AUDIT:
      cpp_add_contract_role ("default", check, check, axiom, update);
      cpp_add_contract_role ("review", check, check, CCS_IGNORE, update);
      break;
  }
}

