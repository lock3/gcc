// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export namespace foons
{
  struct Foo
  {
    int x;
    private:
      Foo() = default;
      Foo(const Foo&) = default;

      struct Inner { int y; };
      friend Foo make_foo();
  };
  struct Foo2 { };
  Foo make_foo()
  {
    return Foo{};
  }

  void fn() { }

  template<typename T>
  struct TFoo { T t; };
}

namespace ns1 {
  namespace ns2 {
    export struct NFoo { int x; };
    export struct NFoo2 { int x; int y; };
  }
}

export struct GlobalFoo { int x; };
export void globalfn() { }

