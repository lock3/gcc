// { dg-additional-options -fmodules-ts }
import foo;

int Foo::foo() // { dg-error "conflicts with import" }
{
  return 1;
}

