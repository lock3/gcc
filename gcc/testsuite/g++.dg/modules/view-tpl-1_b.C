// { dg-additional-options "-fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

export protected Foo restrict x;
void fn()
{
  Foo<int> foo_int;
  foo_int.x = 5; // { dg-error "export protected" }

  Foo<double> foo_double;
  foo_double.x = 5.3; // { dg-error "export protected" }

  Foo<char> foo_char;
  foo_char.xyz = 1.7; // { dg-bogus "export protected" }
}

using Blah = Foo<char>;
export protected Blah restrict xyz;
void fn2()
{
  Foo<char> foo_char;
  foo_char.xyz = 1.7; // { dg-error "export protected" }
}

export protected Foo<Bar> restrict xyz;
void fn3()
{
  Foo<Bar> foo_bar;
  foo_bar.xyz = 1.7; // { dg-error "export protected" }
}

struct Dummy { };

export protected Foo<Dummy> restrict x; // { dg-error "view of a template instantion" }

export protected FooPair restrict second;
/* FIXME
export protected template<typename T> FooPair<int, T> restrict x;
export protected template<> FooPair<int, int> restrict y;
*/

