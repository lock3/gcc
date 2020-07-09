// { dg-additional-options "-fmodules-ts -fcontracts" }
export module foo;
// { dg-module-cmi foo }

void fn(int, void *);
void fn(double, void *);

template<typename T>
struct FooT {
  friend void fn(T, void *); // { dg-warning "non-template function" }

  T memb;
};

template class FooT<double>;  // instantiate

