// { dg-module-do run }
// { dg-additional-options "-fmodules-ts -fcontracts -fcontract-role=default:maybe,maybe,ignore" }
export module bar;
import foo;

int main(int, char**)
{
  foo1(-5);
  FooT<int> foot;
  foot.foo2(-5);
  foot.foo3(-5);
  foot.foo4(-5);
  foot.foo5(-5);
  foot.foo6(-5);
  FooT<int>::foo4(-6);
  FooT<int>::foo6(-6);
  foons::foo7(-7);
  foons::foo8(-8);
  foons::foo9(-9);

  FooT2<int>::Inner foot2_inner;
  foot2_inner.foo10(-10);
  foot2_inner.foo11(-11);

  FooT3<int>::Inner foot3_inner;
  foot3_inner.foo12(-12);
  foot3_inner.foo13(-13);

  FooT2<double>::Inner::foo14(-14);

  foo15(-15);
  return (violation_count == 12 && violation_line_sum == 5036) ? 0 : -1;
}

