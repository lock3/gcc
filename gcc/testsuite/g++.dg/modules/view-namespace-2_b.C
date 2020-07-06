// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

export protected (::) restrict globalfn, GlobalFoo;

