// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=trivial" }

int fun(int n)
  [[ pre: n != 0 ]]
  [[ pre: false ]] // FIXME should pre that don't use args fall under trivial?
{
  [[ assert: false ]]; // { dg-warning "assertion is always .false." }
  return n;
}

int main(int, char**)
{
  fun(0);
}

