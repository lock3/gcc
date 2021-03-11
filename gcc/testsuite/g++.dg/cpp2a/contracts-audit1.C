// ensure audit checks run under -fbuild-level=audit
// { dg-do run }
// { dg-options "-std=c++20 -fcontracts -fbuild-level=audit -Kassert=observe" }
#include <experimental/contracts>

int main(int, char **)
{
  int n = -1;
  [[ assert : n > 0 ]];
  [[ assert default : n > 1 ]];
  [[ assert audit : n > 2 ]];
  static_assert(std::experimental::contracts::is_audit_build());
}

// { dg-output "default std::handle_contract_violation called: .*.C 9 .*(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*.C 10 .*(\n|\r\n|\r)*" }
// { dg-output "default std::handle_contract_violation called: .*.C 11 .*(\n|\r\n|\r)*" }

