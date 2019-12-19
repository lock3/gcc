// { dg-additional-options -fmodules-ts }
import foo;

struct Bar // { dg-error "cannot declare.*in a different module" }
{
};

