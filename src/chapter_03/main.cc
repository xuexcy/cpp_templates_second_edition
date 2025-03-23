/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/21 13:34:44
# Desc   :
########################################################################
*/
#include <vector>

#include "cpp_utils/util.h"
#include "stacknontype.h"

// 3.1 非类型的类模板参数
void run_stacknontype() {
  using namespace stacknontype;
  PRINT_CURRENT_FUNCTION_NAME;
  Stack<int, 20> int20Stack;
  Stack<int, 40> int40Stack;
  int20Stack.push(1);
  std::println("int20Stack top = {}", int20Stack.top());

  Stack<std::string, 30> string30Stack;
  string30Stack.push("b");
  std::println("string30Stack top = {}", string30Stack.top());
}

// 3.2 非类型的函数模板参数
template <int Val, typename T>
T add_value(T x) {
  return x + Val;
}
template <auto Val, typename T = decltype(Val)>
T add_value_2(T x) {
  return x + Val;
}  // add_value
template <auto Func>
void add_value() {
  std::println();
  PRINT_CURRENT_FUNCTION_NAME;
  std::vector<int> v{1, 2, 3, 4, 5};
  std::println("v: {}", v);
  std::transform(v.begin(), v.end(), v.begin(), Func);
  std::println("v after add_value: {}", v);
}
// 不使用 auto
template <typename Func, Func func>
void add_value() {
  std::vector<int> v{1, 2, 3, 4, 5};
  std::transform(v.begin(), v.end(), v.begin(), func);
}

void run_add_value() {
  PRINT_CURRENT_FUNCTION_NAME;
  constexpr auto func = add_value<10, int>;  // int (*)(int)

  add_value<func>();

  // 这里必须要用 constexpr，因为模板参数是在编译期确定的，如果只使用 auto，那 func 变量就是一个运行时的变量，
  // 这样就无法在编译期确定 add_value 确定的第二个模板参数值 Func func
  add_value<decltype(func), func>();
  add_value<add_value<10, int>>();
}


// 3.3 非类型模板参数的限制(不同c++版本对非类型模板参数的限制不同)
// int、enum、pointer、左值引用、std::nullptr_t(nullptr 是值，std::nullptr_t 是类型)
template <double Val>  // 不能是 double ，但是当前编译器支持
double doubledouble() {
  return Val * 2;
}

// template <std::string Val>  // 类类型的对象不能作为非类型模板参数
// std::string stringstring() {
//   return Val + Val;
// }

// 指针或引用做模板实参是，对象不能是字符串字面量、临时变量或数据成员以及其他子成员
template <const char* name>
class MyClass {};  // class MyClass

// TODO(xuechengyun): 待验证, 文中说 c++11 中，对象必须有外部链接，但又说 s2 在 c++11 也可以
// c++ 11 中，对象必须有外部链接
// c++ 14 中，对象必须有外部链接或内部链接
extern const char s1[] = "hi";  // c++所有版本, 外部链接
const char s2[] = "hi";  // c++11/c++14, 内部链接
void run_limitation() {
  // 根据 c++ 版本，限制有所不同
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("doubledouble(1.0) = {}", doubledouble<1.0>());
  // MyClass<"hello"> v;  // 字符串字面量不能做模板实参

  MyClass<s1> m1;
  MyClass<s2> m2;
  static const char s3[] = "hi";  // c++17, 无链接
  MyClass<s3> m3;
}

// 3.4 模板参数类型 auto
// c++17 可以使用 auto 来接受任意允许的非类型模板参数
// c++14 只能将 auto 作为返回类型，让编译器推导返回类型, 比如 auto size() const { return numElems; }
template <typename T, auto MaxSize>
class Stack {
public:
  using size_type = decltype(MaxSize);
private:
  std::array<T, MaxSize> elems;
  size_type numElems{0};
public:
  Stack() = default;
  size_type size() const { return numElems; }

};  // class Stack
template <auto T>
class Msg {
public:
  void print() const {
    std::println("Msg: {}", T);
  }
};  // class Msg

template <decltype(auto) N>
class C {
public:
  void print() const {
    std::println("C: {}", N);
  }
};  // class C
void run_type_auto() {
  Stack<int, 20> int20Stack1;
  Stack<int, 20u> int20Stack2;
  if constexpr (std::is_same_v<decltype(int20Stack1), decltype(int20Stack2)>) {
    std::println("int20Stack1 and int20Stack2 are the same type");
  } else {
    std::println("int20Stack1 and int20Stack2 are different types");
  }
  static const char s[] = "hello";
  Msg<s> msg1;
  msg1.print();
  Msg<42> msg2;
  msg2.print();

  constexpr int i{22};
  C<i> x;
  x.print();
}

int main() {
  run_stacknontype();
  run_add_value();
  run_limitation();
  run_type_auto();
  return 0;
}

