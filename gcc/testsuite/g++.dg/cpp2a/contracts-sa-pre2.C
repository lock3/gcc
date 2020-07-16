// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=pre" }

int fun()
  [[ pre: false ]]
{
  return 0;
}

int fun2(int n)
  [[ pre: n != 0 ]]
{
  return n;
}

int main(int, char**) {
  constexpr int n = 0;
  fun(); // { dg-warning "never satisfied here" }
  fun2(n); // { dg-warning "never satisfied here" }
}

