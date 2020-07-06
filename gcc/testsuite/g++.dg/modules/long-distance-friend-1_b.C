// { dg-additional-options "-fmodules-ts -Wlong-distance-friends" }
import foo;

struct Bar
{
  friend int foo(); // { dg-warning "is not visible to befriended decl" }
  friend class Foo; // { dg-warning "is not visible to befriended decl" }
  friend class FooComplete; // { dg-warning "is not visible to befriended decl" }
};

struct Bar2
{
  friend int templateFoo<int>(int); // { dg-warning "is not visible to befriended decl" }

  friend int templateFoo<double>(double); // { dg-warning "is not visible to befriended decl" }

  friend class TemplateFoo<int>; // { dg-warning "is not visible to befriended decl" }


  friend class TemplateFooComplete<int>; // { dg-warning "is not visible to befriended decl" }

  friend class TemplateFooComplete<double>; // { dg-warning "is not visible to befriended decl" }
};

