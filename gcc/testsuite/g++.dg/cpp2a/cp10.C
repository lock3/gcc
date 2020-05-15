
template<typename T>
struct S
{
  template<typename Q>
  int f(Q a) [[ pre: a > 0 ]];

#if 0
  template<typename Q>
  int g(int a) [[ pre: a > 0 ]]
  {
    return -a;
  }
#endif
};

template<typename T>
template<typename Q>
int S<T>::f(Q a) [[ pre: a > 0 ]]
{
  return -a;
}

int main(int, char**)
{
  S<int> s_int;
  s_int.f<int>(-5);
  s_int.f<double>(-5);

#if 0
  S<double> s_double;
  s_double.f<int>(-5);
  s_double.f<double>(-5);
#endif
  return 0;
}
