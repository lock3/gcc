// { dg-module-do run }
// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=observe" }
module;
#include <cstdio>
#include <experimental/contracts>
export module bar;
// { dg-module-cmi bar }
import foo;

using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(bar_custom)]] bar_custom_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

int bar_fn(int n)
  [[ pre bar_custom: n > 0 ]]
{
  printf("%s(%d)\n", __FUNCTION__, n);
  return n;
}

int bar_fn2(int n)
  [[ pre foo_custom custom bar_custom: n > 0 ]]
{
  printf("%s(%d)\n", __FUNCTION__, n);
  return n;
}

int main(int, char**)
{
  foo_fn(-5);
  bar_fn(-5);
  bar_fn2(-5);
  return violation_count == 3 ? 0 : -1;
}

