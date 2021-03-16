// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module bar;
// { dg-module-cmi bar }

using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

namespace bar {
  export struct [[contract_label(foo_custom)]] bar_custom_label
  {
    static consteval semantic adjust_semantic(attribute, semantic s)
    {
      return s;
    }
  };
}

export struct [[contract_label(custom)]] bar_custom_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

