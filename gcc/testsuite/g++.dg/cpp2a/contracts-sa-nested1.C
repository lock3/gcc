// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=full" }

int nested1(int n)
  [[ pre: n != 0 ]]
{
  return n;
}

int nested2(int m)
  [[ pre: nested1(m) != 1 ]]
{
  return m;
}

int nested3(int o)
  [[ pre: nested2(o) != 2 ]]
{
  return o;
}

int main(int, char**) {
  {
    constexpr int a = 0;
    nested1(a); // { dg-warning "is never satisfied here" }
    nested2(a); // { dg-warning "is never satisfied here" }
    nested3(a); // { dg-warning "is never satisfied here" }
  }
  {
    constexpr int a = 1;
    nested1(a);
    nested2(a); // { dg-warning "is never satisfied here" }
    nested3(a); // { dg-warning "is never satisfied here" }
  }
  {
    constexpr int a = 2;
    nested1(a);
    nested2(a);
    nested3(a); // { dg-warning "is never satisfied here" }
  }
  {
    constexpr int a = 3;
    nested1(a);
    nested2(a);
    nested3(a);
  }
}

