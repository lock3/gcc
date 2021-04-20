// { dg-do compile { target c++17_down } }
#include <experimental/contracts>
// { dg-error "only available with -std=c..20" "" { target c++17_down } 36 }

