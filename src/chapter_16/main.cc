/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2026/04/02 14:42:31
# Desc   : chapter_16: 特化与重载
########################################################################
*/

#include <bitset>
#include <cassert>
#include <cstddef>
#include <print>
#include "cpp_utils/util.h"

// 16.1 当"泛型代码"不是特别适用的时候
template <typename T>
class Array {
 private:
  T* data;
  std::size_t len_;
 public:
  Array(T* other, std::size_t len=1): data(other), len_(len) {}
  Array(const Array&) = default;
  Array& operator=(const Array&) = default;
  void exchangeWith(Array* b) {
    T* tmp = data;
    data = b->data;
    b->data = tmp;
    std::swap(len_, b->len_);
  }
  T& operator[](std::size_t k) {
    return data[k];
  }
  std::size_t size() const {
    return len_;
  }
};  // class Array
template <typename T>
inline void exchange(T* a, T* b) {
  // 当 T = Array<P> 时，下面的实现性能很差
  T tmp(*a);  // 拷贝构造
  *a = *b;  // 拷贝赋值运算 operator=
  *b = tmp;  // 拷贝赋值运算 operator=
}
// 16.1.1 透明自定义
// 特化 exchange<Array<T>>
template <typename T>
inline void exchange(Array<T>* a, Array<T>* b) {
  a->exchangeWith(b);
}
// 16.1.2 语义的透明性
struct S {
  int x;
} s1, s2;  // struct S
int h{0};
int i{0};

template <typename T>
void exchange_array(Array<T>* a, Array<T>* b) {
  T* p = &(*a)[0];
  T* q = &(*b)[0];
  for (std::size_t k = 0; k < a->size(); ++k) {
    exchange(p++, q++);
  }
}
void distinguish() {
  {
    Array<int> a1(&h);
    Array<int> a2(&i);
    int* p = &a1[0];  // 语义上，p 指向 a1 中的数组的地址
    a1[0] = 1;
    a2[0] = 2;
    assert(&h == &a1[0]);
    assert(&i == &a2[0]);
    assert(p == &h);  // 实际上, p 指向 h
    assert(1 == h);
    assert(2 == i);

    exchange(&a1, &a2);

    assert(&i == &a1[0]);
    assert(&h == &a2[0]);
    assert(p == &h);  // 调用 exchange 后 p 还是指向 h，但此时 h 是 a2 中的数组的地址了
    assert(p == &a2[0]);
    assert(1 == h);
    assert(2 == i);
    // 我们期望 p 在 exchange 前后都指向 a1 中的数组的地址，但 p 在语义上和实际上的含义有差异，导致行为不符合预期
  }
  {
    int* q = &s1.x;  // 语义上， q 指向 s1 中成员变量 x
    s1.x = 1;
    s2.x = 2;
    assert(q == &s1.x);

    exchange(&s1, &s2);  // 调用 exchange 后，只是交换了 s1.x 和 s2.x 的值，不影响 q
    assert(q == &s1.x);
    assert(2 == s1.x);
    assert(1 == s2.x);
  }
  // 由于 exchange<Array<T>> 特化的实现问题，在调用 exchange 函数后, p 和 q 的逻辑不一致, 这样会让使用者感到困惑
  {
    Array<int> a1(&h);
    Array<int> a2(&i);
    int* p = &a1[0];  // 语义上，p 指向 a1 中的数组的地址
    a1[0] = 1;
    a2[0] = 2;
    assert(&h == &a1[0]);
    assert(&i == &a2[0]);
    assert(p == &h);  // 实际上, p 指向 h
    assert(1 == h);
    assert(2 == i);

    exchange_array(&a1, &a2);

    assert(&h == &a1[0]);
    assert(&i == &a2[0]);
    assert(p == &h);  // 调用 exchange 后 p 还是指向 h，此时 h 还是 a1 中的数组的地址了
    assert(p == &a1[0]);
    assert(2 == h);
    assert(1 == i);
  }
}

void run_distinguish() {
  PRINT_CURRENT_FUNCTION_NAME;
  distinguish();
}

// 16.2 重载函数模板
// 函数名和模板参数名都相同
template <typename T>
int f(T) { PRINT_CURRENT_FUNCTION_NAME; return 1; }
template <typename T>
int f(T*) { PRINT_CURRENT_FUNCTION_NAME; return 2; }

// 函数签名相同时，重载解析规则将不知道应该选择哪一个函数
// 当模板出现在不同的编译单元时，就可以在同一个程序中同时存在



void run_overload() {
  PRINT_CURRENT_FUNCTION_NAME;
  assert(1 == f<int*>((int*)nullptr));  // f<T>(T), T = int*
  assert(2 == f<int>((int*)nullptr));  // f<T>(T*), T = int

// 16.2.2 重载的函数模板的局部排序
  assert(1 == f(0));  // f<T>(T), T = int
  assert(1 == f(nullptr));  // f<T>(T), T = std::nullptr_t

  // f<T>(T), T = int* 和 f<T>(T*), T = int 都可以匹配成功, 这也就意味着调用存在二义性
  // 重载解析规则: 选择产生自更特殊的模板的函数, 这里 f<T>(T*)
  assert(2 == f((int*)nullptr));  // f<T>(T*), T = int,
}

// 16.2.3 正式的排序规则
// 1. 默认实参包含的函数调用参数和未使用的省略号参数，在后面的内容中将不再考虑
template<typename T> void bar(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("bar<T>");
}
template <typename T, typename U> void bar(T t, U = U{}) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("bar<T, U>");
}
template<typename T> void test(T t) { }
template<typename T> void test(T t, ...) { }  // C语言风格的可变参数，和C++ <typename... Args>(Args... args) 不同
void no_more_consideration() {
  PRINT_CURRENT_FUNCTION_NAME;
  /*
  对于 bar<T, U>(T, U=U{}), 在调用 bar(1) 时，由于默认参数 U = U{} 的存在，模板参数 U 不再参与类型推导
  对于 test<T>(T, ...), 在调用 test(1)时，由于未使用省略号参数，省略号参数的类型也不再参与类型推导
  */
  bar(1);
  // test(1);
}

// 2. 通过替换每个模板参数来虚构两个不同的实参类型(虚设参数列表)
// a. 用唯一的类型替换每个模板类型参数
template <typename T, typename U>
void haha(T, U);  // -> void haha(Type1, Type2);
// b. 用唯一的类模板替换每个模板参数
template <template<typename> class TT, typename T>
void hihi(TT<T>, T);  // -> void hihi(TemplateType1<Type1>, Type1);
// c. 用适当类型的唯一值替换每个非类型模板参数
template<int N, typename T>
void baz(T (&arr)[N]);  // -> void baz(Type1(&arr)[0]);
// 第2步的3小步执行完后，相当于每个模板函数都有了一个新的名字(签名)[返回类型，函数名，类型模板参数，非类型模板参数]

/* 3. 如果第2个模板可以成功对第1个综合参数类型列表进行模板实参演绎，并且能够精确匹配，则第1个模板比第2个模板更加特殊
解释:
  - B 能推导 A, A 不能推导 B, 则 A 比 B 更特殊
  - A 能推导 B, B 不能推导 A, 则 B 比 A 更特殊
  - A、B 都能相互推导，或者都无法相互推导，则认为两个模板之间没有排序关系
  - 一定要是精确匹配
*/
template <typename T>
void say(T*, T);  // 模板函数 A
template <typename T, typename U>
void say(T*, U);  // 模板函数 B
/*
用 B 推导 A
  第一步: 对 A 虚设参数列表，得到 void say(Type1*, Type1)
  第二步: 用列表推导 B <T, U>,
    T* 匹配 Type1* -> T = Type1
    U 匹配 Type1 -> U = Type1
  推导成功
用 A 推导 B
  第一步: 对 B 虚设参数列表, 得到 void say(Type2*, Type3)
  第二步: 用列表推导 A <T>,
    T* 匹配 Type2* -> T = Type2
    T 匹配 Type3 -> T = Type3
  Type2 != Type3 矛盾, 推导失败
所以 A 比 B 更特殊
*/
/*
对于调用 f((int*)nullptr), 分析函数 f 的排序
template <typename T>
int f(T);  // -> A : int f(Type1)
template <typename T>
int f(T*);  // -> B: int f(Type2*)
A 推导 B:
  - B: f(Type2*)
  - A<T>: T = Type2*
  - 推导成功
B 推导 A:
  - A: f(Type1)
  - B<T>: T* = Type1, 无法推导出 T
  - 推导失败
所以 B 比 A 更特殊
*/

template <typename T>
void t(T*, const T* = nullptr, ...);
template <typename T>
void t(const T*, T*, T* = nullptr);
void example(int* p) {
  // t(p, p);  // 存在二义性
}
/*
对于调用 t(p, p), 没有使用第一个函数的省略号参数和第二个函数的最后一个具有默认值的参数，因此在排序时不考虑这些参数
A 推导 B:
  - B: t(const Type2*, Type2*)
  - A<T>:
    - T* = const Type2*
    - const T* = Type2*
    - 令 T = const X 可以演绎成功，但不是精确匹配，因为需要调整限定符
B 推导 A:
  - A: t(Type1*, const Type1*)
  - B<T>:
    - const T* = Type1*
    - T* = const Type1*
    - 令 T = const Y 可以演绎成功，但不是精确匹配
所以 A, B 没有排序关系
*/

// 16.2.4 模板和非模板
// 在其他条件相同的时候，实际的函数调用会优先选择非模板函数
template <typename T>
std::string g(T) { return "Template"; }
std::string g(int&) { return "NonTemplate"; }
void template_and_nontemplate_1() {
  PRINT_CURRENT_FUNCTION_NAME;
  int x = 7;
  assert("NonTemplate" == g(x));
}
// 当常量和引用限定符不同时，重载解析规则优先级可能会改变
template <typename T>
std::string gg(T&) { return "Template"; }
std::string gg(const int&) { return "NonTemplate"; }
void template_and_nontemplate_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  int x = 7;
  assert("Template" == gg(x));  // T = int，实例化的 gg<>(int&) 比 gg(const int&) 更匹配
  const int c = 8;
  // 当 T = const int时，gg<>(const int&) 和 gg(const int&) 的签名具有相同的 const int& 类型
  // 此时优先选择非模板函数
  assert("NonTemplate" == gg(c));
}

class C {
 public:
  C() = default;
  C(const C&): value(1) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("copy constructor");
  }
  C(C&&): value(2) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("move constructor");
  }
  // 当成员函数模板可能隐藏拷贝或移动构造函数时，通常必须部分禁用他们, 否则其优先级可能出普通构造函数高
  // 见 chapter_06 中的 class Person5, Person6, 通过 concept requires enable_if 来禁用部分类型匹配
  template <typename T>
  C(T&&): value(3) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("template constructor");
  }
  /*
  1. 使用 enable_if
    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, C>>
  2. 使用 requires
    template <typename T> requires (!std::is_same_v<std::decay_t<T>, C>)
  */
  int value{0};
};  // class C

void template_and_nontemplate_3() {
  PRINT_CURRENT_FUNCTION_NAME;
  C x;  // default constructor
  assert(0 == x.value);

  C x2{x};  // template constructor, T = C, 如果 x 是 const C 就会调用拷贝构造函数
  assert(3 == x2.value);

  C x3{std::move(x)};  // move constructor
  assert(2 == x3.value);

  const C c;  // default constructor
  assert(0 == c.value);

  C x4{c};  // copy constructor
  assert(1 == x4.value);

  C x5{std::move(c)};  // template constructor, T = const C, std::move(c)的类型是 const C&&
  assert(3 == x5.value);
}

// 16.2.5 变参函数模板
template <typename T>
int hh(T*) { PRINT_CURRENT_FUNCTION_NAME; return 1; }
template <typename... Ts>
int hh(Ts...) { PRINT_CURRENT_FUNCTION_NAME; return 2; }
template <typename... Ts>
int hh(Ts*...) { PRINT_CURRENT_FUNCTION_NAME; return 3; }

void variadic_arguments() {
  assert(2 == hh(0, 0.0));  // hh<>(Ts...),  Ts... == int, double

  /* hh(Ts...) 和 hh(Ts*...) 都可以推导成功, 但 hh(Ts*...) 比 hh(Ts...) 更特殊，所以hh(Ts*..) 比 hh(Ts...)优先
  A: hh(Ts...)
  B: hh(Ts*...)
  A 推导 B:
    - B: hh(TypeB1*, TypeB2*)
    - A<T1, T2>:
      - T1 = TypeB1*
      - T2 = TypeB2*
    A 推导 B 成功
  B 推导 A:
    - A: hh(TypeA1, TypeA2)
    - B<T1, T2>
      - T1* = TypeA1
      - T2* = TypeA2
      - 推导失败，无法得知 T1, T2 类型

  */
  assert(3 == hh((int*)nullptr, (double*)nullptr));  // hh<>(Ts*...), Ts = int, double

  /* 三个调用都成功,
  比较 hh<T>(T*) 和 hh<Ts...>(Ts*...), 相互都可以推导成功，排序会产生歧义
  但是，规则禁止来自函数参数包的实参匹配非参数包的形参。也就是非参数包参数比参数包参数优先级更高。
  这样，可以认为 hh<T>(T*) 推导 hh<Ts...>(Ts*...)失败, hh<T>(T*) 更特化

  */
  assert(1 == hh((int*)nullptr));  // hh(T*)
}

// 对于函数签名中的类型中的包扩展，也和 variadic_arguments 中的规则相同
template <typename... Ts> class Tuple{};
template <typename T> int f(Tuple<T*>) { PRINT_CURRENT_FUNCTION_NAME; return 1; }
template <typename... Ts> int f(Tuple<Ts...>) { PRINT_CURRENT_FUNCTION_NAME; return 2; }
template <typename... Ts> int f(Tuple<Ts*...>) { PRINT_CURRENT_FUNCTION_NAME; return 3; }

void variadic_arguments_in_class() {
  assert(2 == f(Tuple<int, double>()));
  assert(3 == f(Tuple<int*, double*>()));
  assert(1 == f(Tuple<int*>()));
}

void run_ordering_rule() {
  PRINT_CURRENT_FUNCTION_NAME;
  no_more_consideration();
  template_and_nontemplate_1();
  template_and_nontemplate_2();
  template_and_nontemplate_3();
  variadic_arguments();
  variadic_arguments_in_class();
}

// 16.3 显式特化
// 类模板和变量模板不能被重载，通过特化来实现类似函数模板选择最佳匹配的功能
// 16.3.1 全局的类模板特化
//  - 别名模板不能特化
template <typename T>
class SS {
 public:
 void info() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("generic (SS<T>::info)");
 }
};
// 全局特化的实现不需要和泛型定义有任何关联
template <>
class SS<void> {
 public:
  void msg() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("fully specialized (SS<void>::msg())");
  }
};  // class SS<void>

template <typename T>
class Types {
 public:
  using I = int;
};  // class Types
template <typename T, typename U = typename Types<T>::I>
class BB;
template <>
class BB<void> {  // BB<void, Types<void>::I> 指定的模板实参列表必须和模板参数列表一一对应
 public:
  void f();
};  // class BB<void>
template <> class BB<char, char>;
// template <> class BB<char, 0>;  // 不可以为类型模板参数指定非类型值
void run_specialized() {
  PRINT_CURRENT_FUNCTION_NAME;
  BB<int>* p1;  // 指针类型，不用定义
  // BB<int> p2;  // 没有定义
  BB<void>* p3;  // 特化类型, BB<void, Types<void>::void>
  BB<void, int> p4;  // 特化类型 BB<void, int>
  // BB<void ,char> p5;  // 没有定义
  // BB<char, char> p6;  // 特化类型，没有定义
}

template <typename T> class CC;
template <>
class CC<char**> {
 public:
  void print() const;
};  // class CC<char**>
// 全局特化声明不是模板声明，在类外定义成员时，不能指定 template <>
// 全局特化声明就是一个带有符合模板参数的普通类，所以不能用 template <>
void CC<char**>::print() const {
  std::println("pointer to pointer to char");
}
// 定义print()时, 不能写成 template <> void CC<char**>::print() const {}

template <typename T>
class Outside {
 public:
  template <typename U>
  class Inside {
  };  // class Inside
};  // class Outside
template <>
class Outside<void> {
  template <typename U>
  class Inside {
    private:
      static int count;
  };  // class Inside
};  // class Outside<void>

// Outside<void> 是特化，Inside<U> 不是特化
// 所以 Outside<void> 前面不能用 template<> 前缀, 但定义 count 时需要 template <typename U> 前缀
template <typename U>
int Outside<void>::Inside<U>::count = 1;

// 程序中如果同时存在模板的显示版本和生成版本，程序将是无效的
template <typename T>
class Invalid {
};  // class Invalid
Invalid<double> x1;  // 产生一个 Invalid<double> 实例化实体
// template <> class Invalid<double>;  // 此时再特化 Invalid<double> 就是无效的，因为前面 Invalid<double> 已经被实例化

// 但是，不同的实例化出现在不同的编译单元，问题就很难被发现
template <typename T>
class Danger {
 public:
  enum { max = 10 };
};  // class Danger
char buffer[Danger<void>::max];
extern void clear(char*);

void run_specialized_danger() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("Danger<void>::max = {}", std::size(buffer));
  assert(10 == std::size(buffer));
  // 上面 buffer 数组的大小就是 10, 因为这个单元在编译时，没有遇到特化的 Danger<void>
  // 但是在 clear 函数中，使用了 danger.h 特化的 Danger<void> 中的 max = 100
  // 必须注意确保特化的声明对泛型模板的所有使用者可见, 特化的声明通常应该跟在模板头文件的声明之后
  // 避免使用来自外部源的模板特化.
  clear(buffer);
}

// 16.3.2 函数模板全特化
template <typename T>
int g(T) { PRINT_CURRENT_FUNCTION_NAME; return 1; }
template <typename T>
int g(T*) { PRINT_CURRENT_FUNCTION_NAME; return 2; }
// 特化 template<> int g<int>(int)
template <> int g<int>(int) { PRINT_CURRENT_FUNCTION_NAME; return 3;}
// 特化 template<> int g<int>(int*)
template <> int g<int>(int*) { PRINT_CURRENT_FUNCTION_NAME; return 4; }
// 当特化模板可以通过参数推导和部分排序确定时，全特化声明可以省略显示的模板参数
// 也就是当特化 template<> int g<double>(double) 时, 可以直接写成如下:
template <> int g(double) { PRINT_CURRENT_FUNCTION_NAME; return 5; }

// 一个完整的函数模板特化只提供一个替代定义，而没有提供替代声明，因此，签名必须完全匹配
// 就是说只能特化定义，不能特化声明，声明的签名必须一样
template <typename T> auto foo();
// no: 签名不一致，函数模板的返回类型是 auto，特化的返回类型是 int
// template <> int foo<int>() { return 42; }  // 签名
// yes
template <> auto foo<int>() { return 42; }

// 完整函数模板特化不能包含默认参数值
// "包含"就是说，在特化时不能修改(update)默认参数，也不能添加(add)默认参数, 特化就不能有默认参数值
template <typename T>
int f(T, T x = 42) { return x; }
// 修改默认参数
// template <> int f(int, int = 35) { return 0;}  // ERROR
template <typename T>
int gogogo(T) { return 1; }
// 添加默认参数: Default argument not permitted on an explicit specialization of function
// template<> int gogogo(int = 3) { return 3; }

// 为特化模板指定默认参数仍然适用于显式特化
// 就是说，特化模板可以使用主模板的默认参数
template <typename T>
void say_hi(T x = 1) { PRINT_CURRENT_FUNCTION_NAME; std::println("x: {}", x);}
template <>
void say_hi(int x) {  // 这里不能为 x 提供默认值，默认值由主模板继承来, 也就是 1
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("x: {}", x);
}
void run_say_hi() {
  PRINT_CURRENT_FUNCTION_NAME;
  say_hi<double>();  // 主模板 say_hi<double>(1)
  say_hi(3.14);  // 主模板 say_hi<double>(3.14)
  say_hi(2);  // 特化模板 say_hi<int>(2)
  say_hi<int>();  // 特化模板 say_hi<int>(1), 使用主模板的默认参数值
}

// 16.3.3 全局的变量模板特化
template <typename T> constexpr std::size_t SZ = sizeof(T);
template <> constexpr std::size_t SZ<void> = 0;
template <typename T> typename T::iterator null_iterator;
// 变量模板特化时，变量类型不需要和被特化的模板类型匹配
// 特化类型: int, 被特化的模板类型 T::iterator
template <> int /*BitIterator*/ null_iterator<std::bitset<100>>;

// 16.3.4 全局的成员特化

template <typename T>
class Outer {
 public:
  template <typename U>
  class Inner {
   private:
    static int count;
  };  // class Inner
  static int code;
  void print() const {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("generic");
  }
};  // class Outer
template <typename T>
int Outer<T>::code = 6;
template <typename T> template <typename U>
int Outer<T>::Inner<U>::count = 7;

template<>
class Outer<bool> {
 public:
  template <typename U>
  class Inner {
   private:
    static int count;
  };  // class Inner
  void print() const {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("specialized");
  }
};  // class Outer<bool>

// 类模板的普通静态成员数据和成员函数也可以被全特化, 语法要求给每个封闭类模板加上 template<> 前缀
// 这个前缀表明当前是全特化, 而不是模板
template<>
int Outer<void>::code = 12;
template<>
void Outer<void>::print() const {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("Outer<void>");
}

// 对于类的函数和静态数据成员来说，非定义的类外声明在C++中是不允许的。也就是普通类的静态成员不能在类外声明，在类外只有定义
// 但类模板的特化成员可以在类外声明
template<>
int Outer<double>::code;  // 声明，而不是定义
template<>
void Outer<double>::print() const;

class DefaultInitOnly {
 public:
  DefaultInitOnly() = default;
  DefaultInitOnly(const DefaultInitOnly&) = delete;
};  // class DefaultInitOnly
template <typename T>
class Static {
 private:
  static T sm;
};  // class Static
// 对于只能使用默认构造函数初始化的类型，对静态数据成员进行全局特化，只能使用初始化列表
// 也就是说，对于 Outer<double>::code 这种看起来像是使用了默认构造函数，但它其实会被认为是声明，如果想要进行定义，
// 必须使用初始化列表 {}, Outer<double>::code{}
// 也就是用了 {} 就是定义，没用就是声明
template <>
DefaultInitOnly Static<DefaultInitOnly>::sm;  // 这个是声明
template <>
DefaultInitOnly Static<DefaultInitOnly>::sm{};  // 这个是定义

template <>
template <typename U>
class Outer<wchar_t>::Inner {  // 特化成员模板
 public:
  static long count;  // 特化后改变了 count 的类型, 从 int 变为 long
};  // class Outer<wchar_t>::Inner
template <>
template <typename X>
long Outer<wchar_t>::Inner<X>::count;

template <> template <>
class Outer<char>::Inner<wchar_t> {
 public:
  enum { count = 1 };
};  // class Outer<char>::Inner<wchar_t>

// 由于 Outer<char> 没有被全局特化，此时特化 Outer<char>::Inner<wchar_t> 就需要写两个 template <> 前缀
// 但是 Outer<bool> 已经在前面的代码中被全局特化了，此时 Outer<bool> 就是一个普通的类，于是在特化其中的 Inner 时，
// 只需要为 Inner 的特化写一个 template <> 前缀
template<>  // 这是写给 Inner 的
class Outer<bool>::Inner<wchar_t> {
 public:
  enum { count = 2 };
};  // class Outer<bool>::Inner<wchar_t>

// 16.4 类模板偏特化
template <typename T>
class List {
 public:
  void append(const T&);
  inline std::size_t length() const;
};  // class List
template <>
class List<void*> {
 public:
  void append(void* p);
  inline std::size_t length() const;
};  // class List<void*>
// 偏特化指针
template <typename T>
class List<T*> {
 private:
  List<void*> impl_;
 public:
  inline void append(T* p) {
    impl_.append(p);
  }
  inline std::size_t length() const {
    return impl_.length();
  }
};  // class List<T*>


template <typename T, int I = 3>
class GG;
// 1. 偏特化的实参必须和基本模板的相应参数在种类(类型、非类型或模板)上相匹配
// template <typename T> class GG<int, T>;  // 错误，参数种类不匹配

// 2. 偏特化的参数列表不能有默认实参，但可以使用基本模板的默认实参
// template <typename T = int> class GG<T, 10>;  // 错误, 不能有默认实参

// 3. 偏特化的非类型模板实参应该是非依赖型值，或普通的非类型模板参数，它们不能是更复杂的依赖型表达式
// template <int I> class GG<int, I * 2>;  // 错误：不能有非类型的表达式

// 4. 偏特化的模板实参列表不应和基本模板的参数列表相同
// template <typename U, int K> class GG<U, K>;  // 错误：和基本模板没有区别，只是模板参数名字不同而已

// 5. 加入其中一个模板实参是包扩展的，那么它必须位于模板参数列表的末尾
template <typename... Ts>
class Tuple;
// template <typename Tail, typename... Ts>
// class Tuple<Ts..., Tail>;  // 错误：包扩展必须在末尾
template <typename Tail, typename... Ts>
class Tuple<Tuple<Ts...>, Tail>;  // 正确: 包扩展在嵌套的模板实参列表的末尾

// 特化 List<void* C::*>，任何指向 void* 的成员指针类型的偏特化
template <typename C>
class List<void* C::*> {
 public:
  using ElementType = void* C::*;
  void append(ElementType pm);
  inline std::size_t length() const;
};  // class List<void* C::*>
template <typename T, typename C>
class List<T* C::*> {
 private:
  List<void* C::*> impl;
 public:
  using ElementType = T* C::*;
  inline void append(ElementType pm) {
    impl.append((void* C::*)pm);
  }
  inline std::size_t length() const {
    return impl.length();
  }
};  // class List<T* C::*>

// 16.5 变量模板偏特化

// 16.6 后记
int main() {
  run_distinguish();
  run_overload();
  run_ordering_rule();
  run_specialized();
  run_specialized_danger();
  run_say_hi();
  return 0;
}


