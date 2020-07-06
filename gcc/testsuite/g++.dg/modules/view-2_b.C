// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

export protected Foo permit x, y;
export protected Foo2 restrict x, y;
export protected ::Foo3 permit e1, u2;
export protected Foo4 restrict ~Foo4;
export protected Foo4B restrict Foo4B;
export protected Foo5 restrict Foo5;
export protected Foo6 permit x;
export protected Foo7 permit x;
export protected Foo8 restrict prot;
export protected FooOp restrict operator +=, operator int;

