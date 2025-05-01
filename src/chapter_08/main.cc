/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/01 12:13:52
# Desc   : chapter_08: 编译期编程
########################################################################
*/

#include <print>
#include <type_traits>
#include "cpp_utils/util.h"


// 8.1 模板元编程
template <unsigned p, unsigned d>
struct DoIsPrime {
  static constexpr bool value = (p % d != 0) && DoIsPrime<p, d - 1>::value;
};  // struct DoIsPrime

template <unsigned p>
struct DoIsPrime<p, 2> {
  static constexpr bool value = (p % 2 != 0);
};  // struct DoIsPrime<p, 2>

template <unsigned p>
struct IsPrime {
  static constexpr bool value = DoIsPrime<p, p/2>::value;
};  // struct IsPrime
template <>
struct IsPrime<0> {
  static constexpr bool value = false;
};  // struct IsPrime<0>
template <>
struct IsPrime<1> {
  static constexpr bool value = false;
};  // struct IsPrime<1>
template <>
struct IsPrime<2> {
  static constexpr bool value = false;
};  // struct IsPrime<2>
template <>
struct IsPrime<3> {
  static constexpr bool value = false;
};  // struct IsPrime<3>


// 8.2 使用 constexpr 计算
// c++ 11 中每个 constexpr 函数定义基本限定只能包含一条 return 语句
constexpr bool is_prime_cpp11(unsigned p, unsigned d) {
  return d !=2 ? ( p % d != 0 && is_prime_cpp11(p, d - 1)) : (p % 2 != 0);
}
constexpr bool is_prime_cpp11(unsigned p) {
  return p < 4 ? !(p < 2) : is_prime_cpp11(p, p / 2);
}
// c++ 14 中可以用大多数的控制结构
constexpr bool is_prime_cpp14(unsigned p) {
  for (unsigned int d = 2; d <= p / 2; ++d) {
    if (p % d == 0) {
      return false;
    }
  }
  return p > 1;
}

void run_is_prime() {
  PRINT_CURRENT_FUNCTION_NAME;
  static_assert(!IsPrime<9>::value, "9 is not a prime number");
  static_assert(IsPrime<11>::value, "11 is a prime number");
  static_assert(!is_prime_cpp11(9), "9 is not a prime number");
  static_assert(is_prime_cpp11(11), "11 is a prime number");
  static_assert(!is_prime_cpp14(9), "9 is not a prime number");
  static_assert(is_prime_cpp14(11), "11 is a prime number");

  constexpr bool b1 = is_prime_cpp14(11);  // 在编译期求值
  // 如果在全局或命名空间作用域，则在编译期求值
  // 如果在块作用域，编译器决定在编译期还是运行期求值,编译器只需要检查是否可以在编译期进行计算
  const bool b2 = is_prime_cpp14(9);

  std::println("is_prime_cpp14(17) = {}", is_prime_cpp14(17));  // 运行期求值
  std::println();
}
const bool b3 = is_prime_cpp14(9);  // 在编译期求值

// 8.3 偏特化的执行路径选择
// 主模板
template <int SZ, bool = is_prime_cpp14(SZ)>
struct Helper;
// 如果 SZ 不是质数时的实现
template <int SZ>
struct Helper<SZ, false> {
  static constexpr bool value = false;
  static void print() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("hello, this is not a prime number: {}", SZ);
    std::println("yyds");
  }
};  // struct Helper<SZ, false>
// 如果 SZ 是质数时的实现
template <int SZ>
struct Helper<SZ, true> {
  static constexpr bool value = true;
  static void print() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("hello, this is a prime number: {}", SZ);
    std::println("6b");
  }
};  // struct Helper<SZ, true>

template <typename T, std::size_t SZ>
long foo(const std::array<T, SZ>& coll) {
  Helper<SZ> h;  // 取决于 coll 的大小是否是质数
  h.print();  // 选择不同的实现
  return h.value;
}

void run_template_specialization_selection() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::array<int, 5> coll1 = {1, 2, 3, 4, 5};
  std::array<int, 6> coll2 = {1, 2, 3, 4, 5, 6};
  foo(coll1);  // hello, this is a prime number: 5
  foo(coll2);  // hello, this is not a prime number: 6
  std::println();
}

// 另外，可以将主模板用于其中一个可选(默认)情况，用偏特化实现其他特殊情况
template <int SZ, bool IsPrime = is_prime_cpp14(SZ)>
struct Helper2 {
  static constexpr bool value = IsPrime;
  static void print() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("hello, this is a prime number: {}", SZ);
    std::println("6b");
  }
};  // struct Helper2
template <int SZ>
struct Helper2<SZ, false> {
  static constexpr bool value = false;
  static void print() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("hello, this is not a prime number: {}", SZ);
    std::println("yyds");
  }
};  // struct Helper2
template <typename T, std::size_t SZ>
long foo2(const std::array<T, SZ>& coll) {
  Helper2<SZ> h;  // 取决于 coll 的大小是否是质数
  h.print();  // 选择不同的实现
  return h.value;
}
void run_template_specialization_selection_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::array<int, 5> coll1 = {1, 2, 3, 4, 5};
  std::array<int, 6> coll2 = {1, 2, 3, 4, 5, 6};
  foo2(coll1);  // hello, this is a prime number: 5
  foo2(coll2);  // hello, this is not a prime number: 6
  std::println();
}

// 函数模板不支持偏特化, 解决办法
// 1. 带有静态函数的类: 比如上面的 Helper::print，其实就是通过类中包含静态函数，利用类的偏特化来实现函数的偏特化
// 2. 使用 std::enable_if<>
template <typename T>
std::enable_if_t<std::is_integral_v<T>> print(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("{} is an integral type", t);
}
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>> print(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("{} is a floating type", t);
}
// 3. SFINAE
// 4. c++17 的编译期 if
template <typename T>
void print_with_if(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  if constexpr (std::is_integral_v<T>) {
    std::println("{} is an integral type", t);
  } else if constexpr (std::is_floating_point_v<T>) {
    std::println("{} is a floating type", t);
  }
}
void run_function_specialization() {
  PRINT_CURRENT_FUNCTION_NAME;
  print(42);  // 42 is an integral type
  print(3.14);  // 3.14 is a floating type

  print_with_if(42);
  print_with_if(3.14);
  std::println();
}


// 8.4 SFINAE
// SFINAE: Substitution Failure Is Not An Error
template <typename T, unsigned N>
std::size_t len(T(&)[N]) {  // 一个长度为 N 的 T 类型元素数组
  PRINT_CURRENT_FUNCTION_NAME;
  return N;
}
template <typename T>
typename T::size_type len(const T& t) {  // 要求传入的实参类型具有一个相应的 size_type 成员类型
  PRINT_CURRENT_FUNCTION_NAME;
  return t.size();
}

void run_SFINAE() {
  PRINT_CURRENT_FUNCTION_NAME;
  int arr[5] = {1, 2, 3, 4, 5};
  std::string s = "hello";
  std::println("len(arr) = {}", len(arr));  // len(arr) = 5
  std::println("len(s) = {}", len(s));      // len(s) = 5
  std::println();
  int* p = arr;
  // 编译器找不到 len 函数: 类型是 int*, 没有 size_type 成员类型
  // std::println("len(p) = {}", len(p));

  // 编译器找到 len 函数(第 2 个), 但没有 size() 成员函数, 编译期错误
  // No member named 'size' in 'std::allocator<int>'
  std::allocator<int> x;
  // std::println("len(x) = {}", len(x));  // len(x) = 0
}

// 当替换某个候选函数的返回类型没有意义时忽略该函数，会导致编译器选择另一个参数类型匹配度较低的候选函数
size_t len(...) {
  // 通过，后备函数会提供更有用的默认值、抛出异常或包含静态断言以产生有用的错误信息
  return 0;
}
void run_SFINAE_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  int arr[5] = {1, 2, 3, 4, 5};

  // 前两个 len 都不匹配，匹配到最后一个
  int* p = arr;
  std::println("len(p) = {}", len(p));

  // 第 2 个 len 匹配，不会选择最后一个，由于没有 size() 成员函数，还是会发生编译期错误
  std::allocator<int> x;
  // std::println("len(x) = {}", len(x));  // len(x) = 0
  std::println();
}

// SFINAE 和重载解析
class thread {
public:
  // // 如果 decay_t<F> 与 thread 类型相同，则此构造函数不应参与重载解析
  // template <typename F, typename... Args>
  // explicit thread(F&& f, Args&&... args);

  // 通过 SFINAE 生成无效代码，从而确保忽略这些约束条件下的函数模板，也就是"SFINAE 掉了一个函数模板"
  // 当 std::decay_t<F> 是 thread 时，可以调用 thread 的预定义拷贝或移动构造函数，但这个函数模板可能更匹配，所以我们要禁止此种情况
  // 这样可以确保当使用 thread 对象来构造另一个 thread 对象时，始终使用预定义的拷贝或移动构造函数
  template <typename F, typename... Args, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, thread>>>
  explicit thread(F&& f, Args&&... args) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  thread(const thread&) {  // copy constructor
    PRINT_CURRENT_FUNCTION_NAME;
  }
  thread(thread&&) {  // copy constructor
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class thread
void f() {}
void run_thread() {
  PRINT_CURRENT_FUNCTION_NAME;
  thread t1(f);
  thread t2(t1);  // 调用拷贝构造函数
  thread t3(std::move(t1));  // 调用移动构造函数
  std::println();
}

// 8.4.1 通过 decltype 来 SFINAE 掉表达式
/*
这个模板函数只对 T 是否有 size_type 进行了检测，没有对是否有 size() 函数进行检测
如果调用匹配上这个函数模板，而类型 T 没有 size() 函数，则会导致编译期错误

template <typename T>
typename T::size_type len(const T& t) {
  PRINT_CURRENT_FUNCTION_NAME;
  return t.size();
}

解决办法:
1. 尾置返回类型
2. 使用 decltype 和逗号运算符来定义返回类型
3. 将必须成立的表达式放置于逗号前，并将表达式转换为 void 类型(防止逗号运算符重载)
*/
template <typename T>
auto len2(const T& t) -> decltype((void)(t.size()), T::size_type()) {
  PRINT_CURRENT_FUNCTION_NAME;
  return t.size();
}

// 8.5 编译期 if 简介
template <typename T, typename... Types>
void print(const T& first_arg, const Types&... args) {
  std::println("{}", first_arg);
  if constexpr (sizeof...(args) > 0) {
    print(args...);
  }
}

void run_print() {
  PRINT_CURRENT_FUNCTION_NAME;
  print(1, 2.0, "hello", std::string("world"));
  std::println();
}

// 代码没有实例化意味着只会执行第一阶段编译(定义期),即语法正确性检查和不依赖模板参数的名称检查
template <typename T>
void foo(T t) {
  std::println("argument: {}", t);
  if constexpr (std::is_integral_v<T>) {
    if (t > 0) {
      foo(t - 1);
    }
  } else {
    // 如未声明且未丢弃则报错:
    // undeclared(t) 使用了和模板参数相关的实参 T t
    // 如果没有实例化 else 的调用, 编译器不会报错
    undeclared(t);

    // 如未声明则报错(即使会丢弃): 就是说在满足 if constexpr 的情况下，编译器依然会报错
    // clang 提示：Use of undeclared identifier
    // undeclared();

    // 编译器会检查这行，只是在没有实例化时，会将断言结果丢弃
    static_assert(false, "no integral");  // 始终断言(即使会丢弃)
    static_assert(std::is_integral_v<T>, "no integral");  // 和模板参数相关，只有实例化才会检查
  }
}

void run_foo() {
  PRINT_CURRENT_FUNCTION_NAME;
  foo(1);
}

int main() {
  run_is_prime();
  run_template_specialization_selection();
  run_template_specialization_selection_2();
  run_function_specialization();
  run_SFINAE();
  run_SFINAE_2();
  run_thread();
  run_print();
  run_foo();
  return 0;
}



