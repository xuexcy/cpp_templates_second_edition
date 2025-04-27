/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/04/26 15:00:18
# Desc   : chapter_07: 传值还是传引用
########################################################################
*/

#include <functional>
#include <iostream>
#include <print>


#include "cpp_utils/util.h"
// 7.1 传值
template <typename T>
void print_v(T arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("arg = {}", arg);
}

void run_print_v() {
  PRINT_CURRENT_FUNCTION_NAME;
  print_v(1);  // T = int
  std::string s = "hello";
  // 在传值时，这里使用了 std::string 的拷贝构造函数
  // std::string 类的实现本身可能有一些使拷贝成本更低的优化
  //  1. 小字符串优化(small string optimization, SSO): 将小字符串直接保存在对象的内部，而不必分配内存
  //  2. 写实拷贝优化(copy on write, COW): 只有在需要修改字符串时才会进行拷贝
  // COW 在多线程中有明显的问题，c++11 开始就禁用了标准字符串的写时拷贝优化
  print_v(s);  // T = std::string

  // 按值传递类型衰变
  //  1. 数组类型会衰变为指针类型
  //  2. 删除 const 和 volatile 限定符
  const std::string c{"hello"};
  print_v(s);  // T = std::string
  print_v("hi");  // T = const char*,  const char[3] 衰变为 const char*
  int arr[4] = {1, 2, 3, 4};
  // print_v(arr);  // T = int*
  std::println();
}

// 7.2 按引用传递
// 传递引用时，参数类型不会衰变
template <typename T>
void print_r(const T& arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("arg = {}", arg);
}

void run_print_r() {
  print_r("hi");  // T = char[3], arg = const char(&)[3]
  int arr[4] = {1, 2, 3, 4};
  print_r(arr);  // T = int[4], arg = const int(&)[4]
}

template <typename T>
void out_r(T& arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("arg = {}", arg);
}

template <typename T, typename = std::enable_if_t<!std::is_const_v<T>>>
//template <typename T>
//requires (!std::is_const_v<T>)
void modify_r(T& arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  static_assert(!std::is_const_v<T>, "out parameter T is const");
  std::println("arg = {}", arg);
}
void run_out_r() {
  const std::string c{"hi"};
  out_r(c);  // T = const std::string, 此时无法在 out_r 中修改 c
  // 通过 std::enable_if_t 或 requires 或 static_assert 来禁用该函数接受 const 参数
  // modify_r(c);  // T = const std::string, 此时无法在 out_r 中修改 c
}

// 7.2.3 通过转发引用进行传递
template <typename T>
void pass_r(T&& arg) {
  PRINT_CURRENT_FUNCTION_NAME;
}

template <typename T>
void pass_r_2(T&& arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  T x;  // 当参数为左值是，x 就是一个引用，此时 x 必须要初始化
}
void run_pass_r() {
  std::string s{"hi"};
  pass_r(s);  // T = std::string&, arg = std::string&
  pass_r(std::string("hi"));  // T = std::string, arg = std::string&&
  pass_r(std::move(s));  // T = std::string, arg = std::string&&
  int arr[4] = {1, 2, 3, 4};
  pass_r(arr);  // T = int(&)[4], arg = int(&)[4]
  const std::string c{"he"};
  pass_r(c);  // T = const std::string&, arg = const std::string&
  pass_r("hi");  // T = const char(&)[3], arg = const char(&)[3]

  pass_r_2(42);  // T = int, arg = int&&
  int i{42};
  // 此时 T = int&, 函数内的 x 是一个引用，必须初始化
  // 所以下面代码会导致编译失败
  // pass_r_2(i);  // T = int&, arg = int&
}

// 7.3 使用 std::ref() 和 std::cref()
void print_s(const std::string& arg) {
 std::cout << arg << std::endl;
}
template <typename T>
void print_t(T arg) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 包装器会隐式转换成原始对象
  print_s(arg);

  // 由于没有为 std::reference_wrapper 定义 operator<<，所以不能直接打印
  // 但是通过 print_s 告诉编译器将 std::reference_wrapper 隐式转换为 std::string 是可以的
  // std::cout << arg << std::endl;
}

void run_print_t() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string s{"heh"};
  print_t(s);  // pass by value, T = int, arg = int
  // std::ref: 创建一个 std::reference_wrapper<> 类型，引用原始参数，并按值传递这个对象
  // 看起来像是传递了一个引用
  // 另外，包装起还支持返回原始类型的隐式转换，生成原始对象的引用
  //  operator type&() const;
  //  type& get() const;
  print_t(std::ref(s));  // T = std::reference_wrapper<int>, arg = std::reference_wrapper<int>
  print_t(std::cref(s));  // T = std::reference_wrapper<const int>, arg = std::reference_wrapper<const int>

  // 编译器必须知道返回类型必要的隐式转换
  // 只有通过泛型代码将对象传递给非泛型代码时，std::ref 和 std::cref 才能正常工作
  // 以 print_t 为例，在它的实现中调用 print_s，明确告诉编译器将 arg 转换为 std::string，
  // 此时 std::ref 和 std::cref 才能正常工作
}

// 7.4 处理字符串字面值和数组
template <typename T>
void foo(const T& arg1, const T& arg2) {}
template <typename T>
void foo2(T arg1, T arg2) {
  if (arg1 == arg2) {}
}

// 仅对数组有效
template <typename T, std::size_t L1, std::size_t L2>
void foo3(T (&arg1)[L1], T (&arg2)[L2]) {}

// 通过 std::enable_if_t<std::is_array_v<T>> 来确定参数是不是数组
template <typename T, typename = std::enable_if_t<std::is_array_v<T>>>
void foo4(T&& arg1, T&& arg2) {}
void run_foo() {
  PRINT_CURRENT_FUNCTION_NAME;
  // 类型不退化，编译错误: 传引用时类型不会退化，一个是 char[3], 一个是 char[4]
  // foo("hi", "guy");

  // 类型退化，运行时错误: T = char*
  // 本意是比较两个字符串，但实际上比较的是两个指针
  foo2("hi", "guy");
}

// 7.5 处理返回值
// 当函数返回引用类型时，引用的原始对象可能被销毁
void reference_and_then_delete() {
  {
    std::string* s = new std::string("what");
    auto& c = (*s)[0];
    // 这里比较明显的 delete
    delete s;
    // std::println("c = {}", c);  // 运行时错误
  }
  {
    auto s = std::make_unique<std::string>("what");
    auto& c = (*s)[0];
    // 这里不太明显的 delete 了
    s.reset();
    // std::println("c = {}", c);  // 运行时错误
  }
}
// 模板函数的参数类型 T 可能就是引用类型，如果我们直接返回该参数类型，那就可能导致问题
template <typename T>
T ret_r(T&& p) {  // r: by reference
  PRINT_CURRENT_FUNCTION_NAME;
  return T{};
}
void run_ret_r() {
  PRINT_CURRENT_FUNCTION_NAME;
  auto ret_1 = ret_r(42);  // T = int, p = int&&
  std::println("ret_1 = {}", ret_1);  // ret_1 = 0
  int i = 42;

  // 编译错误
  // auto ret_2 = ret_r(i);  // T = int&, p = int&
}
// 即使使用传值调用推导模板参数，但还是可以显示将 T 指定为引用类型，这样也可能导致错误
template <typename T>
T ret_v(T p) {  // v: by value
  PRINT_CURRENT_FUNCTION_NAME;
  return T{};  // return by reference if T is reference
}
void run_ret_v() {
  PRINT_CURRENT_FUNCTION_NAME;
  auto ret_1 = ret_v(42);  // T = int, p = int
  std::println("ret_1 = {}", ret_1);  // ret_1 = 0
  int i = 42;

  auto ret_2 = ret_v(i);  // T = int, p = int

  // 编译错误
  // auto ret_3 = ret_v<int&>(i);  // T = int&, p = int&
}
// 使用 std::remove_reference<> 去掉引用
template <typename T>
std::remove_reference_t<T> ret_v_remove_r(T p) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 原文中写的是 T{}, 但这样好像无法编译，还是要用 std::remove_reference_t<T>{}
  // return T{};
  return std::remove_reference_t<T>{};  // return by value
}
// 使用 auto 进行衰变
template <typename T>
auto ret_v_auto(T p) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 这个应该也是不行的，如果 T 是引用类型的话
  return T{};  // return by value
}
void run_remove_reference() {
  PRINT_CURRENT_FUNCTION_NAME;
  int i = 42;
  // T = int&, p = int&
  auto ret_1 = ret_v_remove_r<int&>(i);  // T = int&, p = int&
  std::println("ret_1 = {}", ret_1);  // ret_1 = 0
  static_assert(std::is_same_v<decltype(ret_1), int>);  // T = int

  // auto ret_2 = ret_v_auto<int&>(i);  // T = int&, p = int&
}

int main() {
  run_print_v();
  run_print_r();
  run_out_r();
  run_pass_r();
  run_print_t();
  run_foo();
  run_ret_r();
  run_ret_v();
  return 0;
}


