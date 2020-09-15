// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts" }

int fun()
  [[ pre: false ]] // { dg-warning "precondition is always" }
{
  return 0;
}

int fun2(int n)
  [[ pre: n != 0 ]]
{
  return n;
}

int fun3() // never called
  [[ pre: false ]] // { dg-warning "precondition is always" }
{
  return 0;
}

int main(int, char**) {
  constexpr int n = 0;
  fun();
  fun2(n);
}

