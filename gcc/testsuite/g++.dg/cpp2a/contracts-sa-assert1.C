// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstant-contracts -Wconstant-postconditions" }

constexpr int fun(int n)
  [[ pre: n != 0 ]]
  [[ post r: r != 0 ]]
{
  [[ assert: false ]]; // { dg-warning "assertion is always .false." }
  // { dg-message "use ...unreachable... to indicate unreachable code" "" { target *-*-* } .-1 }
  [[ assert: true ]]; // { dg-warning "assertion is always .true." }
  return n;
}

constexpr int fun2(int n)
  [[ pre: n != 0 ]]
  [[ post r: r != 0 ]]
{
  [[ assert: false ]]; // { dg-warning "assertion is always .false." }
  // { dg-message "use ...unreachable... to indicate unreachable code" "" { target *-*-* } .-1 }
  [[ assert: true ]]; // { dg-warning "assertion is always .true." }
  return n;
}

constexpr void fun3()
  [[ pre: true ]] // { dg-warning "precondition is always .true." }
  [[ post: true ]] // { dg-warning "postcondition is always .true." }
{
  constexpr int n = 0;
  [[ assert: n != 0 ]]; // { dg-warning "assertion is always .false." }
}

constexpr int fun4(int n)
  [[ post: true ]] // { dg-warning "postcondition is always .true." }
{
  [[ assert: n == 1 ]];
  return n;
}

int main(int, char**) {
  constexpr int n = 0;
  fun(0); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  fun2(n); // { dg-warning "precondition ..n != 0.. is never satisfied here" }
  fun3();
  fun4(n); // { dg-warning "assertion ..n == 1.. is never satisfied here" }
}

