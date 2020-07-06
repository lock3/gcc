// { dg-additional-options "-fmodules-ts" }
export module foo;
// { dg-module-cmi foo }

namespace foons {
export template<typename T1, typename T2>
struct FooPair
{
  T1 first;
  T2 second;
};

export template<typename T2>
struct FooPair<int, T2>
{
  int x;
  T2 second;
};

export template<>
struct FooPair<int, int>
{
  int x, y;
};
}

