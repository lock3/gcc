// { dg-additional-options "-fmodules-ts" }
export module bar;
// { dg-module-cmi bar }
export import foo;

export protected Foo restrict x, z;
export protected foons restrict fn, z;

