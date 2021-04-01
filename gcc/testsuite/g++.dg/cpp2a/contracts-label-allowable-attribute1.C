// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(pre_only)]] pre_only_label
{
  static consteval bool allowable_attribute(attribute a) {
    return a == attribute::precondition;
  }
};

struct [[contract_label(bad_pre_only)]] bad_pre_only_label
{
  static consteval int allowable_attribute(attribute a) {
    return 5;
  }
};

struct [[contract_label(bad_pre_only2)]] bad_pre_only2_label
{
  static consteval bool allowable_attribute(attribute a) {
    return a == attribute::foo; // { dg-error "not a member" }
  }
};

int f1(int n)
  [[ pre pre_only: false ]] // { dg-bogus "allowed" }
  [[ post pre_only: false ]]; // { dg-error "allowed" }

int f2(int n)
  [[ pre bad_pre_only: false ]] // { dg-error "allowed" }
  [[ post bad_pre_only: false ]]; // { dg-error "allowed" }

int f3(int n)
  [[ pre bad_pre_only2: false ]] // { dg-error ".constexpr." }
  [[ post bad_pre_only2: false ]]; // { dg-error ".constexpr." }

