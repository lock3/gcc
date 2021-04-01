// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct
  [[contract_label(l1)]]
  [[contract_label(l2)]]
  label { };

int f1(int n)
  [[ pre l1 l1: false ]]; // { dg-error "twice" }

int f2(int n)
  [[ pre l2 l2: false ]]; // { dg-error "twice" }

int f3(int n)
  [[ pre l1 l2: false ]]; // { dg-error "twice" }

