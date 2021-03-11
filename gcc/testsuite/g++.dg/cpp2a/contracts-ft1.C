// { dg-do compile { target c++17_down } }

#ifdef __cpp_contracts
static_assert (false);
#endif

#ifdef __cpp_contracts_literal_semantics
static_assert (false);
#endif

#ifdef __cpp_contracts_roles
static_assert (false);
#endif

