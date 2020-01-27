// { dg-additional-options "-fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

struct FooChild0 : Foo
{
  void fn()
  {
    prot = 5; // { dg-bogus "protected" }
  }
};

export protected Foo restrict prot;

struct FooChild1 : Foo
{
  void fn()
  {
    prot = 5; // { dg-error "protected" }
  }
};

