// { dg-additional-options "-fmodules-ts" }
export module bar;
// { dg-module-cmi bar }
export import foo;

export protected Foo permit;

