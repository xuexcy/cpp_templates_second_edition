/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/04 20:52:19
# Desc   :
########################################################################
*/

#include <print>

#include "cpp_utils/util.h"

// 1.1 函数模板初探
// 1.1.1 定义模板
template <typename T>
T max(T a, T b) {
  PRINT_CURRENT_FUNCTION_NAME;
  return a > b ? a : b;
}
// 1.1.2 使用模板
void run_max() {
  PRINT_CURRENT_FUNCTION_NAME;
  int i = 42;
  std::println("max(7, i): {}", ::max(7, i));
  double f1{3.4};
  double f2{-6.7};
  std::println("max(f1, f2): {}", ::max(f1, f2));
  std::string s1{"mathematics"};
  std::string s2{"math"};
  std::println("max(s1, s2): {}", ::max(s1, s2));
}

// 1.1.3 两阶段编译
/*
第一阶段: 忽略模板参数，检查代码正确性
第二阶段: 实例化后，检查所有代码有效性（当前阶段才会检查依赖模板参数的部分）
*/
template <typename T>
void foo(T t) {
  // undeclared();  // 编译期第一阶段报错
  // undeclared(t);  // 编译期第二阶段报错
  // static_assert(sizeof(int) > 10, "int too small");
  // static_assert(sizeof(T) > 10, "T too small");  // 第二阶段报错
}

// 1.2 模板实参推导简介
// 1.2.1 类型推导中的类型转换
/*
规则 1: 声明调用参数按引用传递，实参必须完全匹配
规则 2: 声明调用参数按值传递，支持退化的简单转换:
  a: 忽略 const 和 volatile
  b: 引用转换为引用类型
  c: 数组或函数转换为指针类型
*/
template <typename T>
void foo_by_reference(const T& t1, const T& t2) {
  PRINT_CURRENT_FUNCTION_NAME;
}
void run_by_reference() {
  int i{0};
  int arr[4];
  // &i 为 int* 类型, arr 为 int[4] 类型
  // foo_by_reference(&i, arr);

}
template <typename T>
void foo_by_value(T t1, T t2) {
  PRINT_CURRENT_FUNCTION_NAME;
}
void run_by_value() {
  int i{0};
  const int c{42};
  foo_by_value(i, c);  // T = int
  foo_by_value(c, c);  // T = int
  int& ir = i;
  foo_by_value(i, ir);  // T = int
  int arr[4];
  foo_by_value(&i, arr);  // &i 为 int*, arr 推导为 int*   --> T = int*

  // max(4, 7.2);  // T = int or double
  foo_by_value(static_cast<double>(4), 7.2);  // 强制转换，T = double
  foo_by_value<double>(4, 7.2);  // 显示指定, T = double
}
// 1.2.2 默认实参的类型推导
// 不能使用默认的实参做类型推导
template <typename T>
void foo_with_default_argument(T t = "") {
  PRINT_CURRENT_FUNCTION_NAME;
}
void run_with_default_argument() {
  foo_with_default_argument(1);  // T = int
  // foo_with_default_argument();  // no, 不能通过默认实参 "" 推导出 T 为 std::string
}
template <typename T = std::string>
void foo_with_default_type(T t = "") {
  PRINT_CURRENT_FUNCTION_NAME;
}
void run_with_default_type() {
  foo_with_default_type(1);  // T = int
  foo_with_default_type();  // yes,通过默认模板实参类型 std::string 推导出 T = std::string
}
void run_deduce() {
  run_by_reference();
  run_by_value();
  run_with_default_argument();
  run_with_default_type();
}

// 1.3.2 推导返回类型
// 通过 std::decay 获取退化后的类型，避免 T1，T2 为引用类型
template <typename T1, typename T2>
auto max(T1 t1, T2 t2) -> std::decay<decltype(true ? t1 : t2)>::type {
  PRINT_CURRENT_FUNCTION_NAME;
  return t1 > t2 ? t1 : t2;
}
// 1.3.2 返回类型为公共类型
// std::common_type_t
template <typename T1, typename T2>
std::common_type_t<T1, T2> max_common_type(T1 t1, T2 t2) {
  PRINT_CURRENT_FUNCTION_NAME;
  return t1 > t2 ? t1 : t2;
}

int main() {
  run_max();
  run_deduce();
  return 0;
}


