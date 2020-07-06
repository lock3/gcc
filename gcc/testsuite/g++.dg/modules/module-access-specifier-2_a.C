// { dg-additional-options "-fmodules-ts" }
module;
#include "module-access-specifier-2_a.hpp"
export module foo; // { dg-warning "not writing module" }

void fn()
{
  GMF gmf{};
  gmf.x; // { dg-error "restricted to the global module" }
}

struct T1
{
  restrict foo; // { dg-error "does not name a type" }
};
struct T2
{
  restrict: foo; // { dg-error "does not name a type" }
};
struct T3
{
  restrict :foo; // { dg-error "does not name a type" }
};

