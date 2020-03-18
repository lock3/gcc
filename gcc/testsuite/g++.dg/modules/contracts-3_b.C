// { dg-additional-options "-fmodules-ts -fcontracts -fcontract-versioning -fcontract-role=default:maybe,maybe,ignore" }
module foo;

int fn_int(int n)
{
  return fn_t(n);
}

