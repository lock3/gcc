// ensure that failing pre/post can fail at constexpr time
// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -fcontract-continuation-mode=on -Wconstexpr-contract-checking=full" }

constexpr int ffun(int a)
  [[ pre: a > 0 ]]
  [[ post r: r > 10 ]]
{
  return a;
}

template<typename T>
constexpr int ftfun(T a)
  [[ pre: a > 1 ]]
  [[ post r: r > 11 ]]
{
  return a;
}

constexpr int explicitfn(int a)
  [[ pre ignore: a > 2 ]]
  [[ pre check_maybe_continue: a > 3 ]]
  [[ post ignore r: r > 12 ]]
  [[ post check_maybe_continue r: r > 13 ]]
{
  return a;
}

template<typename T>
constexpr int ftfun2(T a)
  [[ pre: a > 4 ]]
  [[ post r: r > 14 ]]
{
  return a;
}

constexpr int test1() {
  constexpr int a = ffun(-10); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 6 }
  constexpr int b = ftfun(-10); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 14 }
  constexpr int c = explicitfn(-10); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 22 }
  constexpr int d = ftfun2(-10.0); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 31 }
  return a + b + c + d;
}

constexpr int test2() {
  constexpr int e = ffun(5); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 7 }
  constexpr int f = ftfun(5); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 15 }
  constexpr int g = explicitfn(5); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 24 }
  constexpr int h = ftfun2(5.5); // { dg-warning "never satisfied" }
  // { dg-error "contract predicate" "" { target *-*-* } 32 }
  return e + f + g + h;
}

int main(int, char **) {
  constexpr int t1 = test1();
  constexpr int t2 = test2();
  return t2;
}

