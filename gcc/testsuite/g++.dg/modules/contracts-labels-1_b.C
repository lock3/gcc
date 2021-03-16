// { dg-module-do compile }
// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module bar; // { dg-warning "not writing module" }
import foo;

using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(custom)]] bar_custom_label // { dg-error "redeclaration" }
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

struct [[contract_label(foo_custom)]] bar_foo_custom_label // { dg-error "redeclaration" }
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

int main(int, char**)
{
  return 0;
}

