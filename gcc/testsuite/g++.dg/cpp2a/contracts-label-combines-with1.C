// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>

struct [[contract_label(l1)]] l1_label {};
struct [[contract_label(l2)]] l2_label {};

struct [[contract_label(allow)]] allow_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return true;
  }

  template<typename T>
  static consteval int combines_with(int n)
  {
    return false;
  }
};

struct [[contract_label(deny)]] deny_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return false;
  }
};

struct [[contract_label(deny_l1)]] deny_l1_label
{
  template<typename T>
  static consteval bool combines_with(int n)
  {
    return !std::same_as<T, l1_label>;
  }

  template<typename... Ts>
  static consteval bool combines_with()
  {
    return !(std::same_as<Ts, l1_label> || ...);
  }
};

struct [[contract_label(deny_even)]] deny_even_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return sizeof...(Ts) % 2 == 1;
  }
};

struct [[contract_label(deny_odd)]] deny_odd_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return sizeof...(Ts) % 2 == 0;
  }
};

struct [[contract_label(bad)]] bad_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
  } // { dg-error "no return" }
};

struct [[contract_label(bad2)]] bad2_label
{
  static consteval bool combines_with()
  {
    return false;
  }
};

struct [[contract_label(nt)]] nt_label
{
  static consteval bool combines_with()
  {
    return true;
  }
};

int f1(int n)
  [[ pre allow : false ]] // { dg-bogus "combin" }
  [[ pre l1 allow : false ]] // { dg-bogus "combin" }
  [[ pre l2 allow : false ]] // { dg-bogus "combin" }
  [[ pre l1 l2 allow : false ]] // { dg-bogus "combin" }
  [[ pre deny : false ]] // { dg-error "combin" }
  [[ pre l1 deny : false ]] // { dg-error "combin" }
  [[ pre l2 deny : false ]] // { dg-error "combin" }
  [[ pre l1 l2 deny : false ]] // { dg-error "combin" }
  [[ pre bad : false ]] // { dg-error ".constexpr." }
  [[ pre bad2 : false ]] // { dg-error "" }
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre deny_even : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_even : false ]] // { dg-error "combin" }
  [[ pre l2 deny_even : false ]] // { dg-error "combin" }
  [[ pre l1 l2 deny_even : false ]] // { dg-bogus "combin" }
  [[ pre deny_odd : false ]] // { dg-error "combin" }
  [[ pre l1 deny_odd : false ]] // { dg-bogus "combin" }
  [[ pre l2 deny_odd : false ]] // { dg-bogus "combin" }
  [[ pre l1 l2 deny_odd : false ]] // { dg-error "combin" }
  [[ pre nt : false ]] // { dg-error "no matching function" }
  [[ pre l1 nt : false ]] // { dg-error "no matching function" }
  [[ pre l2 nt : false ]] // { dg-error "no matching function" }
  [[ pre l1 l2 nt : false ]] // { dg-error "no matching function" }
{
  return -n;
}

