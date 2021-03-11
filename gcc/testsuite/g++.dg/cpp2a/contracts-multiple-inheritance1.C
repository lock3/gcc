// { dg-do compile }
// { dg-options "-std=c++2a" }

struct BaseA {
  virtual int fun(int n) [[ pre: n > 0 ]] { return -n; }
};

struct BaseB {
  virtual int fun(int n) [[ pre: n > 0 ]] { return -n; }
};

struct Child : public BaseA, BaseB {
  int fun(int n) [[ pre: n > 0 ]] { return -n; }
};

