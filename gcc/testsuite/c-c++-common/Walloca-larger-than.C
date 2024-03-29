/* PR middle-end/100425 - missing -Walloca-larger-than with -O0
   { dg-do compile }
   { dg-options "-O0 -Wall -Walloca-larger-than=128" } */

typedef __SIZE_TYPE__ size_t;

#if __cplusplus
extern "C"
#endif

void* alloca (size_t);

void sink (void*);

void warn_alloca_too_large (void)
{
  sink (alloca (1));
  sink (alloca (128));
  sink (alloca (129));    // { dh-warning "\\\[-Walloca-larger-than" }
  sink (alloca (1024));   // { dh-warning "\\\[-Walloca-larger-than" }
}
