// check that a valid program using assertions compiles and runs
//   ensure an axiom with a failing predicate doesn't prevent a successful run
//   (axiom level contracts are never checked at runtime)
// { dg-do run }
// { dg-options "-std=c++2a" }
#include <experimental/contracts>

int main()
{
  int x = 1;
  [[assume: x < 0]];
  [[assert: x > 0]];
  [[assert audit: x < 0]];
  return 0;
}

