// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

struct FooChild0 : Foo
{
  void fn()
  {
    prot = 5; // { dg-bogus "protected" }
  }
};

struct FooChild1 : FooB
{
  void fn()
  {
    prot = 5; // { dg-error "protected" }
  }
};

