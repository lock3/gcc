// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo { int x, y, z; };

export namespace foons {
  void fn() {}
  struct Foo2 { int x, y, z; };
  int z{5};
}

