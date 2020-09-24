// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contract-checks" }

int fun(int n)
  [[ pre: n > 0 ]]
{
  return 0;
}

constexpr int cfun(int n)
  [[ post r: r > 0 ]]
{
  return -n;
}

int trivial(int n)
  [[ pre: true ]] // { dg-warning "always .true." }
{
}

int main(int, char**)
{
  constexpr int n = -5;
  fun(n); // { dg-warning "never satisfied here" }
  cfun(-n); // { dg-warning "never satisfied here" }
}
