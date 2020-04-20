// test that contracts are matched on friend decls when the type is complete
// { dg-do compile }
// { dg-options "-std=c++2a -fcontracts" }

struct T;

int both(int x, T *t) [[ pre: x > 0 ]] { return 0; }
int both2(int x, T *t) [[ pre: x > 0 ]];
int both3(int x, T *t) [[ pre: x > 0 ]];
int both4(int x, T *t) [[ pre: x > 0 ]];

template<typename Z>
int fn(int x, Z *z) [[ pre: x > 0 ]];

template<typename Z>
int fn2(int x, Z *z);

template<typename Z>
int fn3(int x, Z *z) [[ pre: x > 0 ]];

template<>
int fn3<T>(int x, T *z) [[ pre: x > 1 ]];

struct T
{
  friend int both(int x, T *t) [[ pre: x > 1 ]]; // { dg-error "mismatched" }
  friend int both2(int x, T *t) [[ pre: x > 1 ]] // { dg-error "mismatched" }
  {
    return 0;
  }
  friend int both3(int x, T *t)
    [[ pre: x > 1 ]] [[ pre: t->pri > 0 ]]; // { dg-error "mismatched" }
  friend int both4(int x, T *t)
    [[ pre: x > 1 ]] [[ pre: t->pri > 0 ]]; // { dg-error "mismatched" }
  friend int hidden(int x, T *t)
    [[ pre: x > 1 ]] [[ pre: t->pri > 0 ]]
  {
    return x;
  }

  // fine, spec can have its own contracts
  friend int fn<T>(int x, T *t)
    [[ pre: t->pri > 0 ]]; // { dg-bogus "mismatched" }
  // bad, general contracts must match general
  template<typename Z>
  friend int fn(int x, Z *z)
    [[ pre: z->pri > 1 ]]; // { dg-error "mismatched" }

  // fine, can add contracts
  template<typename Z>
  friend int fn2(int x, Z *z)
    [[ pre: z->pri > 1 ]]; // { dg-bogus "mismatched" }

  // bad, spec's contracts must match spec's prior contracts
  friend int fn3<T>(int x, T *t)
    [[ pre: t->pri > 0 ]]; // { dg-error "mismatched" }

  friend int dup(int x, T *t)
    [[ pre: t->pri > 0 ]];
  friend int dup(int x, T *t)
    [[ pre: t->pri > 1 ]]; // { dg-error "mismatched" }

  int x{1};
  private:
    int pri{-10};
};

int both3(int x, T *t)
  [[ pre: x > 1 ]] [[ pre: t->pri > 0 ]] // { dg-error "mismatched" }
{
  return 0;
}
int both4(int x, T *t)
  [[ pre: x > 1 ]] [[ pre: t->pri > 0 ]]; // { dg-error "mismatched" }
int hidden(int x, T *t)
  [[ pre: x > 0 ]] [[ pre: t->pri > 1 ]]; // { dg-error "mismatched" }

