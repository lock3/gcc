// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo {
protected:
  int prot;
};

