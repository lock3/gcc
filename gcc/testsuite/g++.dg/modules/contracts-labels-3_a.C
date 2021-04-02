// { dg-do compile }
// { dg-options "-std=c++20 -fmodules-ts" }
module;
#include <experimental/contracts>
export module foo;
// { dg-module-cmi foo }

export struct [[contract_label(l1)]] l1_label {};
export struct [[contract_label(l2)]] l2_label {};

export struct [[contract_label(deny_l1)]] deny_l1_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return !(std::same_as<Ts, l1_label> || ...);
  }
};

