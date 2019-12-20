// { dg-additional-options -fmodules-ts }
export module foo;
// { dg-module-cmi foo }

export int foo() { return 0; }
export struct Foo;
export struct FooComplete {};

export template<typename T> int templateFoo(T t) { return 0; }
export template<> int templateFoo(double t) { return 0; }

export template<typename T> struct TemplateFoo;

export template<typename T> struct TemplateFooComplete { T t; };
export template<> struct TemplateFooComplete<double> { double t; double f; };


