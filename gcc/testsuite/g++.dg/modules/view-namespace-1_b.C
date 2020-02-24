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
}

// FIXME global restrictions
//export protected :: restrict globalfn, GlobalFoo;

