// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts -Wconstant-postconditions" }

constexpr int fun1(int n)
  [[ pre: n != 0 ]]
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: false ]] // { dg-warning "always .false." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}
constexpr int fun2(int n)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}
constexpr int fun3(int n)
  [[ pre: true ]] // { dg-warning "always .true." }
  [[ post r: false ]] // { dg-warning "always .false." }
  [[ post r: true ]] // { dg-warning "always .true." }
{
  return n;
}

template<typename T>
constexpr T tfun(T t)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}
template<typename T>
constexpr T tfun2(T t)
  [[ pre: t > 0 ]]
  [[ post: t < 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}

template<typename T>
constexpr auto tfun_auto(T t)
  [[ pre: false ]] // { dg-warning "always .false." }
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}
template<typename T>
constexpr auto tfun_auto2(T t)
  [[ pre: t > 0 ]]
  [[ post: false ]] // { dg-warning "always .false." }
{
  return t;
}

struct Foo {
  Foo(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
    : memb{n}
  {
  }

  static int sfun(int n)
    [[ pre: n != 0 ]]
    [[ pre: false ]] // { dg-warning "always .false." }
  {
    return n;
  }
  static int sfun2(int n)
    [[ pre: false ]] // { dg-warning "always .false." }
  {
    return n;
  }

  void fun(int n) const
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

struct CFoo {
  constexpr CFoo(int n) : memb{n} { }

  int fun(int n) const
    [[ pre: n != 0 ]]
    [[ pre: memb != 0 ]]
  {
    return n;
  }

  int memb{0};
};

int nested1(int n)
  [[ pre: n != 0 ]]
{
  return n;
}

int nested2(int n)
  [[ pre: nested1(n) != 1 ]]
{
  return n;
}

int nested3(int n)
  [[ pre: nested2(n) != 2 ]]
{
  return n;
}

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
  tfun2(2); // { dg-warning "postcondition ..t < 0.. is never satisfied here" }
  tfun2(2.5); // { dg-warning "postcondition ..t < .double.0.. is never satisfied here" }

  tfun_auto(1); // { dg-warning "precondition .false. is never satisfied here" }
  tfun_auto(1.5); // { dg-warning "precondition .false. is never satisfied here" }
  tfun_auto2(1); // { dg-warning "postcondition .false. is never satisfied here" }
  tfun_auto2(1.5); // { dg-warning "postcondition .false. is never satisfied here" }

  Foo f1{0};
  Foo f2{n};

  f1.fun(0);
  f1.fun(n);

  f2.fun(0);
  f2.fun(n);

  constexpr int m = 1;
  f1.fun(m);
  f2.fun(m);

  constexpr CFoo f3{m};
  int f30 = f3.fun(0);
  int f3n = f3.fun(n);
  int f3m = f3.fun(m);

  Foo::sfun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  Foo::sfun(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  Foo::sfun2(0); // { dg-warning "precondition .false. is never satisfied here" }
  Foo::sfun2(n); // { dg-warning "precondition .false. is never satisfied here" }

  TFoo<int> ft_int1{0};
  TFoo<int> ft_int2{n};

  ft_int1.fun(0);
  ft_int1.fun(n);

  // FIXME sfun call sees dependently typed contracts, should see n != 0
  TFoo<int>::sfun(0); // { dg-warning "precondition .false. is never satisfied here" }
  TFoo<int>::sfun(n); // { dg-warning "precondition .false. is never satisfied here" }
  TFoo<int>::sfun2(0); // { dg-warning "precondition .false. is never satisfied here" }
  TFoo<int>::sfun2(n); // { dg-warning "precondition .false. is never satisfied here" }

  return 0;
}

