// { dg-additional-options "-fmodules-ts" }
export module baz; // { dg-warning "not writing module" }
import bar;

foons::Foo dummy; // { dg-error "does not name a type" }

export protected foons permit Foo2; // { dg-error "may only .restrict." }

