// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>

struct [[contract_label(l1)]] l1_label {};
struct [[contract_label(l2)]] l2_label {};

struct [[contract_label(deny_l1)]] deny_l1_label
{
  template<typename... Ts>
  static consteval bool combines_with()
  {
    return !(std::same_as<Ts, l1_label> || ...);
  }
};

template<typename T>
void fn1(T t)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

template<typename T>
struct S1 {
  void fn1(T t)
    [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
    [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
    [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  {
  }

  void fn2(T t);
};

template<typename T>
void S1<T>::fn2(T t)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

template<typename T>
struct S2 {
  template<typename U>
  void fn1(T t, U u)
    [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
    [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
    [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  {
  }

  template<typename U>
  void fn2(T t, U u);
};

template<typename T>
template<typename U>
void S2<T>::fn2(T t, U u)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

