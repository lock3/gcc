// { dg-additional-options "-fmodules-ts" }
import foo;

struct Bar
{
  restrict:
    template<typename T> void tfun(T t) { return; }
    template<typename T> struct TSub {
      public:
        T x;
      restrict:
        T y;
    };
  public:
    template<typename T> TSub<T> getSub(T t) { return TSub<T>{}; }
};

template<typename R>
struct TemplateBar
{
  restrict:
    template<typename T> void tfun(T t) { return; }
    template<typename T> struct TSub {
      public:
        T x;
      restrict:
        T y;
    };
  public:
    template<typename T> TSub<T> getSub(T t) { return TSub<T>{}; }
};

void test()
{
  Foo foo;
  foo.tfun(5); // { dg-error "restricted to module" }
  foo.tfun(5.0); // { dg-error "restricted to module" }

  Foo::TSub<int> tsub_int; // { dg-error "restricted to module" }
  Foo::TSub<double> tsub_double; // { dg-error "restricted to module" }
  auto tsub_int2 = foo.getSub(5);
  tsub_int2.x = 6; // { dg-bogus "restricted to module" }
  tsub_int2.y = 7; // { dg-error "restricted to module" }
  auto tsub_double2 = foo.getSub(5.0);
  tsub_double2.x = 6.5; // { dg-bogus "restricted to module" }
  tsub_double2.y = 7.6; // { dg-error "restricted to module" }
}

void test2()
{
  TemplateFoo<int> tfoo_int;
  tfoo_int.tfun(5); // { dg-error "restricted to module" }
  tfoo_int.tfun(5.0); // { dg-error "restricted to module" }

  TemplateFoo<int>::TSub<int> tsub_int; // { dg-error "restricted to module" }
  TemplateFoo<int>::TSub<double> tsub_double; // { dg-error "restricted to module" }
  auto tsub_int2 = tfoo_int.getSub(5);
  tsub_int2.x = 6; // { dg-bogus "restricted to module" }
  tsub_int2.y = 7; // { dg-error "restricted to module" }
  auto tsub_double2 = tfoo_int.getSub(5.0);
  tsub_double2.x = 6.5; // { dg-bogus "restricted to module" }
  tsub_double2.y = 7.6; // { dg-error "restricted to module" }
}

void test3()
{
  Bar bar;
  bar.tfun(5); // { dg-bogus "restricted to module" }
  bar.tfun(5.0); // { dg-bogus "restricted to module" }

  Bar::TSub<int> tsub_int; // { dg-bogus "restricted to module" }
  Bar::TSub<double> tsub_double; // { dg-bogus "restricted to module" }
  auto tsub_int2 = bar.getSub(5);
  tsub_int2.x = 6; // { dg-bogus "restricted to module" }
  tsub_int2.y = 7; // { dg-bogus "restricted to module" }
  auto tsub_double2 = bar.getSub(5.0);
  tsub_double2.x = 6.5; // { dg-bogus "restricted to module" }
  tsub_double2.y = 7.6; // { dg-bogus "restricted to module" }
}

void test4()
{
  TemplateBar<int> tbar_int;
  tbar_int.tfun(5); // { dg-bogus "restricted to module" }
  tbar_int.tfun(5.0); // { dg-bogus "restricted to module" }

  TemplateBar<int>::TSub<int> tsub_int; // { dg-bogus "restricted to module" }
  TemplateBar<int>::TSub<double> tsub_double; // { dg-bogus "restricted to module" }
  auto tsub_int2 = tbar_int.getSub(5);
  tsub_int2.x = 6; // { dg-bogus "restricted to module" }
  tsub_int2.y = 7; // { dg-bogus "restricted to module" }
  auto tsub_double2 = tbar_int.getSub(5.0);
  tsub_double2.x = 6.5; // { dg-bogus "restricted to module" }
  tsub_double2.y = 7.6; // { dg-bogus "restricted to module" }
}

