// { dg-additional-options "-fmodules-ts" }
export module bar;
import foo;

struct Bar {
};

template<> struct TFoo<Bar> {
restrict:
  int quux;
};

struct BarFoo {};
struct BarFoo2 {};
template struct TFoo<BarFoo2>;

int main(int, char**) {
  TFoo<Bar> tfoo_bar;
  tfoo_bar.quux = 5; // { dg-bogus "restricted to module" }

  TFoo<Foo> tfoo_foo;
  tfoo_foo.quux = 5; // { dg-error "restricted to module" }

  TFoo<BarFoo> tfoo_barfoo;
  tfoo_barfoo.quux = 5; // { dg-error "restricted to module" }

  TFoo<FooBar> tfoo_foobar;
  tfoo_foobar.quux = 5; // { dg-error "restricted to module" }

  TFoo<BarFoo2> tfoo_barfoo2;
  tfoo_barfoo2.quux = 5; // { dg-error "restricted to module" }

  return 0;
}

// { dg-excess-errors "not writing module" }

