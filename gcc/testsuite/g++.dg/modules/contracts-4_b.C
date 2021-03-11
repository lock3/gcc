// { dg-additional-options "-std=c++20 -fmodules-ts -Kpre=ignore" }
module foo;

int fn_int(int n)
{
  return fn_t(n);
}

