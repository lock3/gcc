// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-postconditions" }

static int gvar{-1};

constexpr int fun(int n)
  [[ post r: r != 0 ]]
{
  [[ assert: false ]]; // { dg-bogus "assertion is always .false." }
  if (gvar)
    goto l1; // { dg-error ".goto. in .constexpr. function" }
l1:
  gvar = 15;
  return n;
}

int main(int, char**)
{
  int n = fun(0);
}

