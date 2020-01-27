// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo1
{
  int x;
};

export struct Foo2
{
  int x;
  Foo2() = default;
};

export struct Foo3
{
  int x;
  Foo3(Foo3 &) = default;
};

export struct Foo4
{
  int x;
  Foo4 &operator=(Foo4 &) = default;
};

export struct Foo5
{
  int x;
  Foo5() { }
  ~Foo5() = default;
};

