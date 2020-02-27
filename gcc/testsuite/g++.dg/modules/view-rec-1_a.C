// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo {
  struct Bar { int x; Bar() { } };
  static Bar get_bar() { return Bar{}; }
};

