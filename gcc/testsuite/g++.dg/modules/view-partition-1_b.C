// { dg-additional-options "-fmodules-ts" }
export module M;
// { dg-module-cmi M }
export import :P;

export protected ::A restrict y;
