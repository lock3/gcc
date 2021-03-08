// { dg-do compile }
// { dg-additional-options { -std=c++2a -Kkt=true -Kko=on -Kky=yes -Kke -Kkf=false -Kkoff=off -Kkn=no -Kfoo=foo -Kbar=bar -Kn1=123 -Kn2=-50 -Knn=12n -Kb1=FILE_NOT_FOUNDY -Kb2=Truthy } }
#include <cstddef>

/* If T is bool, the result is true if v is "true", "on", "yes", or "" and
   false if v is "false", "off", or "no".
   Otherwise, the program is ill-formed.  */
bool f1()
{
  static_assert(__meta_maybe_getenv("kt", bool, false)); // { dg-bogus "not a boolean value" }
  static_assert(__meta_maybe_getenv("ko", bool, false)); // { dg-bogus "not a boolean value" }
  static_assert(__meta_maybe_getenv("ky", bool, false)); // { dg-bogus "not a boolean value" }
  static_assert(__meta_maybe_getenv("ke", bool, false)); // { dg-bogus "not a boolean value" }
  static_assert(!__meta_maybe_getenv("kf", bool, true)); // { dg-bogus "not a boolean value" }
  static_assert(!__meta_maybe_getenv("koff", bool, true)); // { dg-bogus "not a boolean value" }
  static_assert(!__meta_maybe_getenv("kn", bool, true)); // { dg-bogus "not a boolean value" }
  static_assert(!__meta_maybe_getenv("n3", bool, false)); // { dg-bogus "not a boolean value" }

  bool b7 = __meta_maybe_getenv("foo", bool, false); // { dg-error "not a boolean value" }
  bool b8 = __meta_maybe_getenv("bar", bool, false); // { dg-error "not a boolean value" }
  return b7 || b8;
}

/* If T is a signed integer type, the result is equivalent to
   (T)std::strtoll(v, ptr, 0). If, after the call, *ptr would not point to the
   end of v, the program is ill-formed.  */
void f2()
{
  static_assert(__meta_maybe_getenv("n1", int, -1) == 123); // { dg-bogus "not a signed integral" }
  static_assert(__meta_maybe_getenv("n2", int, -1) == -50); // { dg-bogus "not a signed integral" }
  static_assert(__meta_maybe_getenv("n3", int, -1) == -1); // { dg-bogus "not a signed integral" }
  __meta_maybe_getenv("nn", int, -1); // { dg-error "not a signed integral" }
}

/* If T is an unsigned integer type, the result is equivalent to
   (T)std::strtoull(v, ptr, 0). If after the call, *ptr would not point to the
   end of v, the program is ill-formed.  */
void f3()
{
  static_assert(__meta_maybe_getenv("n1", unsigned int, 17u) == 123); // { dg-bogus "not an unsigned integral" }
  static_assert(__meta_maybe_getenv("n2", unsigned int, 17u) == -50); // { dg-bogus "not an unsigned integral" }
  static_assert(__meta_maybe_getenv("n3", unsigned int, 17u) == 17u); // { dg-bogus "not an unsigned integral" }
  __meta_maybe_getenv("nn", unsigned int, 17u); // { dg-error "not an unsigned integral" }
}

/* If T is an enumeration type, the result is that of the [id-expression] n
   where n is the enumerator found by lookup in the scope of T. If no such
   enumerator is found, the program is ill-formed.  */
enum Boool
{
  Truthy = 3,
  Falsey = 5,
  FILE_NOT_FOUNDY = 7,
  INVALID = 9,
};

void f4()
{
  static_assert(__meta_maybe_getenv("b1", Boool, FILE_NOT_FOUNDY) == FILE_NOT_FOUNDY); // { dg-bogus "not an enumerator of .Boool." }
  static_assert(__meta_maybe_getenv("b2", Boool, Truthy) == Truthy); // { dg-bogus "not an enumerator of .Boool." }
  static_assert(__meta_maybe_getenv("b3", Boool, INVALID) == INVALID); // { dg-bogus "not an enumerator of .Boool." }
  __meta_maybe_getenv("foo", Boool, INVALID); // { dg-error "not an enumerator of .Boool." }
  __meta_maybe_getenv("bar", Boool, INVALID); // { dg-error "not an enumerator of .Boool." }
}

/* If T is const char*, the result is a pointer to a const char array
   containing the characters of the build value for k and a terminating null
   character.  */
consteval bool f5(const char *zoo)
{
  constexpr auto foo = __meta_maybe_getenv("foo", const char *, (const char *)NULL);
  if (!(foo[0] == 'f' && foo[1] == 'o' && foo[2] == 'o' && foo[3] == '\0'))
    return false;
  constexpr auto bar = __meta_maybe_getenv("bar", const char *, (const char *)NULL);
  if (!(bar[0] == 'b' && bar[1] == 'a' && bar[2] == 'r' && bar[3] == '\0'))
    return false;
  constexpr auto z = __meta_maybe_getenv("zoo", const char *, (const char *)NULL);
  if (z != NULL)
    return false;
  return true;
}

int main(int, char**)
{
  static_assert(f5("zoo"));
}

