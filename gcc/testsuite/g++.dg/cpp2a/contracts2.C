// check that a valid program using assertions compiles and runs
//   ensure an assume with a failing predicate doesn't prevent a successful run
//   (assume semantic contracts are never checked at runtime)
// { dg-do run }
// { dg-options "-std=c++2a -fcontracts -Kassume=assume" }
#include <experimental/contracts>

int main()
{
  int x = 1;
  [[assume: x < 0]];
  [[assert default: x > 0]];
  return 0;
}
