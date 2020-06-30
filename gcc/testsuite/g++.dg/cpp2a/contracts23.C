// ensure a check_always_continue level assert with a failing predicate
// generates an error during runtime then continues
// { dg-do run }
// { dg-options "-std=c++2a -fcontracts -O0" }
#include <cstdio>

int constexpr f()
{
  constexpr int x = 1;
  // FIXME: should this block constexpr eval?
  [[assert check_always_continue: x > 0]];
  return x;
}

template<typename T> int k()
{
  int x = 1;
  [[assert check_always_continue: x < 0]];
  return x;
}

int main()
{
  int x = 1;
  [[assert check_always_continue: x < 0]];
  constexpr int x2 = f();
  int x3 = k<int>();

  printf ("returning from main\n");
  return 0;
}

// { dg-output "default std::handle_contract_violation called: .*.C 25 main .*(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*.C 18 k<int> .*(\n|\r\n|\r)*" }
// { dg-output "returning from main" }

