// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export template<typename T>
struct TFoo {
restrict:
  int quux;
};

export struct Foo { };

export struct FooBar {};

export template struct TFoo<FooBar>;

