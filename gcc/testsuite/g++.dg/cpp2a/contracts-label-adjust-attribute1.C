// { dg-do run }
// { dg-options "-std=c++20 -Kpre=observe" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(ignore_assert)]] ignore_assert_label
{
  static consteval semantic adjust_semantic(attribute a, semantic s)
  {
    if (a == attribute::assertion)
      return semantic::ignore;
    return s;
  }
};

struct [[contract_label(make_assert)]] make_assert_label
{
  static consteval attribute adjust_attribute(attribute a)
  {
    return attribute::assertion;
  }
};


int f1(int n)
  [[ pre ignore_assert : false ]]
  [[ pre make_assert : false ]]
  [[ pre make_assert ignore_assert : false ]]
{
  return -n;
}

int main(int, char**)
{
  f1(-1);
  return 0;
}

// { dg-output "default std::handle_contract_violation.*.C 27 f1 false.*" }
// { dg-output "default std::handle_contract_violation.*.C 28 f1 false.*" }

