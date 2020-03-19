// { dg-additional-options "-fmodules-ts" }
import M;

int f() 
{
  A a;
  a.y = 1.0f; // { dg-error "export protected" }
  return a.x; // { dg-error "export protected" }
}