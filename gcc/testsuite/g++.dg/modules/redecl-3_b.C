// { dg-additional-options "-fmodules-ts -fcontracts" }
import foo;

struct Bar
{
  friend int foo();
};

int foo() // { dg-error "conflicts with import" }
{
  return 0;
}

