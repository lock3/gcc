// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(valid)]] valid_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

class [[contract_label(test)]] test_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::ignore;
  }
};

int fun(int n)
  [[ pre test : false ]] // { dg-error "private within this context" }
  [[ post [r] : false ]] // { dg-bogus "post" }
{
  return -n;
}

int main(int, char**)
{
  [[ assert [x] : false ]]; // { dg-error "post" }
  [[ assert invalid : false ]]; // { dg-error "was not declared" }
}

int fun2()
  [[ pre: false ]]
{
  return 0;
}

int m1()
  [[ pre valid : false ]];
int m1()
  [[ pre : false ]]; // { dg-error "mismatch" }

int m2()
  [[ pre : false ]];
int m2()
  [[ pre default : false ]]; // { dg-error "mismatch" }

int a1()
  [[ pre a + : false ]]; // { dg-error "contract-id" }

int a2()
  [[ pre a:: : false ]]; // { dg-error "contract-id" }

int a3()
  [[ pre a:: + : false ]]; // { dg-error "contract-id" }

