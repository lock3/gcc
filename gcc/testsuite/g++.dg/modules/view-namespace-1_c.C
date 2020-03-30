// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

foons::Foo dummy; // { dg-error "export protected" }

void fn()
{
  foons::fn(); // { dg-error "export protected" }
  auto foo = foons::make_foo(); // { dg-error "export protected" }
  foo.x = 5; // { dg-error "export protected" }

  globalfn(); // { dg-error "export protected" }
  GlobalFoo gf{}; // { dg-error "export protected" }

  foons::TFoo<int> tfoo_int; // { dg-error "export protected" }

  ns1::ns2::NFoo nfoo; // { dg-error "export protected" }

  ns1::ns2::NFoo2 nfoo2;
  nfoo2.x = 5; // { dg-error "export protected" }
  nfoo2.y = 5; // { dg-error "export protected" }

  //ns1::Bar bar;
  //bar.x = 10;

  foons2::fn2();
  foons2::fn(); // { dg-error "export protected" }
  foons2::Foo f; // { dg-error "export protected" }

  foons3::fn(); // { dg-error "export protected" }
}

export protected foons permit Foo2; // { dg-bogus "may only .restrict." }
export protected (::) permit globalfn2; // { dg-error "may only .restrict." }

export protected foons3 restrict fn2; // { dg-error "not a member" }

