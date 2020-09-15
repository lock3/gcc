// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts" }

int fun()
  [[ post: false ]] // { dg-warning "postcondition is always" }
{
  return 0;
}

int fun2(int n)
  [[ post: n != 0 ]]
{
  return n;
}

int fun3()
  [[ post: false ]] // { dg-warning "postcondition is always" }
{
  return; // { dg-error "no value" }
}

int fun4(int n)
  [[ post r: r != 0 ]]
{
  return n;
}

int fun5(int n)
  [[ post: false ]] // { dg-warning "postcondition is always" }
{
  [[ assert: false ]]; // { dg-warning "assertion is always" }
  return 0;
}

int main(int, char**) {
  constexpr int n = 0;
  fun(); // { dg-bogus "never satisfied here" }
  fun2(n); // { dg-bogus "never satisfied here" }
  fun4(n); // { dg-bogus "never satisfied here" }
  fun5(n); // { dg-bogus "never satisfied here" }
}

