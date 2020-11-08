// { dg-additional-options "-std=c++20 -fmodules-ts -fcontracts -fcontract-versioning -fcontract-role=default:maybe,maybe,ignore" }
module foo;

void use_fun(int n)
{
  fun<int>(n);
}

void use_fun2(int n)
{
  fun2<int>(n);
}

void use_others(int n)
{
  fun_both<int>(n);
  fun_inner<int>(n);
}

// ensure version is mangled into generated symbols
// { dg-final { scan-assembler "_Z3funIiEvi.VC_OOAOIA" } }
// { dg-final { scan-assembler "_Z4fun2IiEvi.VC_IA" } }
// { dg-final { scan-assembler "_Z8fun_bothIiEvi.VC_IAIA" } }
// { dg-final { scan-assembler "_Z9fun_innerIiEvi.VC_IA" } }

