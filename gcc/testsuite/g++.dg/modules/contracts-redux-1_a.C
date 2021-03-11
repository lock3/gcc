// Basic test to ensure that contract_labels are correctly reregistered on
// module import
// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
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

using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

namespace foo {
  export struct [[contract_label(foo_custom)]] custom_label
  {
    static consteval semantic adjust_semantic(attribute, semantic s)
    {
      return s;
    }
  };
}

export struct [[contract_label(custom)]] custom_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

export int foo_fn(int n)
  [[ pre foo_custom custom: n > 0 ]]
{
  printf("%s(%d)\n", __FUNCTION__, n);
  return n;
}

