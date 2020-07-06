// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

void fn() {
  Foo::Bar bar; // { dg-error "export protected" }
  auto b = Foo::get_bar(); // { dg-error "export protected" }
  b.x = 5; // { dg-error "export protected" }
}

