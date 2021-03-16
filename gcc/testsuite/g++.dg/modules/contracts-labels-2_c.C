// { dg-module-do compile }
// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module baz; // { dg-warning "not writing module" }
import foo;
import bar;
// { dg-error "conflicting declaration of contract label .foo_custom." "" { target *-*-* } 8 }
// { dg-error "conflicting declaration of contract label .custom." "" { target *-*-* } 8 }

int main(int, char**)
{
  return 0;
}

