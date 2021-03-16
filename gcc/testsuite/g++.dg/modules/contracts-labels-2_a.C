// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module foo;
// { dg-module-cmi foo }

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

