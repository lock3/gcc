// test that assumed constexpr contracts that reference defined entities cause
// constexpr eval failure when the predicate is constexpr false
// test that assumed constexpr contracts that reference undefined entities in
// an unevaluated context cause constexpr eval failure
// { dg-do compile }
// { dg-options "-std=c++2a -Kassume=assume" }

constexpr int f(int t)
{
  return -1;
}

constexpr int dummy()
{
  [[ assume: f(1) > 0 ]];
  return -1;
}

constexpr int undef(int t);

constexpr int dummy2()
{
  [[ assume: sizeof(decltype(f(1))) < 0 ]];
  return -1;
}

int main()
{
  constexpr int n = dummy(); // { dg-message "in .constexpr. expansion" }
  // { dg-error "contract predicate" "" { target *-*-* } 15 }
  constexpr int m = dummy2(); // { dg-message "in .constexpr. expansion" }
  // { dg-error "contract predicate" "" { target *-*-* } 23 }
  return 0;
}

