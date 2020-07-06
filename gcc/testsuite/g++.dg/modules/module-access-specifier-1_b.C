// { dg-additional-options "-fmodules-ts" }
export module bar;
import foo;

struct Bar {
restrict:
  int quux;
  int tquux;
};
struct EmptyBar { };

template<> struct TemplateFoo<Bar> {
restrict:
  int tquux;
  int mquux;
public:
  int pub;
};

template<> struct TemplateFoo<EmptyBar> {
public:
  int tquux;
  int pub;
};

void test()
{
  FooStruct fs;
  fs.quux = 1; // { dg-error "restricted to module" }
  fs.bar(); // { dg-error "restricted to module" }
  FooStruct::X x1; // { dg-error "restricted to module" }

  fs.x.x = 5;
  fs.x = { 6 };
  auto fsx = fs.f();
  fsx.x = 5;

  FooClass fc;
  fc.quux = 1; // { dg-error "restricted to module" }
  fc.bar(); // { dg-error "restricted to module" }
  FooClass::X x2; // { dg-error "restricted to module" }

  SpecInline si;
  si.x = 1; // { dg-error "restricted to module" }
  si.y = 1; // { dg-error "restricted to module" }
  si.z = 1; // { dg-error "restricted to module" }

  TemplateFoo<int> tfoo_int;
  tfoo_int.tquux = 5; // { dg-error "restricted to module" }
  TemplateFoo<double> tfoo_double;
  tfoo_double.tquux = 5; // { dg-error "restricted to module" }

  TemplateFoo<Bar> tfoo_bar;
  // should be fine, spec belongs to bar
  tfoo_bar.tquux = 5; // { dg-bogus "restricted to module" }
  tfoo_bar.mquux = 5;
  tfoo_bar.pub = 5;

  TemplateFoo<EmptyBar> tfoo_emptybar;
  tfoo_emptybar.tquux = 5; // should be fine, spec belongs to bar
  tfoo_emptybar.pub = 5;

  q1<FooStruct>(); // fine, instantiated in foo
  q1<Bar>(); // fine, instantiated in bar

  q2<FooClass>(); // error, bar trying to use FooClass::tquux@foo
  // { dg-excess-errors "FooClass@foo::tquux. is restricted to module" }
}

