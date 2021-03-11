// Basic test to ensure that guarded templates correctly serialize and
// deserialize their contracts through the CMI.
// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe -Kpost=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module foo;
// { dg-module-cmi foo }

using std::experimental::contracts::violation;

export int violation_count{0};
export void handle_contract_violation(const violation &violation)
{
  violation_count++;
  printf("violation_count: %d\n", violation_count);
}

export int nontemplate(int n)
  [[ pre: n > 0 ]]
  [[ post [r]: r > 0 ]]
{
  return -n;
}

export
template<typename T>
T fn(T n)
  [[ pre: n > 0 ]]
  [[ post [r]: r > 0 ]]
{
  printf("%s(%d)\n", __FUNCTION__, n);
  return n;
}

export
template<typename T>
void void_fn(T n)
  [[ pre: n < 0 ]]
{
  printf("%s(%d)\n", __FUNCTION__, n);
}

