// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(make_assert)]] make_assert_label
{
  static consteval attribute adjust_attribute(attribute a)
  {
    return attribute::assertion;
  }
};

struct [[contract_label(make_pre)]] make_pre_label
{
  static consteval attribute adjust_attribute(attribute a)
  {
    return attribute::precondition;
  }
};

struct [[contract_label(bad_test)]] bad_test_label
{
  static consteval attribute adjust_attribute(attribute a)
  {
    return attribute::foo; // { dg-error "not a member" }
  }
};


int f1(int n)
  [[ pre make_assert : false ]] // { dg-bogus "single adjust_attribute" }
  [[ pre make_pre : false ]] // { dg-bogus "single adjust_attribute" }
  [[ pre make_assert make_pre : false ]] // { dg-error "single adjust_attribute" }
  [[ pre bad_test : false ]] // { dg-error ".constexpr." }
{
  return -n;
}

