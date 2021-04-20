// ensure that an enforce semantic contract violation is treated as noreturn
// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -O3" }
#include "contracts-literal-semantic.h"

const char *f1(int n)
{
  [[assert check_never_continue: n == 0]];
  if (n == 5)
    return "OPTIMIZED OUT 1";
  return "foo";
}

const char *f2(int n)
{
  [[assert check_maybe_continue: n == 0]];
  if (n == 5)
    return "INCLUDED 1";
  return "foo";
}

const char *f3(int n)
{
  [[assert check_maybe_continue: n == 0]];
  if (n == 5)
    return "INCLUDED 2";
  [[assert check_never_continue: n == 0]];
  if (n == 6)
    return "OPTIMIZED OUT 2";
  return "foo";
}

const char *f4(int n)
{
  [[assert check_never_continue: n == 0]];
  if (n == 5)
    return "OPTIMIZED OUT 3";
  [[assert check_maybe_continue: n == 0]];
  if (n == 6)
    return "OPTIMIZED OUT 4";
  return "foo";
}

// { dg-final { scan-assembler-not "OPTIMIZED OUT 1" } }
// { dg-final { scan-assembler-not "OPTIMIZED OUT 2" } }
// { dg-final { scan-assembler-not "OPTIMIZED OUT 3" } }
// { dg-final { scan-assembler-not "OPTIMIZED OUT 4" } }
// { dg-final { scan-assembler "INCLUDED 1" } }
// { dg-final { scan-assembler "INCLUDED 2" } }

