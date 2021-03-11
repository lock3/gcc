// test that assumed constexpr contracts that reference undefined entities do
// not cause constexpr eval failure
// { dg-do run }
// { dg-options "-std=c++2a -fcontracts -Kassume=assume" }

constexpr int f(int t); // { dg-warning "used but never defined" }

constexpr int dummy()
{
  [[ assume: f(1) > 0 ]];
  return -1;
}

int main()
{
  constexpr int n = dummy();
  return 0;
}

