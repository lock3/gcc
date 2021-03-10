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
#include "diagnostic.h"
#include "options.h"
#include "cxx-config.h"

/* The build environment. Maps C-string keys to C-string values. Keys are
   never mapped to the null string.  */

static hash_map<nofree_string_hash, const char*> build_env;

void
define_knob (const char* arg)
{
  /* Parse the key/value from a copy of ARG.  */  
  char* str = xstrdup (arg);
  char* key;
  char* value;
  char* eq = strchr (str, '=');
  if (!eq)
    {
      key = str;
      value = str + strlen (str);
    }
  else
    {
      int n = eq - str;
      str[n] = 0;
      key = str;
      value = str + n + 1;
    }

  /* TODO: Verify the the key is either "id" or "ns::id" where both ns and
     id are identifiers.  */

  /* Try inserting the key/value into the build environment.  */
  bool existed;
  const char*& entry = build_env.get_or_insert (key, &existed);
  if (existed)
    {
      if (*entry)
	error ("configuration knob %qs previously defined as %qs", key, entry);
      else
	error ("configuration knob %qs previously defined", key);
    }
  else
    entry = value;
}

/* Define a knob if it doesn't already exist.  */

static void
define_knob_if_not_exists (const char *key, const char *value)
{
  bool existed;
  const char*& entry = build_env.get_or_insert (key, &existed);
  if (existed)
    return;
  entry = value;
}

/* Define any default knobs if they don't already have a value given.  */

void
define_default_knobs ()
{
  define_knob_if_not_exists ("assert", "enforce");
  define_knob_if_not_exists ("pre", "enforce");
  define_knob_if_not_exists ("post", "enforce");
  define_knob_if_not_exists ("assume", "ignore");
}

/* Returns the C-string assocated with KEY or null if KEY is not in the
   build environment.  */

const char*
lookup_knob (const char* key)
{
  const char** value = build_env.get(key);
  if (!value)
    return nullptr;
  return *value;
}

/* Print all currently defined knobs to stdout.  */

void
dump_knobs ()
{
  for (auto it = build_env.begin(); it != build_env.end(); ++it)
    inform (UNKNOWN_LOCATION, "knob \"%s\": \"%s\"", (*it).first, (*it).second);
}

