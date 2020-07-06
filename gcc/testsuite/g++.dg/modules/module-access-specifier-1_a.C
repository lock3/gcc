// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct FooStruct
{
  restrict:
    int quux;
    int tquux;
    int bar() { return 1; }
    struct X { int x; };
  public:
    X x;
    X f() { return X{}; }
};

export class FooClass
{
  restrict:
    int quux;
    int tquux;
    int bar() { return 1; }
    struct X { int x; };
};

export class SpecInline { restrict: int x;
  restrict: int y;
  restrict: int
    z;
};

export template<typename T> void q1() {
  T t; t.tquux = 5;
}
export template<typename T> void q2() {
  T t; t.tquux = 5;
}

template void q1<FooStruct>();

export template<typename T> struct TemplateFoo {
restrict:
  int tquux;
};

void foo_fn() {
  TemplateFoo<int> tfoo_int;
  tfoo_int.tquux = 5;
}

TemplateFoo<int> global_tfoo_int;
TemplateFoo<double> global_tfoo_double;

