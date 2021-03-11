// ensure audit checks do not run under build-level=default
// { dg-do run }
// { dg-options "-std=c++20 -fbuild-level=default -Kassert=enforce" }
#include <experimental/contracts>

int main(int, char **)
{
  int n = -1;
  [[ assert audit : n > 2 ]];
  static_assert(!std::experimental::contracts::is_audit_build());
}

