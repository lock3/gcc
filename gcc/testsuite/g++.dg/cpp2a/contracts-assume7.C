// { dg-do compile }
// { dg-additional-options { -std=c++20 } }

int fn1(int n)
  [[ assert : false ]] // { dg-error "assert" }
{
  return -1;
}

int fn2(int n)
  [[ assume : false ]] // { dg-error "assume" }
{
  return -1;
}

