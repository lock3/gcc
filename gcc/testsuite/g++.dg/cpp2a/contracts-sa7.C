// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts -Wconstant-postconditions" }

int fun(int n)
  [[ pre: n > 0 ]]
  [[ post r: r > 10 ]]
{
  return n;
}

constexpr int cfun(int n) {
  return fun(0); // { dg-error "call to non-.constexpr." }
  // { dg-warning "never satisfied here" "" { target *-*-* } .-1 }
}

static int gvar{-1};

int fun2(int n)
  [[ post r: r != 0 ]]
{
  [[ assert: false ]]; // { dg-warning "assertion is always .false." }
  if (gvar)
    goto l1; // goto makes fun invalid constexpr
l1:
  gvar = 15;
  return n;
}

constexpr int cfun2(int n) {
  return fun2(0); // { dg-error "call to non-.constexpr." }
}

