// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=ignore" }
export module bar;
import foo;

export int bar_fn_int(int n)
{
  return fn_t(n);
}

