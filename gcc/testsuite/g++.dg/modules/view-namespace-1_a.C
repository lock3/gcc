// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export namespace foons
{
  struct Foo
  {
    int x;
    private:
      struct Inner { int y; };
  };
  struct Foo2 { };
}

