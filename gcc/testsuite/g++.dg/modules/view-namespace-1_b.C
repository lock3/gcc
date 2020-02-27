// { dg-additional-options "-fmodules-ts" }
export module bar;
export import foo;

void fn()
{
  foons::fn();
}
export protected foons restrict Foo;
export protected foons restrict fn;
export protected foons restrict TFoo;

namespace ns1 {
  export protected ns2 restrict NFoo;
  export protected ns2::NFoo2 restrict x;
  // FIXME can we not reopen a namespace to add to it?
  //export struct Bar { int x; };
}
export protected ns1::ns2::NFoo2 restrict y;

// FIXME global restrictions
//export protected :: restrict globalfn, GlobalFoo;

