// { dg-additional-options "-fmodules-ts -Wlong-distance-friends" }
module;
#include "long-distance-friend-2_a.hpp"
export module foo;
// { dg-module-cmi foo }

class B
{
  friend A; // { dg-warning "is not visible to befriended decl" }
  friend class A2; // { dg-warning "is not visible to befriended decl" }
};

class B2
{
  friend A3; // { dg-warning "is not visible to befriended decl" }
  friend class A4; // { dg-warning "is not visible to befriended decl" }
};

