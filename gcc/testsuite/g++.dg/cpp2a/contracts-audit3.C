// ensure audit checks do not run under build-level=default (same as
// contracts-audit2, but using the implicit build-level)
// { dg-do run }
// { dg-options "-std=c++20 -Kassert=enforce" }
#include <experimental/contracts>

int main(int, char **)
{
  int n = -1;
  [[ assert audit : n > 2 ]];
  static_assert(!std::experimental::contracts::is_audit_build());
}

