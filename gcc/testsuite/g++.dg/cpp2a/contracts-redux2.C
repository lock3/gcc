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

struct [[contract_label(bad_test)]] bad_test_label
{
  static consteval semantic adjust_semantic(attribute a, semantic s)
  {
    return semantic::foo; // { dg-error "not a member" }
  }
};

int fun(int n)
  [[ pre test : false ]] // { dg-error "private within this context" }
  [[ post [r] : false ]] // { dg-bogus "post" }
{
  return -n;
}

int fun2(int n)
  [[ pre bad_test : false ]] // { dg-error ".constexpr." }
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
  [[ pre : false ]]; // { dg-error "fewer contract labels" }

int m2()
  [[ pre : false ]];
int m2()
  [[ pre default : false ]]; // { dg-error "more contract labels" }

int m3()
  [[ pre valid : false ]];
int m3()
  [[ pre default : false ]]; // { dg-error "mismatch" }

int m4()
  [[ pre valid default : false ]];
int m4()
  [[ pre valid default : false ]]; // { dg-bogus "mismatch" }

int a1()
  [[ pre a + : false ]]; // { dg-error "contract-id" }

int a2()
  [[ pre a:: : false ]]; // { dg-error "contract-id" }

int a3()
  [[ pre a:: + : false ]]; // { dg-error "contract-id" }

