// generic assert contract parsing checks
//   check omitted, 'default', 'audit', and 'symbolic' contracts parse
//   check that all concrete semantics parse
//   ensure that an invalid contract label 'invalid' errors
//   ensure that a predicate referencing an undefined variable errors
//   ensure that a missing colon after contract labels errors
//   ensure that an invalid contract label 'invalid' errors
//   ensure that a missing colon after contract label errors
// { dg-do compile }
// { dg-options "-std=c++2a -fcontracts" }
#include <experimental/contracts>

static_assert (__cpp_contracts >= 201906);
static_assert (__cpp_contracts_literal_semantics >= 201906);
static_assert (__cpp_contracts_roles >= 201906);

using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(role_default)]] role_default_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};

#include "contracts-literal-semantic.h"

int main()
{
  int x;

  [[assert: x >= 0]];
  [[assert default: x < 0]];
  [[assert audit: x == 0]];
  [[assert symbolic: x == 1]];

  [[assert: x > 0 ? true : false]];
  [[assert: x < 0 ? true : false]];

  [[assert: x = 0]]; // { dg-error "expected .]. before .=. token" }

  [[assert ignore: x >= 0]];
  [[assert assume: x >= 0]];
  [[assert check_never_continue: x >= 0]];
  [[assert check_maybe_continue: x >= 0]];

  [[assert role_default: x >= 0]];
  [[assert default role_default: x < 0]];
  [[assert audit role_default: x == 0]];
  [[assert symbolic role_default: x == 1]];

  [[assert check_always_continue: x >= 0]]; // { dg-error "not declared" }
  [[assert invalid: x == 0]]; // { dg-error "not declared" }
  [[assert: y == 0]]; // { dg-error ".y. was not declared in this scope" }
  [[assert default x == 0]]; // { dg-error "expected contract-id" }
  [[assert role_default x >= 0]]; // { dg-error "expected contract-id" }

  [[assert role_invalid: x >= 0]]; // { dg-error "not declared" }
  return 0;
}
