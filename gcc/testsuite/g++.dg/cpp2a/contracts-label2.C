// { dg-do compile }
// { dg-options "-std=c++20" }

struct Foo
{
  struct [[contract_label(foo)]] foo_label // { dg-error "namespace scope" }
  {
  };
};

struct [[contract_label(bar)]] bar_label // { dg-error "stateless" }
{
  int x;
};

struct [[contract_label(l1)]] l1_label;

struct l1_label // { dg-error "stateless" }
{
  int x;
};

struct [[contract_label(l2)]] l2_label // { dg-bogus "stateless" }
{
  static int x;
  static int adjust_semantic(int a, int b)
  {
    return a * b;
  }
};

int fn1()
  [[ pre l2 : false ]]; // { dg-error "<experimental/contracts>" }

