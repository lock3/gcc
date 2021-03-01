// { dg-do run }
// { dg-additional-options "-std=c++2a -Kfoo=bar -Kbar=baz -Ktest" }
#include <cstdio>

consteval const char *meta_getenv(const char *key)
{
  return __meta_getenv(key);
}

int main(int, char**)
{
  // FIXME why can't we pass a const char* directly to __meta_getenv?
  //constexpr const char* foo = __meta_getenv("foo");
  //static constinit const char* foo = __meta_getenv("foo");
  //printf("foo: %s", __meta_getenv("foo"));

  printf("foo: %s\n", meta_getenv("foo"));
  printf("bar: %s\n", meta_getenv("bar"));
  printf("test: %s\n", meta_getenv("test"));
  printf("test2: %s\n", meta_getenv("test2"));
  return 0;
}

// { dg-output "foo: bar(\n|\r\n|\r)*" }
// { dg-output "bar: baz(\n|\r\n|\r)*" }
// { dg-output "test: (\n|\r\n|\r)*" }
// { dg-output "test2: .null.(\n|\r\n|\r)*" }

