// ensure the feature test macros are defined pre c++20 while we still support
// -fcontracts independent of std version
// Note that to actually use _any_ label whatsover c++20 consteval support is
// required.
// { dg-do compile { target c++11 } }
// { dg-additional-options "-fcontracts" }

static_assert (__cpp_contracts >= 201906, "__cpp_contracts");
static_assert (__cpp_contracts_literal_semantics >= 201906, "__cpp_contracts_literal_semantics");
static_assert (__cpp_contracts_roles >= 201906, "__cpp_contracts_roles");

int main()
{
  int x;

  [[assert: x >= 0]];

  [[assert: x > 0 ? true : false]];
  [[assert: x < 0 ? true : false]];

  return 0;
}

