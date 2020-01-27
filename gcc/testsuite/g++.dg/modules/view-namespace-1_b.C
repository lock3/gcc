// { dg-additional-options "-fmodules-ts" }
export module bar; // { dg-warning "not writing module" }
import foo;

foons::Foo dummy1;
export protected foons restrict Foo;
foons::Foo dummy2; // { dg-error "does not name a type" }

export protected foons permit Foo2; // { dg-error "may only .restrict." }

