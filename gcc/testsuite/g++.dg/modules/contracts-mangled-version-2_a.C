// test to ensure that equal configs from multiple TUs are compatible
// { dg-additional-options "-std=c++20 -fmodules-ts -fcontracts -fcontract-versioning" }
export module foo;
// { dg-module-cmi foo }

export template<typename T>
void fun(int n)
  [[ pre: n > 0 ]]
  [[ pre audit: n > 0 ]]
  [[ pre assume: n > 0 ]]
  [[ pre check_maybe_continue: true ]]
  [[ pre ignore: true ]]
  [[ pre assume: true ]]
{
}

export void use_fun(int n);

export template<typename T>
void fun2(int n)
  [[ pre ignore: true ]]
  [[ pre assume: true ]]
{
}

export template<typename T>
void fun3(int n)
  [[ pre ignore: true ]]
  [[ pre assume: true ]];

export template<typename T>
void fun_both(int n)
  [[ pre ignore: true ]]
  [[ pre assume: true ]]
{
  [[ assert ignore: true ]];
  [[ assert assume: true ]];
}

export template<typename T>
void fun_inner(int n)
{
  [[ assert ignore: true ]];
  [[ assert assume: true ]];
}

