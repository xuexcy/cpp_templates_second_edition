/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/23 19:53:57
# Desc   : chapter 05: 基本技巧
########################################################################
*/

#include <bitset>
#include <vector>

// 5.1 关键字 typename
// 只要依赖模板参数的名称是一种类型，就必须使用 typename
#include <print>

#include "cpp_utils/util.h"
template <typename T>
class MyClass {
public:
  void foo() {
    int ptr{31};
    {
      // 可能被当成 class T 的静态成员 SubType 与 ptr 的乘积
      T::Subtype* ptr;
    }
    {
      // 使用 typename 关键字来指明 SubType 是一个类型。
      // 定义一个指向 T::SubType 的指针变量 ptr。
      typename T::SubType* ptr;
    }
  }
};  // class MyClass

template <typename T>
void print_container(const T& container) {
  typename T::const_iterator it;
  typename T::const_iterator end = container.end();
  for (it = container.begin(); it != end; ++it) {
    std::print("{} ", *it);
  }
  std::println();
}
void run_print_container() {
  std::vector<int> vec{1, 2, 3, 4, 5};
  print_container(vec);
}

// 5.2 零初始化
template <typename T>
void foo() {
  int z;  // z 是未确定值
  T x;    // 如果 T 是内置类型，x 是未确定值
  T y{};  // 值初始化，要么调用自身的构造函数(默认构造函数，或 initializer-list 构造函数)，要么用 0
          // 初始化
  std::println(" 0 == x: {}", 0 == x);
  std::println(" 0 == y: {}", 0 == y);
}
void run_zero_initialization() {
  PRINT_CURRENT_FUNCTION_NAME;
  foo<int>();
  foo<double>();
}

// 5.3 使用 this->
template <typename T>
class Base {
public:
  void bar() { PRINT_CURRENT_FUNCTION_NAME; }
};  // class Base
void bar() {
  PRINT_CURRENT_FUNCTION_NAME;
}
// 对于基类依赖模板参数的模板类(class Derived : Base<T>), x 并不总是等同于 this->x
template <typename T>
class Derived : public Base<T> {
public:
  void foo() {
    bar();           // 它会调用一个外部的 bar(), 或者编译错误
    this->bar();     // 调用自身(或继承而来)的 bar()
    Base<T>::bar();  // 调用继承而来的 bar()
  }
};  // class Derived
void run_this() {
  Derived<int> d;
  d.foo();
}

// 5.4 处理原始数组和字符串字面量的模板
template <typename T, int N, int M>
bool less(T (&a)[N], T (&b)[M]) {
  PRINT_CURRENT_FUNCTION_NAME;
  for (int i = 0; i < N && i < M; ++i) {
    if (a[i] < b[i]) {
      return true;
    }
    if (b[i] < a[i]) {
      return false;
    }
  }
  return N < M;
}
void run_less() {
  int x[] = {1, 2, 3};
  int y[] = {1, 2, 3, 4, 5};
  std::println("less(x, y): {}", less(x, y));                    // less<int, 3, 5>
  std::println("less(\"ab\", \"abc\"): {}", less("ab", "abc"));  // less<const char, 3, 5>
}
// 主模板
template <typename T>
struct MyClassV2;  // class MyClassV2

// 已知边界数组的偏特化
template <typename T, std::size_t SZ>
struct MyClassV2<T[SZ]> {
  static void print() { PRINT_CURRENT_FUNCTION_NAME; }
};  // struct MyClassV2<T[SZ]>

// 已知边界数组引用的偏特化
template <typename T, std::size_t SZ>
struct MyClassV2<T(&)[SZ]> {
  static void print() { PRINT_CURRENT_FUNCTION_NAME; }
};  // struct MyClassV2<T(&)[SZ]>

// 未知边界数组的偏特化
template <typename T>
struct MyClassV2<T[]> {
  static void print() { PRINT_CURRENT_FUNCTION_NAME; }
};  // struct MyClassV2<T[]>

// 未知边界数组引用的偏特化
template <typename T>
struct MyClassV2<T(&)[]> {
  static void print() { PRINT_CURRENT_FUNCTION_NAME; }
};  // struct MyClassV2<T[]>

template <typename T>
struct MyClassV2<T*> {
  static void print() { PRINT_CURRENT_FUNCTION_NAME; }
};  // struct MyClassV2<T*>

template <typename... Ts>
void call_my_class_v2(Ts&&...) {
  (MyClassV2<Ts>::print(), ...);
//  (MyClassV2<std::conditional_t<
//    std::is_lvalue_reference_v<decltype(ts)>,  // 如果是左值引用
//    decltype(ts),                             // 保留原始类型
//    std::remove_reference_t<decltype(ts)>     // 否则移除引用
//>>::print(), ...);
//  (std::println("{}", typeid(Ts).name()), ...);
}

template <typename A1, typename A2, typename A3, typename X1, typename X2, typename X3>
void foo(
    int a1[7], int a2[],    // 根据语言规则，这是指针类型
    int (&a3)[42], int (&x0)[],  // 已知和未知边界数组的引用
    A1 aa1, A2& aa2, A3&& aa3,
    X1 x1,              // 传值会产生类型退化
    X2& x2, X3&& x3) {  // 传引用不会产生类型退化
  PRINT_CURRENT_FUNCTION_NAME;
  // a1 = int*, a2 = int*, a3 = int(&)[42], x0 = int(&)[]
  // aa1 = int*,  aa2 = int(&)[42], aa3 = int(&)[42]
  // x1 = int*, x2 = int(&)[], x3 = int(&)[]

  #define FORWARD(arg) std::forward<decltype(arg)>(arg)
  call_my_class_v2(
    FORWARD(a1),
    FORWARD(a2),
    FORWARD(a3),
    FORWARD(x0),
    FORWARD(aa1),
    FORWARD(aa2),
    FORWARD(aa3),
    FORWARD(x1),
    FORWARD(x2),
    FORWARD(x3));
}
void run_foo() {
  PRINT_CURRENT_FUNCTION_NAME;
  int a[42];
  MyClassV2<decltype(a)>::print();  // {T[SZ], int[42]}
  extern int x[];                   // 前置声明数组
  MyClassV2<decltype(x)>::print();  // {T[], int[]}
  // 最后六个参数 (a, a, a, x, x, x), a 是已知边界的数组 int[42]，x 是未知边界的数组 int[]
  // template <A1, A2, A3, X4, X5, X6>
  // (A1 aa1, A2& aa2, A3&& aa3, X4 x1, X5& x2, X6&& x3)
  //        |
  //        |
  //        V
  // aa1, x1 为传值类型，数组退化为指针, 即 int*，
  // 所以 A1 = int*, X1 = int*
  //    aa1 = int*, x1 = int*
  // aa2, aa3, x2, x3 是引用类型, 传参不会发生退化，即原数组类型 int[42] 和 int[]
  // 所以 A2 = A3 = int[42], X2 = X3 = int[]
  //    aa2 = aa3 = int(&)[42], x2 = x3 = int(&)[]
  //        |
  //        |
  //        V
  // A1 = int*, A2 = int[42], A3 = int[42],
  // X1 = int*, X2 = int[], X3 = int[]
  // aa1 = int*,  aa2 = int(&)[42], aa3 = int(&)[42]
  // x1 = int*, x2 = int(&)[], x3 = int(&)[]
  foo(a, a, a, x, a, a, a, x, x, x);
}
int x[] = {0, 8, 15};

// 5.5 成员模板
// 默认赋值操作符要求赋值操作符的两端具有相同的类型, 比如无法将 Stack<int> 赋值给 Stack<float>
// 所以我们需要将赋值操作符定义为模板函数, 以解决这个问题
template <typename T>
class Stack {
private:
  std::deque<T> elems;
public:
  template <typename> friend class Stack;
  template <typename U>
  Stack& operator=(const Stack<U>& other) {
    elems.clear();
    elems.insert(elems.begin(), other.elems.begin(), other.elems.end());
    // Stack<U> tmp(other);
    // elems.clear();
    // while (!tmp.empty()) {
    //   elems.push_front(tmp.top());
    //   tmp.pop();
    // }
    return *this;
  }
};  // class Stack
// 增加模板参数来改变内部容器类型
template <typename T, typename C = std::deque<T>>
class Stack2 {
private:
  C elems;
  template <typename, typename> friend class Stack2;
public:
  // 只有那些被调用的成员函数才会实例化
  // 也就是说，如果 operator= 没有被调用，那么就不会实例化
  // 当我们将 C 设定为 std::vector<int>，并且没有使用 operator= 时，即使 operator= 中通过
  // push_front 的方法来实现，也不会出现编译错误
  template <typename T2, typename C2>
  Stack2& operator=(const Stack2<T2, C2>& other) {
    elems.clear();
    elems.insert(elems.begin(), other.elems.begin(), other.elems.end());
    return *this;
  }
};  // class Stack2


// 成员函数模板可以全特化，但不能偏特化
class BoolString {
private:
  std::string value;
public:
  BoolString(const std::string& s) : value(s) {}
  template <typename T = std::string>
  T get() const { return value; }
  template <>
  inline bool get<bool>() const {
    return value == "true" || value == "1" || value == "on";
  }
};  // class BoolString
void run_bool_string() {
  PRINT_CURRENT_FUNCTION_NAME;
  BoolString s1("hello");
  std::println("s1.get(): {}", s1.get());  // hello
  std::println("s1.get<bool>(): {}", s1.get<bool>());  // false

  BoolString s2("on");
  std::println("s2.get(): {}", s2.get());  // on
  std::println("s2.get<bool>(): {}", s2.get<bool>());  // true
}

// 5.5.1 .template 构造
// 在调用成员模板是，如果需要显示指定模板参数，就使用 template 关键字
// 否则，编译器会不知道 < 是模板参数列表的开头
// 注意：只有当 .template 前的构造依赖于模板参数时，才有这种问题
template <unsigned long N>
void print_bit_set(const std::bitset<N>& bs) {
  // 编译错误
  // bs 依赖于模板参数，编译器不知道 to_string< 中的 < 是模板参数列表的开头
  // std::println("bitset<{}>: {}", N, bs.to_string<char, std::char_traits<char>, std::allocator<char>>());
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("bitset<{}>: {}", N, bs.template to_string<char, std::char_traits<char>, std::allocator<char>>());
}
template <typename T>
struct AA {
  template <typename U>
  void print() const {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct AA
struct BB {
  template <typename U>
  void print() const {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct BB
template <typename T>
void print_aa_and_bb(const AA<T>& a, const BB& b) {
  // a 的构造和模板参数有关，必须使用 .template
  // a.print<int>();  // no
  a.template print<int>();  // yes

  // b 的构造于模板参数无关
  b.print<int>();  // yes
  b.template print<int>();  // yes
}
void run_member_template() {
  AA<float> a;
  BB b;
  print_aa_and_bb(a, b);

  std::bitset<8> bs{0b00001111};
  print_bit_set(bs);
}

// 5.5.2 泛型 Lambda 和成员模板
static auto add = [](auto x, auto y) {
  PRINT_CURRENT_FUNCTION_NAME;
  return x + y;
};

void run_lambda() {
  PRINT_CURRENT_FUNCTION_NAME;
  add(1, 2);  // int int
  add(1, 2.0);  // int double
}

// 5.6 变量模板
template <typename T = long double>
constexpr T pi{3.141592653589793238};
void run_variable_template() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("pi<double>: {}", pi<double>);
  std::println("pi<float>: {}", pi<float>);
  std::println("pi<long double>: {}", pi<>);
  // 必须使用 <>
  // std::println("pi<long double>: {}", pi);
}
// 变量模板也可以用非类型参数进行参数化
template <int N>
std::array<int, N> my_arr{};
template <auto N>
constexpr decltype(N) dval{N};

void run_nontype_variable_template() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("dval<'c'>: {}", dval<'c'>);
  my_arr<10>[0] = 42;
  std::println("my_arr<10>[0]: {}", my_arr<10>[0]);  // 42
  my_arr<10>[0] = 41;
  std::println("my_arr<10>[0]: {}", my_arr<10>[0]);  // 41
  my_arr<11>[0] = 42;
  std::println("my_arr<11>[0]: {}", my_arr<11>[0]);  // 42
  std::println("my_arr<10>[0]: {}", my_arr<10>[0]);  // 41
  std::println("my_arr<10>: {}", my_arr<10>);
  std::println("my_arr<11>: {}", my_arr<11>);
}

// 5.7 双重模板参数
// template <typename T, template <typename Elem> typename Cont = std::deque>
// 省略 Elem, 因为没有使用
// template <typename T, template <typename> typename Cont = std::deque>
// c++17 前 Cont 和 std::deque 的模板参数个数必须相同
template <typename T, template <typename Elem, typename = std::allocator<Elem>> typename Cont = std::deque>
class Stack3 {
private:
  Cont<T> elems;  // elements
public:
  void push(const T& elem);
  void pop();
  const T& top() const;
  bool empty() const { return elems.empty(); }
  template <typename T2, template <typename Elem2, typename = std::allocator<Elem2>> typename Cont2>
  Stack3<T, Cont>& operator=(const Stack3<T2, Cont2>& other);
  // 友元
  template <typename, template <typename, typename> class>
  friend class Stack3;
};  // class Stack3

template<typename T, template<typename, typename> class Cont>
void Stack3<T, Cont>::pop() {
  assert(!elems.empty());
  elems.pop_back();
}

int main() {
  run_print_container();
  run_this();
  run_less();
  run_foo();
  run_bool_string();
  run_member_template();
  run_lambda();
  run_variable_template();
  run_nontype_variable_template();
  return 0;
}
