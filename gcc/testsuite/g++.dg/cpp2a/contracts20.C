// test that contract attributes cause errors pre-c++2a
// { dg-do compile { target c++17_only } }

int fun(int a)
  [[ pre: a > 0 ]]
  [[ post [r]: r < 0 ]]
{
  [[ assert: a != 0 ]];
  return -a;
}

