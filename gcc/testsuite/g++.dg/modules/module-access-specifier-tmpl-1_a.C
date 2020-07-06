// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

export struct Foo
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

export template<typename R>
struct TemplateFoo
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

