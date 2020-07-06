// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

void fn()
{
  Foo1 foo1a;
  Foo1 foo1b{foo1a};
  foo1a = foo1b;

  Foo2 foo2; // { dg-error "export protected" }

  Foo3 foo3a; // { dg-error "no matching" }
  Foo3 foo3b{foo3a}; // { dg-error "export protected" }

  Foo4 foo4a, foo4b;
  foo4b = foo4a; // { dg-error "export protected" }

  Foo5 foo5; // { dg-error "Foo5@foo::.Foo5.*export protected" }
}

