// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

void fn()
{
  Foo<int> foo_int;
  foo_int.x = 5; // { dg-error "export protected" }

  Foo<double> foo_double;
  foo_double.x = 5.3; // { dg-error "export protected" }
}

void fn2()
{
  Foo<char> foo_char;
  foo_char.xyz = 1.7; // { dg-error "export protected" }
  foo_char.x = 7; // { dg-bogus "export protected" }
}

void fn3()
{
  Foo<Bar> foo_bar;
  foo_bar.xyz = 1.7; // { dg-error "export protected" }
}

export protected Foo<Dummy> restrict x; // { dg-error "view of a template instantion" }

void fn4()
{
  FooPair<double, double> foopair_double_double;
  foopair_double_double.second = 5.3; // { dg-error "export protected" }
}

void fn5()
{
  FooPair<int, int> foopair_int_int;
  foopair_int_int.y = 5; // { dg-error "export protected" }
}

void fn6()
{
  FooPair<int, double> foopair_int_double;
  foopair_int_double.x = 5; // { dg-error "export protected" }
}

void fn7()
{
  //FooPair<int, double> foopair_int_double;
  BlahSpec<double> foopair_int_double;
  foopair_int_double.x = 5; // { dg-error "export protected" }
}

