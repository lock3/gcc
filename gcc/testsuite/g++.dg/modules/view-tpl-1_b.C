// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

export protected Foo restrict x;

export using Blah = Foo<char>;
export protected Blah restrict xyz;

export protected Foo<Bar> restrict xyz;

export struct Dummy { };

export protected FooPair restrict second;
export protected template<> FooPair<int, int> restrict y;
export protected template<typename T> FooPair<int, T> restrict x;

export template<typename T>
using BlahSpec = FooPair<int, T>;

export protected BlahSpec restrict x;

