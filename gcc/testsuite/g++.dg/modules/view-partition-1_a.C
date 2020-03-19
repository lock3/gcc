// { dg-additional-options "-fmodules-ts" }
export module M:P;
// { dg-module-cmi M:P }

export struct A 
{
  int x;
  float y;
};

export protected ::A restrict x;