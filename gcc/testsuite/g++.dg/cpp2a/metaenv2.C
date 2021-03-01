// { dg-do run }
// { dg-additional-options "-std=c++2a -Kfoo=5 -Kbar=3 -Ktest -Kb=yes -Knb=no -Ka1=Truthy -Ka2=Falsey -Ka3=FileNotFoundy" }
#include <cstdio>

consteval int meta_maybe_getenv_int(const char *key, int defl)
{
  return __meta_maybe_getenv(key, int, defl);
}

template<typename T>
consteval T meta_maybe_getenv(const char *key, T defl)
{
  return __meta_maybe_getenv(key, T, defl);
}

enum Boolean
{
  Truthy = 5,
  Falsey = 7,
  FileNotFoundy = 9,
  Invalid = 11,
};

int main(int, char**)
{
  // test int parsing
  printf("foo: %d\n", meta_maybe_getenv_int("foo", -1));
  printf("bar: %d\n", meta_maybe_getenv_int("bar", -1));

  printf("foo: %d\n", meta_maybe_getenv<int>("foo", -1));
  printf("bar: %d\n", meta_maybe_getenv<int>("bar", -1));
  printf("test: %d\n", meta_maybe_getenv<int>("test", -1));
  printf("test2: %d\n", meta_maybe_getenv<int>("test2", -1));

  // test bool parsing
  printf("test: %d\n", meta_maybe_getenv<bool>("test", false));
  printf("b: %d\n", meta_maybe_getenv<bool>("b", false));
  printf("test2: %d\n", meta_maybe_getenv<bool>("test2", false));
  printf("nb: %d\n", meta_maybe_getenv<bool>("nb", true));

  // test enum parsing
  printf("a1: %d\n", meta_maybe_getenv<Boolean>("a1", Invalid));
  printf("a2: %d\n", meta_maybe_getenv<Boolean>("a2", Invalid));
  printf("a3: %d\n", meta_maybe_getenv<Boolean>("a3", Invalid));
  printf("a4: %d\n", meta_maybe_getenv<Boolean>("a4", Invalid));
  return 0;
}

// { dg-output "foo: 5(\n|\r\n|\r)*" }
// { dg-output "bar: 3(\n|\r\n|\r)*" }

// { dg-output "foo: 5(\n|\r\n|\r)*" }
// { dg-output "bar: 3(\n|\r\n|\r)*" }
// { dg-output "test: 0(\n|\r\n|\r)*" }
// { dg-output "test2: -1(\n|\r\n|\r)*" }

// { dg-output "test: 1(\n|\r\n|\r)*" }
// { dg-output "b: 1(\n|\r\n|\r)*" }
// { dg-output "test2: 0(\n|\r\n|\r)*" }
// { dg-output "nb: 0(\n|\r\n|\r)*" }

// { dg-output "a1: 5(\n|\r\n|\r)*" }
// { dg-output "a2: 7(\n|\r\n|\r)*" }
// { dg-output "a3: 9(\n|\r\n|\r)*" }
// { dg-output "a4: 11(\n|\r\n|\r)*" }

