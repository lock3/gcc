// { dg-do compile }
// { dg-options "-std=c++20 -fcontracts" }
#include <experimental/contracts>

struct [[contract_label(foo)]] foo_label { };

namespace std::experimental::contracts 
{
  struct [[contract_label(axiom)]] axiom_label
  {
    static consteval semantic adjust_semantic(attribute, semantic s)
    {
      return semantic::assume;
    }
  };
}

consteval auto ce_foo()
{
  using std::experimental::contracts::attribute;
  using std::experimental::contracts::semantic;
  using std::experimental::contracts::axiom_label;
  auto z = axiom_label::adjust_semantic(attribute::assertion, semantic::ignore);
  return z;
}

int main(int, char **)
{
  auto z = ce_foo();
}

int bar2(int n)
  [[ pre axiom: n > 0 ]]
{
  return -n;
}

template<typename T>
T tfun(T n)
  [[ pre axiom: n > 0 ]]
{
  return -n;
}

