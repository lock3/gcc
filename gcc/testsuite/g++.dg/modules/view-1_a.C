// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo
{
  int quux;
  int tquux;
  int bar() { return 1; }
  struct X { int x; };

  protected:
    int prot_x;
  private:
    int priv_x;
  restrict:
    int mod_x;
};

export struct FooAccess
{
  protected:
    int prot_x;
  private:
    int priv_x;
  restrict:
    int mod_x;
};

export struct Foo1 { int x, y, z; };
export struct Foo2 { int x, y, z; };
export struct Foo3 { int x, y, z; };
export struct Foo4 { int x, y, z; };

export struct FooBase { int x, y, z; int foo() { return 0; } };
export struct FooChild : public FooBase { int cx, cy, cz; };
export struct FooChild2 : public FooBase { int x, y; int foo() { return 0; } };
export struct FooChild3 : public FooBase { using FooBase::foo; };

export struct FooBase2 { struct Inner { int x; }; };
export struct FooChild4 : public FooBase2 { };
//export struct FooChild5 : public FooBase2 { using FooBase2::Inner; };
// FIXME ^ general modules issue
export struct FooChild6 : FooBase2 { struct Inner { int y; }; };

export struct FooIncomplete;

export struct Foo5 { int x; operator bool() { return true; } };
export struct Foo6
{
  int x;
  template<typename T>
  operator T() { return T{}; }
};

