// { dg-additional-options "-fmodules-ts -Wlong-distance-friends" }
export module foo;
// { dg-module-cmi foo }

export class Foo;
export class FooComplete {};
class FooPrivate;
class FooPrivateComplete {};

