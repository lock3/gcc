// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

export void fn1()
{
  Foo foo;
  foo.x = 5;
  foo.y = 5;
  foo.quux = 5; // { dg-error "export protected" }
}

export void fn2()
{
  Foo2 foo;
  foo.x = 5; // { dg-error "export protected" }
  foo.y = 5; // { dg-error "export protected" }
  foo.quux = 5;
}

export void fn3()
{
  Foo3 foo;
  foo.u2 = 3.5 + (int)foo.e1;
  int z = (int)foo.e2; // { dg-error "export protected" }
  Foo3 foo2;
  foo2.u1 = 5; // { dg-error "export protected" }
}

export void fn4()
{
  Foo4 foo; // { dg-error "export protected" }
}

export void fn4_2()
{
  Foo4B foo; // { dg-error "export protected" }
}

export void fn5()
{
  Foo5 foo; // { dg-error "export protected" }
}

export void fn6()
{
  Foo6 foo; // { dg-bogus "export protected" }
}

export void fn7()
{
  Foo7 foo; // { dg-error "export protected" }
}

struct Foo8Child : Foo8
{
  void fn()
  {
    prot = 5; // { dg-error "protected" }
  }
};

export void fn_op()
{
  FooOp a, b;
  a += b; // { dg-error "export protected" }
  int z = (int)a; // { dg-error "export protected" }
  double zd = (double)a; // { dg-bogus "export protected" }
  bool zb = (bool)a; // { dg-error "export protected" }
}

