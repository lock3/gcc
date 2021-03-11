// check that a valid program using assertions compiles and runs
//   ensure a symbolic contract with a failing predicate doesn't prevent a
//   successful run (symbolic contracts are never checked at runtime)
// { dg-do run }
// { dg-options "-std=c++2a -fbuild-level=audit -Kassert=observe" }
#include <experimental/contracts>

struct [[contract_label(role_custom)]] role_custom_label {};

int main()
{
  int x = 10;
  [[assert symbolic: x < 0]];
  [[assert role_custom: x < 0]];
  [[assert audit role_custom: x < 1]];
  [[assert symbolic role_custom: x < 1]];
  return 0;
}

// { dg-output "default std::handle_contract_violation called: .*.C 14 main .*(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*.C 15 main .*(\n|\r\n|\r)*" }

