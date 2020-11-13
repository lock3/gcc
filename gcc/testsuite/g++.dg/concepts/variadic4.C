// PR c++/73456
// { dg-do compile { target c++17_only } }
// { dg-options "-fconcepts" }

template<typename...> struct list {};

template<typename Seq>
concept bool Sequence = true;

template<Sequence... Seqs>
  // requires (Sequence<Seqs> && ...)
struct zip;

template<Sequence... Seqs>
  requires true
  // requires (Sequence<Seqs> && ...) && requires { typename list<Seqs...>; }
struct zip<Seqs...> {}; // { dg-error "does not specialize" }
// The constraints of the primary template and specialization are not
// comparable because the generated fold expressions do not form identical
// atomic constraints. The specializations are unordered.

int main()
{
    zip<list<>, list<int>> {};
}
