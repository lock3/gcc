// { dg-do compile }
// { dg-options "-std=c++20" }
#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct dummy_label
{
  static consteval semantic adjust_semantic(attribute, semantic s)
  {
    return s;
  }
};


struct [[contract_label(goto)]] kw_label1 : dummy_label {};
struct [[contract_label(int)]] kw_label2 : dummy_label {};

struct [[contract_label(labels)]] l1 : dummy_label {};
struct [[contract_label(foo)]] l2 : dummy_label {};
struct [[contract_label(labels::bar)]] l3 : dummy_label {};

struct [[contract_label(unfinished)]] unfinished_label;

int main(int, char**)
{
  [[ assert goto int : true ]];
  [[ assert labels::foo : true ]]; // { dg-error "not declared" }
  [[ assert labels::bar : true ]];
  [[ assert unfinished : true ]]; // { dg-error "incomplete" }
}

struct [[contract_label(labels::  bar)]] l3b : dummy_label {}; // { dg-error "redeclaration" }

int fn1()
  [[ post [a, b] : false ]]; // { dg-error "expected .]." }
// { dg-error "expected .:." "" { target *-*-* } .-1 }

int fn2()
  [[ post [a,] : false ]]; // { dg-error "expected .]." }
// { dg-error "expected .:." "" { target *-*-* } .-1 }

int fn3()
  [[ post [default] : false ]]; // { dg-error "expected identifier" }
// { dg-error "expected .]." "" { target *-*-* } .-1 }
// { dg-error "expected .:." "" { target *-*-* } .-2 }

int fn4()
  [[ post [a : false ]]; // { dg-error "expected .]." }

int fn5()
  [[ pre [r] : false ]]; // { dg-error "post" }

int fn6()
  [[ pre default audit : false ]]; // { dg-error "value_type" }

struct [[contract_label(d2)]] d2_label
{
  using value_type = int;
};
struct [[contract_label(d3)]] d3_label
{
  using value_type = int;
};

int fn7()
  [[ pre d2 d3 : false ]]; // { dg-error "value_type" }

