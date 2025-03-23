/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/23 14:37:36
# Desc   : chapter 04: 变参模板
########################################################################
*/

#include <cassert>
#include <complex>
#include <print>
#include <thread>
#include <unordered_map>
#include <vector>

#include "add_space.h"
#include "cpp_utils/util.h"

#include "fold_traverse.h"
#include "var_using.h"

// 4.1 变参模板简介
// 4.1.1 变参模板示例
void print() {}
template <typename T, typename... Types>
void print(T firstArgs, Types... args) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("{}", firstArgs);
  print(args...);
}
void run_print() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string s("world");
  print(1, 2.5, "hello", s);  // int, double, const char*, std::string
}

// 4.1.2 sizeof... 运算符
// 通过 sizeof... 就可以不用实现一个 print() 来接受长度为 0 的 args...
template <typename T, typename... Types>
void print_sizeof(T firstArg, Types... args) {
  std::println("{}", sizeof...(Types));
  std::println("{}", sizeof...(args));
  std::println("firstArg {}", firstArg);

  // no:
  // if (sizeof...(args) > 0) {
  //   print_sizeof(args...);
  // }

  /** 在没有 constexpr 的情况下，通常函数模板中 if 的两个分支都会被实例化
  当 sizeof...(args) == 1 时, 实例化出来的 else 分支会报错, 即:
  if (sizeof...(args) > 0) {
    print_sizeof(args...);  // args... = 1
  } else {
    print_sizeof();  // no, 没有实现 print_sizeof()
  }
  */

  // yes
  if constexpr (sizeof...(args) > 0) {
    print_sizeof(args...);
  }
}
void run_print_sizeof() {
  PRINT_CURRENT_FUNCTION_NAME;
  print_sizeof(1, 2.5, "hello", "world");  // sizeof...(Types) = 3, sizeof...(args) = 3
}


// 4.2 折叠表达式
// 对所有实例使用二元运算符
template <typename... T>
auto fold_sum(T... t) {
  // (... op pack)   -> (((pack1 op pack2) op pack3) ... op packN)
  auto sum1 = (... + t);
  // (pack op ...)   -> (pack1 op (... (packN-1 op packN)))
  auto sum2 = (t + ...);
  // (init op ... op pack)   -> ((((init op pack1) op pack2) op pack3) ... op packN)
  auto sum3 = (1 + ... + t);
  // (pack op ... init)   -> (pack1 op (... (packN-1 op (packN op init))))
  auto sum4 = (t + ... + 4);
  std::println("sum1: {} , sum2: {} , sum3: {} , sum4: {}", sum1, sum2, sum3, sum4);
  return (... + t);
}
void run_fold_sum() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("fold_sum(1, 2, 3): {}", fold_sum(1, 2, 3));
}

void run_fold_traverse() {
  Node* root = new Node{0};
  root->left = new Node{1};
  root->left->right = new Node{2};
  Node* node = traverse(root, &Node::left, &Node::right);
  std::println("root->left->right value: {}", node->value);

  assert(2 == node->value);

  delete root->left->right;
  delete root->left;
  delete root;
}

void run_fold_print() {
  fold_print(1, 2, 'a', "b", 3.1);
}

// 4.3 变参模板应用
// 通常，实参是通过移动与“完美转发”的
// make_shared 函数
template <typename T, typename... Args>
std::shared_ptr<T> make_shared(Args&&... args);
// thread 构造函数
class MyThread {
public:
  template <typename F, typename... Args>
  MyThread(F&& f, Args&&... args);
};  // class MyThread
// vector emplace_back 函数
template <typename T, typename Allocator=std::allocator<T>>
class MyVector {
public:
  template <typename... Args>
  T& emplace_back(Args&&... args);
};  // class MyVector
// 变长模板参数的退化与普通参数相同
// 传值：拷贝实参并且类型会退化（数字变成指针）
// 传引用: 参数会引用原始实参并且类型不会退化
template <typename... Args>
void foo(Args... args);  // args 是类型退化后的副本(拷贝)
template <typename... Args>
void bar(const Args&... args);  // args 是传入对象未退化的引用
void run_variadic_template_application() {
  // 智能指针使用变长参数初始化
  auto sp = std::make_shared<std::complex<float>>(4.2f, 7.7f);

  auto foo = [](int i, int j, int k) {
    std::println("i: {}, j: {}, k: {}", i, j, k);
  };
  // 通过变长参数向线程传递实参
  std::thread t(foo, 1, 2, 3);
  t.join();

  struct Customer {
    std::string name;
    int age{-1};
    double height{-0.0};
  };  // struct Customer
  std::vector<Customer> v;
  // 传递实参给新元素压入 vector 容器的构造函数
  v.emplace_back("xuechengyun", 29, 1.75);
}

// 4.4 变参数模板和变参表达式
// 4.4.1 变参表达式
template <typename... T>
void print_double(const T&... args) {
  print((args + args)...);
}
template <typename... T>
void print_add_one(const T&... args) {
  // print(args + 1...);  // 错误: 1... 是小数点过多的字面量
  print(args + 1 ...);  // 加一个空格
  print((args + 1)...);
}
void run_print_with_calculation() {
  PRINT_CURRENT_FUNCTION_NAME;
  print_double(7.5, std::string("hello"), 1);
  print_add_one(7.5, -1, 1u);
}
// c++17
template <typename T1, typename... TN>
constexpr bool is_homogeneous(T1, TN...) {
  PRINT_CURRENT_FUNCTION_NAME;
  return (std::is_same_v<T1, TN> && ...);
}
void run_is_homogeneous() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("is_homogeneous(1, 2, 3): {}", is_homogeneous(1, 2, 3));  // true
  std::println("is_homogeneous(1, 2u, 3): {}", is_homogeneous(1, 2u, 3));  // false
}

// 4.4.2 变参索引
template <typename C, typename... Idx>
void print_elems(const C& c, Idx... idx) {
  print(c[idx] ...);
}
template <size_t... Idx, typename C>
void print_elems(const C& c) {
  print(c[Idx] ...);
}
void run_print_elems() {
  std::vector<int> v{1, 2, 3, 4, 5, 6};
  print_elems(v, 0, 2, 4); // 1 3 5
  print_elems<1, 3, 5>(v);  // 2 4 6
}

// 4.4.3 变参类模板
template <typename... Elements>
class Tuple {

};  // class Tuple
template <std::size_t...>
struct Indices {
};  // struct Indices
template <typename T, std::size_t... Idx>
void print_by_idx(const T& t, Indices<Idx...>) {
  print(std::get<Idx>(t) ...);
}
void run_variadic_template() {
  PRINT_CURRENT_FUNCTION_NAME;
  Tuple<int, double, std::string> t;
  std::array<std::string, 5> arr{"hello", "world", "foo", "bar", "baz"};
  print_by_idx(arr, Indices<0, 1, 2>());
}

// 4.4.4 变参推导指引
// std::array 推导指引，根据构造参数推导出 array 的大小
template <typename T, std::size_t N>
class MyArray {
public:
  template <typename... U>
  MyArray(U&&... u) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class MyArray
// 当 T 和 U... 都是同一类型时，推导出 MyArray<T, N> (N = 1 + sizeof...(U))
template <typename T, typename... U> MyArray(T, U...)
->
MyArray<std::enable_if_t<(std::is_same_v<T, U> && ...), T>, (1 + sizeof...(U))>;

void run_deduction_guides() {
  MyArray a{1, 2, 3};  // 推导出 MyArray<int, 3>
  MyArray b{'a', 'b'};  // 推导出 MyArray<char, 2>
  // MyArray c{'a', 1};  // 推导失败，因为 'a' 和 1 不是同一类型
}

// 4.4.4 变参基类和 using 关键字
void run_variadic_base() {
  using CustomerOp = Overloader<CustomerEq, CustomerHash>;
  //
  std::unordered_map<Customer, int, CustomerHash, CustomerEq> c1;
  std::println("c1 insert (xuechengyun, 29): {}", c1.insert({Customer("xuechengyun"), 29}).second);;
  std::println("c1 insert (xuechengyun, 29): {}", c1.insert({Customer("xuechengyun"), 29}).second);;
  std::unordered_map<Customer, int, CustomerOp, CustomerOp> c2;
  std::println("c2 insert (xuechengyun, 29): {}", c2.insert({Customer("xuechengyun"), 29}).second);;
  std::println("c2 insert (xuechengyun, 29): {}", c1.insert({Customer("xuechengyun"), 29}).second);;
}

int main() {
  run_print();
  run_print_sizeof();
  run_fold_sum();
  run_fold_traverse();
  run_fold_print();
  run_variadic_template_application();
  run_print_with_calculation();
  run_is_homogeneous();
  run_print_elems();
  run_variadic_template();
  run_deduction_guides();
  run_variadic_base();
  return 0;
}



