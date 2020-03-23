// Test to ensure the [[versioned]] attribute is correctly applied to
// functions directly and through various scopes.
// { dg-additional-options "-fmodules-ts -fcontracts -fcontract-role=default:maybe,maybe,ignore" }
module;
#include <cstdio>
#include <contract>
export module foo;
// { dg-module-cmi foo }

export int violation_count{0};
export int violation_line_sum{0};
export void handle_contract_violation(const std::contract_violation &violation)
{
  violation_count++;
  violation_line_sum += violation.line_number () * violation_count;
  printf("violation on line %d: %d %d\n", violation.line_number (),
    violation_count, violation_line_sum);
}

export template<typename T>
[[versioned]] void foo1(T t)
  [[ pre: t > 0 ]]
{
  [[ assert: false ]];
}

export template<typename T>
struct [[versioned]] FooT
{
  void foo2(T t)
    [[ pre: t > 0 ]]
  {
    [[ assert: false ]];
  }

  void foo3(int t)
    [[ pre: t > 0 ]]
  {
    [[ assert: false ]];
  }

  static void foo4(int t)
    [[ pre: t > 0 ]]
  {
    [[ assert: false ]];
  }

  template<typename S>
  void foo5(S s)
    [[ pre: s > 0 ]]
  {
    [[ assert: false ]];
  }

  template<typename S>
  static void foo6(S s)
    [[ pre: s > 0 ]]
  {
  }
};

namespace [[versioned]] foons
{
  export template<typename T>
  void foo7(T t)
    [[ pre: t > 0 ]]
  {
    [[ assert: false ]];
  }

  export void foo8(int n)
    [[ pre: n > 0 ]]
  {
    [[ assert: false ]];
  }
}

namespace foons
{
  export void foo9(int n)
    [[ pre: n > 0 ]]
  {
    [[ assert: false ]];
  }
}

export template<typename T>
struct FooT2
{
  struct [[versioned]] Inner
  {
    void foo10(int n)
      [[ pre: n > 0 ]]
    {
      [[ assert: false ]];
    }

    void foo11(T n)
      [[ pre: n > 0 ]]
    {
      [[ assert: false ]];
    }

    static void foo14(int n)
      [[ pre: n > 0 ]]
    {
      [[ assert: false ]];
    }
  };
};

