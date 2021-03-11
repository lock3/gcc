// Small test to ensure that the level and role information printed by various
// contract configurations is correct.
// { dg-do run }
// { dg-options "-std=c++2a -fcontracts -fbuild-level=audit -Kassert=observe -Kpost=observe" }
#include <experimental/contracts>

struct [[contract_label(foo)]] foo_label {};
struct [[contract_label(bar)]] bar_label {};

int ifun(int n)
  [[ post default [r]: false ]]
{
  return -n;
}

int main(int, char **)
{
  [[ assert default: false ]];
  [[ assert: false ]];
  [[ assert audit: false ]];
  [[ assert default foo: false ]];
  [[ assert foo: false ]];
  [[ assert audit foo: false ]];
  [[ assert default foo bar: false ]];
  [[ assert foo bar: false ]];
  [[ assert audit foo bar: false ]];
  ifun(5);
  return 0;
}

// { dg-output "default std::handle_contract_violation called: .*main false 0 3 1 : default(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 0 :(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 1 : audit(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 2 : default foo(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 1 : foo(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 2 : audit foo(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 3 : default foo bar(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 2 : foo bar(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*main false 0 3 3 : audit foo bar(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*ifun false 2 3 1 : default(\n|\r\n|\r)*" }

