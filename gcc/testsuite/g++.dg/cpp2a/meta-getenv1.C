// { dg-do compile }
// { dg-additional-options { "-std=c++2a" } }
#include <cstddef>

consteval const char *foo(bool upper) { if (upper) return "FOO"; return "foo"; }

struct WeirdCString
{
  consteval operator const char *() { return "weird"; }
};

// ill formed if k cannot be converted to const char *
consteval void f1()
{
  __meta_getenv(true); // { dg-error "cannot convert" }
  __meta_getenv(1); // { dg-error "cannot convert" }
  __meta_getenv(5.7); // { dg-error "cannot convert" }
  __meta_getenv(0); // { dg-bogus "cannot convert" }
  __meta_getenv((const char *)1); // { dg-bogus "cannot convert" }
  __meta_getenv(foo(true)); // { dg-bogus "cannot convert" }
  __meta_getenv(foo(false)); // { dg-bogus "cannot convert" }
  __meta_getenv((const char *)WeirdCString{}); // { dg-bogus "cannot convert" }
  __meta_getenv(WeirdCString{}); // { dg-bogus "cannot convert" }
}

template<typename T>
consteval const char *f1t(T t)
{
  const char *val = __meta_getenv(t);
  return "";
}

// ill formed if k is not a constant
void f2(const char *k)
{
  __meta_getenv(k); // { dg-error "constant" }
  __meta_getenv(foo(true)); // { dg-bogus "constant" }
  __meta_getenv(foo(false)); // { dg-bogus "constant" }
}
consteval int f2_consteval(const char *k)
{
  __meta_getenv(k); // { dg-bogus "constant" }
  __meta_getenv(foo(true)); // { dg-bogus "constant" }
  __meta_getenv(foo(false)); // { dg-bogus "constant" }
  return 1;
}

// ill formed if k is null
void f3()
{
  __meta_getenv(NULL); // { dg-error "null" }
  __meta_getenv(nullptr); // { dg-error "null" }
  __meta_getenv((const char*)0); // { dg-error "null" }
  __meta_getenv(foo(true)); // { dg-bogus "null" }
  __meta_getenv(foo(false)); // { dg-bogus "null" }
}

int main(int, char** argv)
{
  f1t(5); // { dg-error "cannot convert" "" { target *-*-* } 29 }

  f2(argv[0]);

  constexpr auto z1 = f1t(nullptr); // { dg-error "null" "" { target *-*-* } 29 }
  constexpr auto z2 = f1t((const char *)0); // { dg-error "null" }
  constexpr auto z3 = f1t(foo(true)); // { dg-bogus "null" }
  constexpr auto z4 = f1t(foo(false)); // { dg-bogus "null" }
  constexpr auto z5 = f1t("foo"); // { dg-bogus "null" }
}

