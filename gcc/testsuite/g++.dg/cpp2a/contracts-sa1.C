// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=full" }

int fun(int n)
  [[ pre: n != 0 ]]
  [[ pre: true ]]
  [[ pre: false ]]
  [[ post r: true ]]
  [[ post r: false ]]
{
  return n;
}

template<typename T>
T tfun(T t)
  [[ pre: false ]]
  [[ pre: t > 0 ]]
  [[ post: false ]]
{
  return t;
}

template<typename T>
auto tfun_auto(T t)
  [[ pre: false ]]
  [[ pre: t > 0 ]]
  [[ post: false ]]
{
  return t;
}

struct Foo {
  Foo(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]]
  {
  }

  static void sfun(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]]
  {
  }

  void fun(int n)
    [[ pre: n != 0 ]]
    [[ pre: memb != 0 ]]
    [[ pre: false ]]
  {
  }

  int memb{0};
};

template<typename T>
struct TFoo {
  TFoo(T n)
    [[ pre: n != 0 ]]
    [[ pre: false ]]
  {
  }

  static void sfun(T n)
    [[ pre: n != 0 ]]
    [[ pre: false ]]
  {
  }

  void fun(T n)
    [[ pre: n != 0 ]]
    [[ pre: memb != 0 ]]
    [[ pre: false ]]
  {
  }

  T memb{0};
};

int main(int, char**) {
  constexpr int n = 0;
  [[ assert: true ]]; // { dg-warning "always .true." }
  [[ assert: false ]]; // { dg-warning "always .false." }
  [[ assert: n == 0 ]]; // { dg-warning "always .true." }
  fun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }
  fun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }

  tfun(1); // { dg-warning "precondition .false. is never satisfied here" }
  tfun(1.5); // { dg-warning "precondition .false. is never satisfied here" }

  tfun_auto(1); // { dg-warning "precondition .false. is never satisfied here" }
  tfun_auto(1.5); // { dg-warning "precondition .false. is never satisfied here" }

  Foo f1{0};
  Foo f2{n};

  f1.fun(0);
  f1.fun(n);

  Foo::sfun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }
  Foo::sfun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }

  TFoo<int> ft_int1{0};
  TFoo<int> ft_int2{n};

  ft_int1.fun(0);
  ft_int1.fun(n);

  TFoo<int>::sfun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }
  TFoo<int>::sfun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  // { dg-warning "precondition .false. is never satisfied here" "" { target *-*-* } .-1 }

  return 0;
}

