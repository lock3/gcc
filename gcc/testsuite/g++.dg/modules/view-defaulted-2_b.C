// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

export protected Foo1 permit x;
export protected Foo2 permit x;
export protected Foo3 permit x;
export protected Foo4 permit x;
export protected Foo5 permit x, Foo5;

