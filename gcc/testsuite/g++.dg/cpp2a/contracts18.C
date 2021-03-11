// check that a valid program using assertions compiles and runs
//   ensure a symbolic contract with a failing predicate doesn't prevent a
//   successful run (symbolic contracts are never checked at runtime)
// { dg-do run }
// { dg-options "-std=c++20" }
#include <experimental/contracts>

struct [[contract_label(role_custom)]] role_custom_label {};

int main()
{
  int x = 1;
  [[assert symbolic: x < 0]];
  [[assert role_custom: x > 0]];
  [[assert audit role_custom: x < 0]];
  return 0;
}

