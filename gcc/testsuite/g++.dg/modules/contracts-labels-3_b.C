// { dg-do compile }
// { dg-options "-std=c++20 -fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

void fn1(int n)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

template<typename T>
void tfn1(T t)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

struct S0 {
  void fn1()
    [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
    [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
    [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  {
  }

  void fn2();
};

void S0::fn2()
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

template<typename T>
struct S1 {
  void tfn1(T t)
    [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
    [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
    [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  {
  }

  void tfn2(T t);
};

template<typename T>
void S1<T>::tfn2(T t)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

template<typename T>
struct S2 {
  template<typename U>
  void tfn1(T t, U u)
    [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
    [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
    [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
  {
  }

  template<typename U>
  void tfn2(T t, U u);
};

template<typename T>
template<typename U>
void S2<T>::tfn2(T t, U u)
  [[ pre deny_l1 : false ]] // { dg-bogus "combin" }
  [[ pre l1 deny_l1 : false ]] // { dg-error "combin" }
  [[ pre l2 deny_l1 : false ]] // { dg-bogus "combin" }
{
}

