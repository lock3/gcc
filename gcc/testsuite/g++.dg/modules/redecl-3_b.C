// { dg-additional-options "-fmodules-ts -Wlong-distance-friends" }
import foo;

struct Bar
{
  friend int foo(); // { dg-warning "is not visible to befriended decl" }
};

int foo() // { dg-error "conflicts with import" }
{
  return 0;
}

