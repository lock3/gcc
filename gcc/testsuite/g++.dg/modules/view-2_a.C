// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo
{
  int quux, x, y;
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

export struct Foo2
{
  int quux, x, y;
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

export struct Foo3
{
  enum {e1, e2, e3};
  union {
    int u1;
    double u2;
    char u3;
  };
};

export struct Foo4
{
  Foo4() { }
  Foo4(int x) { }
  ~Foo4() { }
};

export struct Foo4B
{
  Foo4B() { }
  Foo4B(int x) { }
  ~Foo4B() { }
};

export struct Foo5
{
  int x;
  Foo5() = default;
};

export struct Foo6 { int x; };
export struct Foo7 { int x; Foo7() = default; };

export struct Foo8 {
protected:
  int prot;
};

export struct FooOp
{
  FooOp &operator+=(FooOp &) { return *this; }
  operator int() { return 3; }
  operator double() { return 5.7; }
  template<typename TZ>
  operator TZ() { return TZ{}; }
};

