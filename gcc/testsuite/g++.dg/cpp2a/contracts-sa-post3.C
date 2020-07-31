// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts -Wconstexpr-contract-checking=trivial,prepost" }

int fun()
  [[ post: false ]] // { dg-warning "always .false." }
{
  return 0;
}

int fun2(int n)
  [[ post: n != 0 ]]
{
  return n;
}

int fun3()
  [[ post: false ]] // { dg-warning "always .false." }
{
  return; // { dg-error "no value" }
}

int fun4(int n)
  [[ post r: r != 0 ]]
{
  return n;
}

int fun5(int n)
  [[ post: false ]] // { dg-warning "always .false." }
{
  [[ assert: n != 0 ]];
  return n;
}

int main(int, char**) {
  constexpr int n = 0;
  constexpr int m = 1;
  fun(); // { dg-warning "never satisfied here" }
  fun2(n); // { dg-warning "never satisfied here" }
  fun4(n); // { dg-warning "never satisfied here" }
  fun4(m); // { dg-bogus "never satisfied here" }
  fun5(n); // { dg-warning "contract predicate ..n != 0.. is .false." "" { target *-*-* } 31 }
  fun5(m); // { dg-warning "never satisfied here" }
  // { dg-bogus "contract predicate ..n != 0.. is .false." "" { target *-*-* } 31 }
}

