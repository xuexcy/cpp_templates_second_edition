/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/06/08 14:14:44
# Desc   : chapter_15: 模板实参推导
########################################################################
template argument deduction
*/

#include <cassert>
#include <type_traits>
#include "cpp_utils/util.h"

// 15.1 推导的过程
template <typename T>
T max(T a, T b) {
  return b < a ? a : b;
}
template <typename T>
typename T::ElementT at(T a, int i) {
  return a[i];
}

// 参数声明为非引用时，数组和函数类型会退化为指针类型, 并忽略 const, volatile 限定符
template <typename T> void f(T ts) {
  PRINT_CURRENT_FUNCTION_NAME;
}
// 参数声明为引用时，不会退化和忽略限定符
template <typename T> void g(T&) {
  PRINT_CURRENT_FUNCTION_NAME;
}

void run_deduction_process() {
  PRINT_CURRENT_FUNCTION_NAME;
  {
    // 推导失败, 1 推导出 T = int, 1.0 推导出 T = double, 产生冲突
    // auto g = max(1, 1.0);
  }
  {
    int a[7] = {1, 2, 3, 4 , 5 , 6, 7};
    // 推导失败，a 推导出 T = int*, 但 int* 没有 ElementT 成员类型
    // auto x = at(a, 3);
  }
  {
    double arr[20];
    const int one = 1;
    f(arr);  // T = double*, 数组退化为指针
    g(arr);  // T = double[20]

    f(one);  // T = int
    g(one);  // T = const int

    f(1);  // T = int
    // g(1);  // 编译失败，T = int, 不能把 1 传递给 int&
  }
  std::println();
}

// 15.2 推导的上下文
// 15.3 特殊的推导情况
// 15.4 初始化列表
// 15.5 参数包
template <typename T, typename U>
class pair {
};  // class pair

template <typename T, typename... Rest>
void h1(const pair<T, Rest>&...) {}
template <typename... Ts, typename... Rest>
void h2(const pair<Ts, Rest>&...) {}

void run_parameter_pack() {
  PRINT_CURRENT_FUNCTION_NAME;
  pair<int, float> pif;
  pair<int, double> pid;
  pair<double, double> pdd;
  h1(pif, pid);  // T = int, Rest = [float, double]
  h2(pif, pid);  // Ts = [int, int], Rest = [float, double]
  // h1(pif, pdd);  // pif -> T = int      pdd -> T = double  矛盾
  h2(pif, pdd);  // Ts = [int, double], Rest = [float, double]
  std::println();
}

// 15.5.1 字面量运算符模板
// cs... 必须是一个合法的数字
template <char... cs>
int operator ""_B7() {
  std::array<char, sizeof...(cs)> chars{cs...};
  for (auto c : chars) {  std::print("'{}' ", c); }
  std::println();
  return sizeof...(cs);
}
void run_character_literal() {
  PRINT_CURRENT_FUNCTION_NAME;
  int a = 121_B7;  // a = 3
  assert(3 == a);
  int b = 0xFF00_B7;  // b = 6
  assert(6 == b);
  // int c = 0815_B7;  // 0815 不是一个合法数字，因为8不是八进制字面量
  int d = 01.3_B7; // d = 4
  assert(4 == d);

  std::println();
}

// 15.6 右值引用
// 15.6.1 引用折叠规则
void run_reference_collapse() {
  {
    int i{};
    int& ref = i;  // yes
    // 不允许直接声明引用的引用
    // int&& ref2ref = ref;  // no
  }
  {
    using RI = int&;
    int i{};
    RI r = i;  // yes
    RI& rr = r;  // yes,  引用折叠，得到 int&
    SAME_TYPE(decltype(rr), int&);
  }
  {
    using RCI = const int&;
    volatile RCI&& r = 42;
    SAME_TYPE(decltype(r), const int&);
    using RRI = int&&;
    const RRI&& rr = 42;
    SAME_TYPE(decltype(rr), int&&);
  }
}

// 15.6.2 转发引用
template <typename T>
void forward(T&& p) {
  // 若 T 是一个引用类型，则 x 需要初始化
  // T x;
  std::remove_reference_t<T> x;
}
void run_forward_reference() {
  PRINT_CURRENT_FUNCTION_NAME;
  forward(1);
  int i;
  forward(i);  // T 推导为 int&, 如果直接使用 T x; 则 x 需要初始化
  std::println();;
}

// 15.6.3 完美转发
class C {
};  // class C
void c(C&) {
  PRINT_CURRENT_FUNCTION_NAME;
}
void c(const C&) {
  PRINT_CURRENT_FUNCTION_NAME;
}
void c(C&&) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void forward_to_c(T&& x) {
  PRINT_CURRENT_FUNCTION_NAME;
  c(static_cast<T&&>(x));
  // #include <utility>
  c(std::forward<T>(x));  // 等价于 c(static_cast<T&&>(x))
}

// fas
void run_perfect_forwarding() {
  PRINT_CURRENT_FUNCTION_NAME;
  C v;
  const C c;
  forward_to_c(v);  // T = C&, 调用 c(C&)
  forward_to_c(c);  // T = const C&, 调用 c(const C&)
  forward_to_c(C());  // T = C, 调用 c(C&&)
  forward_to_c(std::move(v));  // T = C, 调用 c(C&&)
  std::println();
}

int main() {
  run_deduction_process();
  run_character_literal();
  run_reference_collapse();
  run_forward_reference();
  run_perfect_forwarding();
  return 0;
}



