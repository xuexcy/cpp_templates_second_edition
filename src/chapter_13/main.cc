/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/18 14:39:18
# Desc   : chapter_12 模板中的名称
########################################################################
*/

// 13.1 名称的分类

// 13.2 名称查找
#include <print>
#include "cpp_utils/util.h"
int x;
class B {
public:
  int i;
};  // class B
class D : public B{
};  // class D
void f(D* pd) {
  pd->i = 3;
  //  D::x = 2;
}

// 13.2.2 依赖于参数的友元声明的查找
template <typename T>
class C {
  friend void f() {}
  friend void f(const C<T>&) {}
};  // class C

void g(C<int>* p) {
  // f();  // 这个 f() 它没有关联的类或命名空间, 因为没有任何参数， 所以是一个无效的调用
  f(*p);  // 关联类 C<int> 和全局命名空间(C<int> 的命名空间就是全局的)
}

// 13.2.3 注入的类名称
int J;
class J {
private:
  int i[2];
public:
  static int j() {
    return sizeof(J);
  }
};  // class J
int j() {
  return sizeof(J);
}

template <template<typename> typename TT>
class X {
};  // class X
template <typename T>
class K {
  K* a;  // 等价于 K<T>*
  K<void>& b;
  X<K> c;  // K 后面没有模板参数，此时表示模板 K<T>
  X<::K> d;  // ::K 还是表示模板(没有::的K是注入的，K 表示类模板 K 还是模板类 K<T> 视上下文定， :: 表示不是注入的，永远表示类模板 K)
};  // class K

void run_class_name_injection() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("J::j() = {} , ::j() = {}", J::j(), ::j());  // 8 4
  std::println();
}

// 13.2.3 当前的实例化
template <typename T>
class Node {
  using Type = T;
  Node* next;  // 指涉当前的实例化，也就是 Node<T>*
  Node<Type>* previous;  // Node<Type> 指涉当前的实例化, 因为 Type 已经明确的指向了 T, Node<Type> 等价于 Node<T>
  Node<T*>* parent;  // Node<T*> 指涉未知的特化, 因为当前的实例化为 Node<T>，此时并不一定实例化了 Node<T*>
};  // class Node

template <typename T>
struct L {
  using Type = T;
  struct M {
    L* l;  // 指涉当前实例化
    L<Type>* l2;  // 指涉当前实例化
    M* m;  // 指涉当前实例化
  };  // struct M
  struct N {
    L* l;  // 指涉当前实例化
    L<Type>* l2;  // 指涉当前实例化
    M* m;  // 指涉未知的特化, 因为 N 并不是包含在 M 中的, 如果我们为 L<int>::M 编写一个显示特化，那么 L<int>::N 中的这个 M 将指向那个显示特化，而不是当前实例化
    N* n;  // 指涉当前实例化
  };  // struct N
};  // class L
template <>
struct L<int>::M {
  int a;
};  // struct L<int>::M

L<int> t;
L<int>::M m;
L<int>::N n;  // L<int>::N 中的 M 指向显示特化的 L<int>::M, 它只包含一个成员变量 a
SAME_TYPE(int, decltype(decltype(n)::m->a));

// 13.3 解析模板
// 13.3.1 非模板中的上下文相关性
/*
X<1>(0)
如果 X 是类模板，则会用 0 作为参数构造一个 X<1> 类型的实例
如果不是，则等价于 (X<1) > 0, 相当于先用 X 和 1 比较大小，再将结果和 0 比较大小
*/
template <bool B>
class Invert {
public:
  static const bool result = !B;
};  // class Invert
void run_invert() {
  bool test = Invert<(1>0)>::result;
  // bool test2 = Invert<1>0>::result;  // 1 后面的 > 会被认为是模板参数列表的结束标记
  // 上述 = 右侧的表达式将等价于 (Invert<1>)0 > :: result
  // 如果 0 可以隐式转换成 Invert<1> ,  并存在 ::result ，那么就会导致结果不符合我们的预期
}
// 13.3.2 依赖类型的名称
template <typename T>
struct Trap {
  enum { x };
};  // struct Trap
template <typename T>
struct Victim {
  int y;
  void poof() {
    // 此时 Trap<T> 是一个未知的特化，和前面提到的 L<T>::N 中的 M 一样，指涉未知的特化
    // Trap<T>::x * y;  // 当编译器解析到这里时，只看到了前面的 Trap 定义，由于 x 是一个枚举值，这里的 * 被认为是乘号
  }
};  // struct Victim
template <>
struct Trap<void> {
  using x = int;
};  // struct Trap<void>
void boom(Victim<void>& bomb) {
  // 当实例化 Victim<void> 时，也会实例化 Trap<void>, 后者的实例化的源代码会覆盖泛型 Trap<T>::x, 此时 x 变成了
  // 一个类型，于是和在解析 Victim 时将 * 认为乘号产生了冲突(类型int乘以一个值y， 这是不合法的)
  bomb.poof();
}

// 13.3.3 依赖模板的名称
template <typename T>
struct Shell {
  struct In {
    template <int M>
    struct Deep {
      virtual void f();
    };  // struct Deep
  };  // struct In
};  // struct Shell

template <typename T, int N>
struct Weird {
  void case1(typename Shell<T>::template In<N>::template Deep<N>* p) {
    PRINT_CURRENT_FUNCTION_NAME;
    // 编译器不会查找 Deep 来判断是否是模板（也许是成员变量），所以需要显示插入 template
    p->template Deep<N>::f();
    // 如果不写 template, 编译器会将 Deep 后面的 < 识别成小于号
    // ((p->Deep) < N ) > ::f();
  }
  void case2(typename Shell<T>::template In<N>::template Deep<N>* p) {
    PRINT_CURRENT_FUNCTION_NAME;
    p->template Deep<N>::f();
  }
};  // struct Weird

// 13.3.4 using 声明的依赖型名称
struct BX {
  void f(int);
  void f(const char*);
  void g();
};  // struct BX
struct DX : private BX {
  using BX::f;  // 将 BX 中的 f 引入 DX 中
  // 在使用 private BX 后， DX 中继承来的 f 对用户不可见，使用 using BX::f ，DX 中的 f 对用户可见
};  // struct DX

template <typename T>
struct BXT {
  using Mystery = T;
  template <typename U>
  struct Magic;

  template <typename U>
  void f() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct BXT
template <typename T>
struct DXTT : private BXT<T> {
  // 当不使用 typename 时，编译器就不知道 BXT<T>::Mystery 是一个类型还是一个变量
  using typename BXT<T>::Mystery;
  Mystery* p;
};  // struct DXTT

template <typename T>
struct DXTM : private BXT<T> {
  // C++ 标准并没有提供一个机制来引入一个模板, 如下语法是错的
  //  using BXT<T>::template Magic;

  // 此时需要使用 C++11 的别名模板来解决这个问题
  // 其实这样很麻烦, 如果 Magic 有很多模板参数，那么 using 的时候就要多写一写代码)
  template <typename U>
  using Magic = typename BXT<T>::template Magic<U>;
  Magic<T>* p;

  // 文中描述无法使用 using 引入函数模板，但本人在cpp23 + clang19.1.7 中测试是可以的
  using BXT<T>::f;
};  // struct DXTM

// 13.3.5 ADL 和显示模板参数
// argument-dependent lookup 参数依赖查找(只适用于函数模板，并依赖于函数参数)
/* 用于在调用函数模板时确定模板参数的类型
1. 先在函数所在空间查找
2. 再在调用函数的空间查找
3. 再到函数实参类型对应的命名空间查找
4. 前面都找不到，编译器报错
*/
namespace N {
struct X {
};  // struct X
template <int I>
void select(X* ) {
  PRINT_CURRENT_FUNCTION_NAME;
}

constexpr int j = 5;
}  // namespace N
void gg(N::X* xp) {
  // 书中描述的无法通过 ADL 找到模板函数 select
  // 这只适用于 cpp20 以前，在 cpp20 中下面代码是可以编译的
  // 根据 xp 的类型，编译器可以在 namespace N 中找到 select 模板函数
  select<3>(xp);

  // 无法确定 select 是什么
  // 1. 当前空间没有 select
  // 2. 函数调用空间，即 gg 所在的空间，没有 select
  // 3. 函数参数类型是 nullptr，也找到不到 select
  //  select<3>(nullptr);
  // 等价于

  // ADL 可以依赖函数参数，但并不依赖于模板参数，所以并不会去 N::j 所在的空间查找
  //  select<N::j>(nullptr);

}

// 13.3.6 依赖型表达式
// 表达式依赖于模板参数, 依赖模板参数的表达式在不同的实例化中的行为有可能不同
/*
1. 值依赖
template <size_t N> f() { N; }
2. 类型依赖
template <typename T> f(T t) { t; }
template <typename T> f2(T t) { call_a_func(t); }
3. 实例化依赖
template <typename T> f() { sizeof(T); }
  如果 T 是不完整类型，编译器会报错
*/

constexpr int i{1};
volatile int jj{2};
template <typename T> void type_dependent1(T&& x) {
  x;  // 这个表达式依赖于 x, 而 x 的类型依赖于模板参数 T
}
int call_a_func(int) {
  PRINT_CURRENT_FUNCTION_NAME;
  return 0;
}  // call_a_func
double call_a_func(double) {
  PRINT_CURRENT_FUNCTION_NAME;
  return 1.1;
}  // call_a_func

template <typename T> void type_dependent2(T x) {
  auto t = call_a_func(x);  // 这个表达式依赖于 x, 而 x 的类型依赖于模板参数 T
  // 当 T 是 Type1 时，调用 class Type1::call_a_func 的构造函数
  // 当 T 是 Type2 时，调用函数 void Type2::call_a_func(double);
  // 当 T 是 double 时，调用函数 call_a_func(double)
}
void run_type_dependent() {
  PRINT_CURRENT_FUNCTION_NAME;
  type_dependent1(i);  // 什么都不会发生, 编译器可能会提示表达式 "x;" 无用
  type_dependent1(jj);  // 编译器会让程序会从内存中读取一次 jj, 编译器不会提示 "x;" 无用"

  type_dependent2(1);  // 调用 int call_a_func(int)
  type_dependent2(1.0);  // 调用 double call_a_func(double)
  std::println();
}

// 13.3.7 编译错误
// C++ 编译器允许在解析模板时诊断错误
void hh() {}
template <size_t N>
void non_dependent_call() {
  // 我们知道，编译器会先解析表达式，和模板参数无关的表达式会进一步解析，和模板参数相关的表达式会在实例化时进行解析
  // 但是，编译器允许在第一步解析时，诊断模板错误

  // 每个 hh(N) 调用都会由于没有匹配的 hh 函数匹配, 所以解析 hh(N) 时会出错
  // 编译器第一次进行解析时，或者第一个模板实例化时产生错误
  // hh(N);
}

// 13.4 派生和类模板
// 13.4.1 非依赖型基类
template <typename X>
struct Base {
  int base_field;
  using T = int;
};  // struct Base
struct D1 : public Base<Base<void>> {
  void f() {
    base_field = 3;
  }
};  // struct D1
template <typename T>
struct D2 : public Base<double> {
  void f() { base_field = 7; }
  // 当在模板中查找受限名称时，会先查找非依赖型基类，在查找模板参数列表
  // 编译器会先到非依赖型基类 Base<Double> 中找到 T (也就是 int)
  T strange;
  SAME_TYPE(T, int);
};  // struct D2
void d(D2<int*>& d2, int* p) {
  // 类型不匹配，strange 是 int，不是模板参数类型 int*
  //  d2.strange = p;
}
// 13.4.2 依赖型基类
template <typename T>
struct DD : public Base<T> {
  // 这里不能直接写成 base_field = 0,  base_field 是一个非依赖型名称，编译器不会到依赖型基类中查找，于是报错
  // 当写成 this->base_field 时，它就是依赖型名称了
  void f() { this->base_field = 0; }
};  // struct DD
template <>
struct Base<bool> {
  enum { base_field = 42 };
};  // struct Base<bool>
void d2(DD<bool>& d) {
  // 由于 Base<bool> 的特化，其定义将会覆盖 Base<T> 中 base_field 的定义，一旦调用 d.f()
  // 编译器就会发现 Base<bool>::base_field 是一个不能改变的枚举值，而不是一个成员变量, 于是就会报错
  // d.f();
}
/*
在使用 this-> 时，存在一些问题
1. 如果 -> 指向的不是一个变量, 那么语法上就会存在问题，比如
*/
template <typename T>
struct BBB {
  enum E { e1 = 6 };
  virtual void zero(E e = e1) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct Base2
template <typename T>
struct DDD : public BBB<T> {
  void f() {
    // this->E e;  // 想要定义一个 E 的变量, 此时不能使用 this->
    typename DDD<T>::E e;  // 在多继承下，我们可能不知道哪个基类声明了 E, 此时就可以使用 DDD<T>::
    // 或者
    typename BBB<T>::E e2;  // 另外，在多继承时，可以避免多个基类声明了相同的名称, 此时就可以使用 BBB<T>::

    // 书上描述：DDD<T>::zero() 会禁止调用虚函数
    // 实际情况: 可以调用虚函数
    DDD<T>::zero();

    this->zero();
  }
};  // struct DD

void run_dependent_base() {
  PRINT_CURRENT_FUNCTION_NAME;
  DDD<int> d;
  d.f();
  std::println();
}

int main() {
  run_class_name_injection();
  run_type_dependent();
  run_dependent_base();
  return 0;
}


