// { dg-do compile }
// { dg-additional-options { "-std=c++2a" } }
#include <cstddef>

consteval const char *foo(bool upper) { if (upper) return "yes"; return "no"; }

struct WeirdCString
{
  consteval operator const char *() { return "weird"; }
};

union UN
{
  int n;
  char b[sizeof(n)];
};

// ill formed if k cannot be converted to const char *
consteval void f1()
{
  __meta_maybe_getenv(true, bool, false); // { dg-error "cannot convert" }
  __meta_maybe_getenv(1, bool, false); // { dg-error "cannot convert" }
  __meta_maybe_getenv(5.7, bool, false); // { dg-error "cannot convert" }
  __meta_maybe_getenv(0, bool, false); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv((const char *)1, bool, false); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(foo(true), bool, false); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(foo(false), bool, false); // { dg-bogus "cannot convert" }
}

template<typename T>
consteval bool f1t(T t)
{
  bool val = __meta_maybe_getenv(t, bool, false);
  return false;
}

// ill formed if k is not a constant
void f2(const char *k)
{
  __meta_maybe_getenv(k, bool, false); // { dg-error "constant" }
  __meta_maybe_getenv(foo(true), bool, false); // { dg-bogus "constant" }
  __meta_maybe_getenv(foo(false), bool, false); // { dg-bogus "constant" }
}
consteval int f2_consteval(const char *k)
{
  __meta_maybe_getenv(k, bool, false); // { dg-bogus "constant" }
  __meta_maybe_getenv(foo(true), bool, false); // { dg-bogus "constant" }
  __meta_maybe_getenv(foo(false), bool, false); // { dg-bogus "constant" }
  return 1;
}

// ill formed if k is null
void f3()
{
  __meta_maybe_getenv(NULL, bool, false); // { dg-error "null" }
  __meta_maybe_getenv(nullptr, bool, false); // { dg-error "null" }
  __meta_maybe_getenv((const char*)0, bool, false); // { dg-error "null" }
  __meta_maybe_getenv(foo(true), bool, false); // { dg-bogus "null" }
  __meta_maybe_getenv(foo(false), bool, false); // { dg-bogus "null" }
}

enum Boool
{
  Truthy,
  Falsey,
  FILE_NOT_FOUNDY,
};

typedef void (*fnptr)();

// ill formed if T is not bool, a standard integer type, an enumeration type,
// or const char*
consteval void f4(const char *k)
{
  __meta_maybe_getenv(k, bool, false); // { dg-bogus "second operand" }
  __meta_maybe_getenv(k, int, 1); // { dg-bogus "second operand" }
  __meta_maybe_getenv(k, long, 2L); // { dg-bogus "second operand" }
  __meta_maybe_getenv(k, Boool, Falsey); // { dg-bogus "second operand" }
  __meta_maybe_getenv(k, const char *, k); // { dg-bogus "second operand" }

  __meta_maybe_getenv(k, WeirdCString, WeirdCString{}); // { dg-error "second operand" }
  __meta_maybe_getenv(k, UN, UN{}); // { dg-error "second operand" }
  __meta_maybe_getenv(k, fnptr, &f3); // { dg-error "second operand" }
}

// ill formed if e cannot be converted to T
consteval void f5(const char *k)
{
  __meta_maybe_getenv(k, bool, WeirdCString{}); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(k, int, WeirdCString{}); // { dg-error "cannot convert" }
  __meta_maybe_getenv(k, long, WeirdCString{}); // { dg-error "cannot convert" }
  __meta_maybe_getenv(k, Boool, WeirdCString{}); // { dg-error "cannot convert" }

  __meta_maybe_getenv(k, const char *, WeirdCString{}); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(k, const char *, "default"); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(k, const char *, NULL); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(k, const char *, k); // { dg-bogus "cannot convert" }
  __meta_maybe_getenv(k, bool, 10 > 5); // { dg-bogus "cannot convert" }
}

// ill formed if e is not a constant expression
void f6(int n)
{
  __meta_maybe_getenv("foo", bool, n > 10); // { dg-error "constant expression" }
  __meta_maybe_getenv("foo", int, n); // { dg-error "constant expression" }
  __meta_maybe_getenv("foo", long, (long)n); // { dg-error "constant expression" }
  __meta_maybe_getenv("foo", Boool, (Boool)n); // { dg-error "constant expression" }
}

int main(int, char** argv)
{
  f1t(5); // { dg-error "cannot convert" "" { target *-*-* } 33 }

  f2(argv[0]);

  constexpr auto z1 = f1t(nullptr); // { dg-error "null" "" { target *-*-* } 33 }
  constexpr auto z2 = f1t((const char *)0); // { dg-error "null" }
  constexpr auto z3 = f1t(foo(true)); // { dg-bogus "null" }
  constexpr auto z4 = f1t(foo(false)); // { dg-bogus "null" }
  constexpr auto z5 = f1t("foo"); // { dg-bogus "null" }
}

