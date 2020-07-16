// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=full" }

int fun1(int n)
  [[ pre: n != 0 ]]
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: false ]] // { dg-warning "always .false." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}
int fun2(int n)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}
int fun3(int n)
  [[ post r: false ]] // { dg-warning "always .false." }
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}

template<typename T>
T tfun(T t)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}
template<typename T>
T tfun2(T t)
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}

template<typename T>
auto tfun_auto(T t)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}
template<typename T>
auto tfun_auto2(T t)
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}

struct Foo {
  Foo(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }

  static void sfun(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }
  static void sfun2(int n)
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }

  void fun(int n)
    [[ pre: n != 0 ]]
    [[ pre: memb != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }

  int memb{0};
};

template<typename T>
struct TFoo {
  TFoo(T n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }

  static void sfun(T n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }
  static void sfun2(T n)
    [[ pre: false ]] // { dg-warning "always .false." }
    [[ pre: n != 0 ]]
  {
  }

  void fun(T n)
    [[ pre: n != 0 ]]
    [[ pre: memb != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
  }

  T memb{0};
};

int main(int, char**) {
  constexpr int n = 0;
  [[ assert: true ]]; // { dg-warning "always .true." }
  [[ assert: false ]]; // { dg-warning "always .false." }
  [[ assert: n == 0 ]]; // { dg-warning "always .true." }
  fun1(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  fun2(0); // { dg-warning "precondition .false. is never satisfied here" }
  fun3(0); // { dg-warning "postcondition .false. is never satisfied here" }
  fun1(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  fun2(n); // { dg-warning "precondition .false. is never satisfied here" }
  fun3(n); // { dg-warning "postcondition .false. is never satisfied here" }

  tfun(1); // { dg-warning "precondition .false. is never satisfied here" }
  tfun(1.5); // { dg-warning "precondition .false. is never satisfied here" }
  tfun2(1); // { dg-warning "postcondition .false. is never satisfied here" }
  tfun2(1.5); // { dg-warning "postcondition .false. is never satisfied here" }

  tfun_auto(1); // { dg-warning "precondition .false. is never satisfied here" }
  tfun_auto(1.5); // { dg-warning "precondition .false. is never satisfied here" }
  tfun_auto2(1); // { dg-warning "postcondition .false. is never satisfied here" }
  tfun_auto2(1.5); // { dg-warning "postcondition .false. is never satisfied here" }

  Foo f1{0};
  Foo f2{n};

  f1.fun(0);
  f1.fun(n);

  Foo::sfun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  Foo::sfun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  Foo::sfun2(0); // { dg-warning "precondition .false. is never satisfied here" }
  Foo::sfun2(n); // { dg-warning "precondition .false. is never satisfied here" }

  TFoo<int> ft_int1{0};
  TFoo<int> ft_int2{n};

  ft_int1.fun(0);
  ft_int1.fun(n);

  TFoo<int>::sfun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  TFoo<int>::sfun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  TFoo<int>::sfun2(0); // { dg-warning "precondition .false. is never satisfied here" }
  TFoo<int>::sfun2(n); // { dg-warning "precondition .false. is never satisfied here" }

  return 0;
}

