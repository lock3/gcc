// { dg-additional-options "-fmodules-ts -fcontracts -fcontract-versioning -fcontract-role=default:maybe,maybe,ignore" }
module foo;

int fn_int(int n)
{
  return fn_t(n);
}

int fn_int_inner(int n)
{
  return fn_t_inner(n);
}

