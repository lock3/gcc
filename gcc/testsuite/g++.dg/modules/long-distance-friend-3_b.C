// { dg-additional-options "-fmodules-ts -Wlong-distance-friends" }
#include "long-distance-friend-3_b.hpp"

// { dg-warning "is not visible to befriended decl" "" { target *-*-* } 6 }
// { dg-warning "is not visible to befriended decl" "" { target *-*-* } 7 }

// { dg-error "does not name a type" "" { target *-*-* } 12 }
// { dg-error "does not name a type" "" { target *-*-* } 13 }

