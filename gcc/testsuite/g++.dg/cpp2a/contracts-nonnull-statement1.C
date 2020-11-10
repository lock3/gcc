// { dg-do compile { target c++17_only } }
// { dg-additional-options "-fcontracts" }
#include <cstdio>

template<typename T>
T fn(T t)
{
  [[ assert: t > 0 ]]
  [[ assert audit %custom: t > 0 ]]
  printf("test\n"); // { dg-error "assertions must be followed by .;." }
  return t;
}

