// { dg-additional-options "-fmodules-ts" }
export module quux; // { dg-warning "not writing module" }
import bar;
import baz;

void fn()
{
  Foo foo;
  foo.x = 5; // { dg-bogus "export protected" }
  foo.y = 5; // { dg-bogus "export protected" }
  foo.z = 5; // { dg-error "export protected" }

  foons::fn(); // { dg-bogus "export protected" }
  foons::Foo2 f2{}; // { dg-bogus "export protected" }
  f2.x = 5; // { dg-error "export protected" }
  f2.y = 5; // { dg-bogus "export protected" }
  foons::z = 10; // { dg-error "export protected" }
}

