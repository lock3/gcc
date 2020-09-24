// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-postconditions" }

static constexpr int gvar{-1};

constexpr int fun(int n)
  [[ post r: r != 0 ]]
{
  [[ assert: false ]]; // { dg-bogus "assertion is always .false." }
  if (gvar)
    goto l1; // { dg-error ".goto. in .constexpr. function" }
  // { dg-error ".goto. is not a constant expression" "" { target *-*-* } .-1 }
  // ^ FIXME, the second error can probably be suppressed
l1:
  return n;
}

int main(int, char**)
{
  int n = fun(0);
}

