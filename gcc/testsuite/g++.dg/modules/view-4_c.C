// { dg-additional-options "-fmodules-ts" }
export module baz;
// { dg-module-cmi baz }
export import foo;

export protected Foo restrict y, z;
export protected foons restrict Foo2, z;
export protected foons::Foo2 restrict x;

