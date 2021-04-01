// { dg-do compile }
// { dg-options "-std=c++20 -Kpre=ignore -Kassert=observe -Kpost=enforce" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(ignore_only)]] ignore_only_label
{
  static consteval bool allowable_semantic(semantic s) {
    return s == semantic::ignore;
  }
};

struct [[contract_label(bad_ignore_only)]] bad_ignore_only_label
{
  static consteval int allowable_semantic(semantic s) {
    return 5;
  }
};

struct [[contract_label(bad_ignore_only2)]] bad_ignore_only2_label
{
  static consteval bool allowable_semantic(semantic s) {
    return s == semantic::foo; // { dg-error "not a member" }
  }
};

int f1(int n)
  [[ pre ignore_only: false ]] // { dg-bogus "allow" }
  [[ post ignore_only: false ]] // { dg-error "allow" }
{
  [[ assert ignore_only: false ]]; // { dg-error "allow" }
  return n;
}

int f2(int n)
  [[ pre bad_ignore_only: false ]] // { dg-error "allow" }
  [[ post bad_ignore_only: false ]] // { dg-error "allow" }
{
  [[ assert bad_ignore_only: false ]]; // { dg-error "allow" }
  return n;
}

int f3(int n)
  [[ pre bad_ignore_only2: false ]] // { dg-error ".constexpr." }
  [[ post bad_ignore_only2: false ]] // { dg-error ".constexpr." }
{
  [[ assert bad_ignore_only2: false ]]; // { dg-error ".constexpr." }
  return n;
}

