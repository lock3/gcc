// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts -Wconstant-postconditions" }

constexpr int cfun(int n)
  [[ pre: n > 11 ]]
{
  return n;
}

constexpr int cfun2(int n)
{
  [[ assert: n > 12 ]];
  return n;
}

constexpr int cfun3(int n)
  [[ post r: r > 13 ]]
{
  return n;
}

constexpr int fun(int n)
  [[ pre: cfun(n) > 10 ]]
  [[ pre: n == 0 ]]
  [[ pre: n != 0 ]]
  [[ post r: r > 10 ]]
{
  return n;
}

constexpr int fun2(int n)
  [[ pre: cfun2(n) > 10 ]]
  [[ pre: n == 0 ]]
  [[ pre: n != 0 ]]
  [[ post r: r > 10 ]]
{
  return n;
}

constexpr int fun3(int n)
  [[ pre: cfun3(n) > 10 ]]
  [[ pre: n == 0 ]]
  [[ pre: n != 0 ]]
  [[ post r: r > 10 ]]
{
  return n;
}

int main(int, char **) {
  constexpr int m  = 5;
  fun(m); // { dg-warning "never satisfied here" }
  fun2(m); // { dg-warning "never satisfied here" }
  fun3(m); // { dg-warning "never satisfied here" }
}

