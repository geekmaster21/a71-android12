// RUN: %clang_cc1 -triple aarch64-linux-eabi -std=c++11 -general-regs-only %s -verify -DBANNED=float -Wno-unused-value
// RUN: %clang_cc1 -triple aarch64-linux-eabi -std=c++11 -general-regs-only %s -verify -DBANNED=int '-DVECATTR=__attribute__((ext_vector_type(2)))' -Wno-unused-value
// RUN: %clang_cc1 -triple aarch64-linux-eabi -std=c++11 -general-regs-only %s -verify -DBANNED=FloatTypedef -Wno-unused-value

using FloatTypedef = float;

#ifndef VECATTR
#define VECATTR
#define BannedToInt(x) (x)
#else
#define BannedToInt(x) ((x)[0])
#endif

typedef BANNED BannedTy VECATTR;

namespace default_args {
int foo(BannedTy = 0); // expected-error 2{{use of floating-point or vector values is disabled}}
int bar(int = 1.0);

void baz(int a = foo()); // expected-note{{from use of default argument here}}
void bazz(int a = bar());

void test() {
  foo(); // expected-note{{from use of default argument here}}
  bar();
  baz(); // expected-note{{from use of default argument here}}
  baz(4);
  bazz();
}
} // namespace default_args

namespace conversions {
struct ConvertToFloat {
  explicit operator BannedTy();
};
struct ConvertToFloatImplicit { /* implicit */
  operator BannedTy();
};
struct ConvertFromFloat {
  ConvertFromFloat(BannedTy);
};

void takeFloat(BannedTy);
void takeConvertFromFloat(ConvertFromFloat);
void takeConvertFromFloatRef(const ConvertFromFloat &);

void test() {
  BannedTy(ConvertToFloat());

  takeFloat(ConvertToFloatImplicit{}); // expected-error{{use of floating-point or vector values is disabled}}

  ConvertFromFloat(0);                        // expected-error{{use of floating-point or vector values is disabled}}
  ConvertFromFloat(ConvertToFloatImplicit{}); // expected-error{{use of floating-point or vector values is disabled}}

  ConvertFromFloat{0};                        // expected-error{{use of floating-point or vector values is disabled}}
  ConvertFromFloat{ConvertToFloatImplicit{}}; // expected-error{{use of floating-point or vector values is disabled}}

  takeConvertFromFloat(0);    // expected-error{{use of floating-point or vector values is disabled}}
  takeConvertFromFloatRef(0); // expected-error{{use of floating-point or vector values is disabled}}

  takeConvertFromFloat(ConvertFromFloat(0));    // expected-error{{use of floating-point or vector values is disabled}}
  takeConvertFromFloatRef(ConvertFromFloat(0)); // expected-error{{use of floating-point or vector values is disabled}}
}

void takeImplicitFloat(BannedTy = ConvertToFloatImplicit()); // expected-error{{use of floating-point or vector values is disabled}}
void test2() {
  takeImplicitFloat(); // expected-note{{from use of default argument here}}
}
} // namespace conversions

namespace refs {
struct BannedRef {
  const BannedTy &f;
  BannedRef(const BannedTy &f) : f(f) {}
};

BannedTy &getBanned();
BannedTy getBannedVal();

void foo() {
  BannedTy &a = getBanned();
  BannedTy b = getBanned(); // expected-error{{use of floating-point or vector values is disabled}}
  const BannedTy &c = getBanned();
  const BannedTy &d = getBannedVal(); // expected-error{{use of floating-point or vector values is disabled}}

  const int &e = 1.0;
  const int &f = BannedToInt(getBannedVal()); // expected-error{{use of floating-point or vector values is disabled}}

  BannedRef{getBanned()};
  BannedRef{getBannedVal()}; // expected-error{{use of floating-point or vector values is disabled}}
}
} // namespace refs

// The struct assignment special casing we do isn't really intended for C++ at
// the moment.
namespace struct_assign {
struct Foo {
  BannedTy f;
  int i;

  Foo(const BannedTy &f, int i);

  Foo() = default;
  Foo(const Foo &) = default;
  Foo(Foo &&) = default;

  Foo &operator=(const Foo &) = default; // expected-error{{use of floating-point or vector values is disabled}}
  Foo &operator=(Foo &&) = default; // expected-error{{use of floating-point or vector values is disabled}}
};

void foo(Foo &f) {
  Foo a = f; // expected-error{{use of floating-point or vector values is disabled}}
  Foo b = static_cast<Foo &&>(f); // expected-error{{use of floating-point or vector values is disabled}}
  Foo c = Foo{}; // expected-error{{use of floating-point or vector values is disabled}}
  Foo d = Foo{1, 1}; // expected-error{{use of floating-point or vector values is disabled}}

  Foo *p = &f;
  *p = f; // expected-note{{in implicit copy assignment operator for}}
  *p = // expected-note{{in implicit move assignment operator for}}
    static_cast<Foo &&>(f);
}

struct Bar { // expected-error 2{{use of floating-point or vector values is disabled}}
  BannedTy f;
  int i;
};

void bar(Bar &r) {
  Bar a = r; // expected-error{{use of floating-point or vector values is disabled}}
  Bar b = static_cast<Bar &&>(r); // expected-error{{use of floating-point or vector values is disabled}}

  Bar *p = &r;
  *p = a; // expected-note{{in implicit copy assignment operator for}}
  *p = // expected-note{{in implicit move assignment operator for}}
    static_cast<Bar &&>(b);
}
}

namespace class_init {
struct Foo {
  float f = 1.0; // expected-error{{use of floating-point or vector values is disabled}}
  int i = 1.0;
  float j;

  Foo() : j(1) // expected-error{{use of floating-point or vector values is disabled}}
  {}
};
} // namespace class_init

namespace templates {
float bar();

template <typename T>
T foo(int t = bar()) { // expected-error 2{{use of floating-point or vector values is disabled}}
  return t;            // expected-error{{use of floating-point or vector values is disabled}}
}

void test() {
  foo<float>(9); // expected-error{{use of floating-point or vector values is disabled}} expected-note{{in instantiation of function template specialization}}
  foo<float>();  // expected-error{{use of floating-point or vector values is disabled}} expected-note{{in instantiation of default function argument}} expected-note{{from use of default argument}}
}
} // namespace templates

namespace base_classes {
struct Foo {
  BannedTy f;
};

struct Bar : Foo {};
struct Baz : virtual Foo {};

struct Nothing {};
struct Qux : Nothing, Baz {};

Foo getFoo() { // expected-error{{use of floating-point or vector values is disabled}}
  __builtin_trap();
}
Bar getBar() { // expected-error{{use of floating-point or vector values is disabled}}
  __builtin_trap();
}
Baz getBaz() { // expected-error{{use of floating-point or vector values is disabled}}
  __builtin_trap();
}
Qux getQux() { // expected-error{{use of floating-point or vector values is disabled}}
  __builtin_trap();
}
} // namespace base_classes

namespace constexprs {
constexpr float foo = 1.0;
constexpr int bar = 1.0;

constexpr int getFoo() { return 1; }

constexpr float baz = getFoo();
constexpr int qux = baz + foo + bar;

int getVal() {
  return qux;
}

int getVal2() {
  return baz;
}

constexpr int transformFoo(float foo) { // expected-error{{use of floating-point or vector values is disabled}}
  return foo + 1;                       // expected-error{{use of floating-point or vector values is disabled}}
}

int getVal3() {
  constexpr int val = transformFoo(1);
  int val2 = transformFoo(1); // expected-error{{use of floating-point or vector values is disabled}}
}
} // namespace constexprs

namespace rvalue_refs {
float &&foo();
void bar() {
  auto x = foo(); // expected-error{{use of floating-point or vector values is disabled}}
  auto &&y = foo();
}
} // namespace rvalue_refs
