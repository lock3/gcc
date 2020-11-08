// Test that template contracts can be reintpreted using the appropriate flag.
// contracts-3 is same test with a different link order to ensure we're not
// getting lucky with weak symbols and link order.
// { dg-additional-options "-fmodules-ts -fcontracts -fcontract-versioning" }
module;
#include <cstdio>
#include <contract>
export module foo;
// { dg-module-cmi foo }

export int violation_count{0};
export void handle_contract_violation(const std::contract_violation &violation)
{
  violation_count++;
  printf("violation_count: %d\n", violation_count);
}

export template<typename T>
T fn_t(T t)
  [[ pre: t > 0 ]]
  [[ pre audit %custom: t > 0 ]]
{
  printf("%s(%d)\n", "fn_t", t); // FIXME __FUNCTION__
  return t;
}

export int fn_int(int n);

export template<typename T>
T fn_t_inner(T t)
{
  [[ assert: t > 0 ]];
  [[ assert audit %custom: t > 0 ]];
  printf("%s(%d)\n", "fn_t", t);
  return t;
}

export int fn_int_inner(int n);

