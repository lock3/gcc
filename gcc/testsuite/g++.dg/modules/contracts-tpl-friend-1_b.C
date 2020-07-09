// { dg-additional-options "-fmodules-ts -fcontracts" }
module foo;

void fn(int x, void *p) [[ pre: x > 0 ]] {
  auto *obj = reinterpret_cast<FooT<int> *> (p);

  obj->memb = x;
}
void fn(double x, void *p) [[ pre: x > 0 ]] {
  auto *obj = reinterpret_cast<FooT<double> *> (p);

  obj->memb = x;
}

