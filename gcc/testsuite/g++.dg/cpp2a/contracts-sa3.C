// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-postconditions" }

constexpr int fun(int n)
  [[ post r: r != 0 ]]
{
  return n;
}

constexpr int fun_arg(int n)
  [[ post r: n != 0 ]]
{
  return n;
}

constexpr int fun_pre_arg(int n)
  [[ post r: n != 0 ]]
{
  return n;
}

int main(int, char**)
{
  constexpr int n = 0;
  fun(0); // { dg-warning "never satisfied here" }
  fun(n); // { dg-warning "never satisfied here" }

  fun_pre_arg(n); // { dg-warning "never satisfied here" }

  fun_arg(0); // { dg-warning "never satisfied here" }
  fun_arg(n); // { dg-warning "never satisfied here" }
}

