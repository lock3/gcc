// { dg-do run }
// { dg-options "-std=c++20 -Kpre=observe" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

class [[contract_label(test)]] test_label;

class [[contract_label(test)]] test_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::ignore;
  }
};

class [[contract_label(test2a)]] test2_label;

class [[contract_label(test2b)]] test2_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::ignore;
  }
};

class [[contract_label(test3)]] test3_label { };

int fn1()
  [[ pre test3 : false ]]
{
  return -1;
}

int main(int, char**)
{
  fn1();
}

// { dg-output " 30 fn1 " }

