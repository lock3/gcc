// { dg-additional-options "-fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

export protected; // { dg-error "expected unqualified-id" }
export protected Foo; // { dg-error "expected .permit. or .restrict." }
export protected Foo permit; // { dg-error "expected unqualified-id" }
export protected Foo restrict; // { dg-error "expected unqualified-id" }
export protected Foo permit bad; // { dg-error "not a member" }
export protected Foo permit bad, ; // { dg-error "expected unqualified-id" }
// { dg-error "not a member" "" { target *-*-* } .-1 }

/* export protected ignores access */
export protected FooAccess restrict prot_x; // { dg-bogus "is protected" }
export protected FooAccess restrict priv_x; // { dg-bogus "is private" }
export protected FooAccess restrict mod_x; // { dg-bogus "is restricted" }

export protected Foo permit quux, quux; // { dg-error "more than once" }

export protected ::Foo2 restrict x;
export protected ::Foo2 permit x; // { dg-error "already restricted" }

export protected ::Foo3 restrict x;
export protected ::Foo3 restrict x; // { dg-error "already restricted" }

export protected Foo4 restrict xyz; // { dg-error "not a member" }

export protected FooChild restrict x; // { dg-error "not a member" }
export protected FooChild restrict foo; // { dg-error "not a member" }

export protected FooChild2 restrict x, y; // { dg-bogus "not a member" }
export protected FooChild2 restrict foo; // { dg-bogus "not a member" }

export protected FooChild3 restrict foo; // { dg-bogus "not a member" }

export protected FooChild4 restrict Inner; // { dg-error "not a member" }
//export protected FooChild5 restrict Inner; // dg-bogus "not a member"

export protected FooChild6::Inner restrict y; // { dg-bogus "not a member" }
export protected FooChild6 restrict Inner; // { dg-bogus "not a member" }

export protected FooIncomplete restrict x; // { dg-error "incomplete type" }

export protected foons::FooNested1 permit x; // { dg-error "not been declared" }

export protected Foo5 restrict operator double; // { dg-error "not a member" }
export protected Foo5 restrict operator int; // { dg-error "not a member" }

export protected Foo6 restrict operator int; // { dg-bogus "not a member" }

