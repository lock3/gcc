// { dg-do compile }
// { dg-additional-options { -std=c++2a -Ksfoo=bar -Ksbar=baz -Kstest -Kfoo=5 -Kbar=3 -Ktest -Kb=yes -Knb=no -Ka1=Truthy -Ka2=Falsey -Ka3=FileNotFoundy } }
#include <experimental/meta>
#include <cstdio>

enum Boolean
{
  Truthy = 5,
  Falsey = 7,
  FileNotFoundy = 9,
  Invalid = 11,
};

int main(int, char**)
{
  printf("sfoo: %s\n", std::experimental::meta::getenv("sfoo"));
  printf("sbar: %s\n", std::experimental::meta::getenv("sbar"));
  printf("stest: %s\n", std::experimental::meta::getenv("stest"));
  printf("stest2: %s\n", std::experimental::meta::getenv("stest2"));

  // test int parsing
  printf("foo: %d\n", std::experimental::meta::getenv<int>("foo", -1));
  printf("bar: %d\n", std::experimental::meta::getenv<int>("bar", -1));
  printf("test: %d\n", std::experimental::meta::getenv<int>("test", -1));
  printf("test2: %d\n", std::experimental::meta::getenv<int>("test2", -1));

  // test bool parsing
  printf("test: %d\n", std::experimental::meta::getenv<bool>("test", false));
  printf("b: %d\n", std::experimental::meta::getenv<bool>("b", false));
  printf("test2: %d\n", std::experimental::meta::getenv<bool>("test2", false));
  printf("nb: %d\n", std::experimental::meta::getenv<bool>("nb", true));

  // test enum parsing
  printf("a1: %d\n", std::experimental::meta::getenv<Boolean>("a1", Invalid));
  printf("a2: %d\n", std::experimental::meta::getenv<Boolean>("a2", Invalid));
  printf("a3: %d\n", std::experimental::meta::getenv<Boolean>("a3", Invalid));
  printf("a4: %d\n", std::experimental::meta::getenv<Boolean>("a4", Invalid));
  return 0;
}

// { dg-output "sfoo: bar(\n|\r\n|\r)*" }
// { dg-output "sbar: baz(\n|\r\n|\r)*" }
// { dg-output "stest: (\n|\r\n|\r)*" }
// { dg-output "stest2: .null.(\n|\r\n|\r)*" }

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


