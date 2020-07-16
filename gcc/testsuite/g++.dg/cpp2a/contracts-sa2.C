// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=trivial" }

int fun(int n)
  [[ pre: n != 0 ]]
{
  [[ assert: false ]]; // { dg-warning "assertion is always .false." }
  return n;
}

int fun2(int n)
  [[ pre: false ]] // { dg-warning "always .false." }
{
  return n;
}

int main(int, char**)
{
  fun(0);
  fun2(0); // { dg-warning "never satisfied here" }
}

