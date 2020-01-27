// { dg-additional-options "-fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

export protected Foo permit x, y;

export void fn1()
{
  Foo foo;
  foo.x = 5;
  foo.y = 5;
  foo.quux = 5; // { dg-error "export protected" }
}

export protected Foo2 restrict x, y;

export void fn2()
{
  Foo2 foo;
  foo.x = 5; // { dg-error "export protected" }
  foo.y = 5; // { dg-error "export protected" }
  foo.quux = 5;
}

export protected ::Foo3 permit e1, u2;

export void fn3()
{
  Foo3 foo;
  foo.u2 = 3.5 + (int)foo.e1;
  int z = (int)foo.e2; // { dg-error "inaccessible" }
  Foo3 foo2;
  foo2.u1 = 5; // { dg-error "inaccessible" }
}

export protected Foo4 restrict ~Foo4;
export void fn4()
{
  Foo4 foo; // { dg-error "export protected" }
}
export protected Foo4 restrict Foo4;
export void fn4_2()
{
  Foo4 foo; // { dg-error "export protected" }
}

export protected Foo5 restrict Foo5;
export void fn5()
{
  Foo5 foo; // { dg-error "export protected" }
}

export protected Foo6 permit x;
export void fn6()
{
  Foo6 foo; // { dg-bogus "export protected" }
}

export protected Foo7 permit x;
export void fn7()
{
  Foo7 foo; // { dg-error "export protected" }
}

export protected Foo8 restrict prot;
struct Foo8Child : Foo8
{
  void fn()
  {
    prot = 5; // { dg-error "protected" }
  }
};

export protected FooOp restrict operator +=, operator int;
export void fn_op()
{
  FooOp a, b;
  a += b; // { dg-error "export protected" }
  int z = (int)a; // { dg-error "export protected" }
  double zd = (double)a; // { dg-bogus "export protected" }
  bool zb = (bool)a; // { dg-error "export protected" }
}

