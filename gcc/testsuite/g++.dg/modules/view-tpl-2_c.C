// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

void fn4()
{
  foons::FooPair<double, double> foopair_double_double;
  foopair_double_double.second = 5.3; // { dg-bogus "export protected" }
}

void fn5()
{
  foons::FooPair<int, int> foopair_int_int;
  foopair_int_int.y = 5; // { dg-bogus "export protected" }
}

void fn6()
{
  foons::FooPair<int, double> foopair_int_double;
  foopair_int_double.x = 5; // { dg-error "export protected" }
}

void fn7()
{
  foons::FooPair<int, int> foopair_int_int;
  foopair_int_int.x = 5; // { dg-bogus "export protected" }
  foopair_int_int.y = 7; // { dg-bogus "export protected" }
}

