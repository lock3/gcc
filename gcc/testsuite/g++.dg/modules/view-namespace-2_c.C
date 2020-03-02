// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

void fn()
{
  globalfn(); // { dg-error "export protected" }
  GlobalFoo gf{}; // { dg-error "export protected" }
}

