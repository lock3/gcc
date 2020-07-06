#pragma once
import foo;

class A
{
  friend Foo;
  friend FooComplete;
};

class A2
{
  friend FooPrivate;
  friend FooPrivateComplete;
};

