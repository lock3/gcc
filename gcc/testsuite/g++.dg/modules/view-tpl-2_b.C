// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

export protected template<typename T> foons::FooPair<int, T> restrict x;

/*
template<typename T>
using BlahSpec = FooPair<int, T>;
export protected BlahSpec restrict x;
*/

