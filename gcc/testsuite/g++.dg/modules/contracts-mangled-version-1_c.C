// { dg-module-do run }
// { dg-additional-options "-std=c++20 -fmodules-ts -fcontracts -fcontract-role=default:ignore,ignore,ignore" }
module;
#include <cstdio>
export module baz;
import foo;

void local_use_fun(int n)
{
  fun<int>(n);
}

void local_use_fun2(int n)
{
  fun2<int>(n);
}

int main(int, char**) {
  printf("use_fun:\n");
  use_fun(-5);
  printf("local_use_fun:\n");
  local_use_fun(-5);
  return 0;
}

// ensure we have non-versioned symbols for our calls
// { dg-final { scan-assembler "_Z3funIiEvi" } }
// { dg-final { scan-assembler "_Z4fun2IiEvi" } }

