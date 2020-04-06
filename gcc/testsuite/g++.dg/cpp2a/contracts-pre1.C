// generic pre contract parsing checks
//   check omitted, 'default', 'audit', and 'axiom' contract levels parse
//   ensure that an invalid contrcat level 'off' errors
//   ensure that a predicate referencing an undefined variable errors
//   ensure that a missing colon after contract level errors
// { dg-do compile }
// { dg-options "-std=c++2a -fcontracts" }

void f1(int x) [[ pre: x >= 0 ]] { }
void f2(int x) [[ pre default: x >= 0 ]] { }
void f3(int x) [[ pre audit: x >= 0 ]] { }
void f4(int x) [[ pre axiom: x >= 0 ]] { }

void finvalid(int x) [[ pre invalid: x >= 0 ]] { } // { dg-error "expected contract level" }
void fundeclared() [[ pre: x >= 0 ]] { } // { dg-error ".x. was not declared in this scope" }
void fmissingcolon(int x) [[ pre default x == 0]] { } // { dg-error "expected .:. before .x." }

void (*fp1)(int x) [[ pre: x > 0 ]]; // { dg-error "only allowed on functions" }
void (*fp2 [[ pre: x > 0 ]])(int x); // { dg-error "only allowed on functions" }
typedef void (*fp3)(int x) [[ pre: x > 0 ]]; // { dg-error "only allowed on functions" }
typedef void (*fp4 [[ pre: x > 0 ]])(int x); // { dg-error "only allowed on functions" }
fp3 fn5(int a) [[ pre: a > 0 ]]; // { dg-bogus "only allowed on functions" }

int main()
{
  return 0;
}
