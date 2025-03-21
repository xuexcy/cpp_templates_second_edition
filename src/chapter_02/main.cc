/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/14 18:08:31
# Desc   :
########################################################################
*/

#include <cassert>
#include <iostream>
#include <list>
#include <ostream>
#include <stack>
#include <typeinfo>
#include <vector>

#include "cpp_utils/util.h"

template <typename T>
class Stack {
private:
  std::vector<T> elems;
public:
  void push(const T &elem);
  void pop();
  const T& top() const;
  bool empty() const {
    return elems.empty();
  }
  void print(std::ostream& os) const {
    for (auto& elem : elems) {
      os << elem << " ";
    }
  }

};  // class Stack

template <typename T>
void Stack<T>::push(const T &elem) {
  elems.push_back(elem);
}

template <typename T>
void Stack<T>::pop() {
  assert(!elems.empty());
  elems.pop_back();
}

template <typename T>
const T& Stack<T>::top() const {
  assert(!elems.empty());
  return elems.back();
}

// 2.4 友元简介
// 2.4.1 随着模板实例化的友元
template <typename T>
class Stack1 : public Stack<T> {
  // 这是一个普通函数，随着 Stack1<T> 而实例化。而不是一个函数模板
  friend std::ostream& operator<<(std::ostream& os, const Stack1<T>& stack) {
    PRINT_CURRENT_FUNCTION_NAME;
    stack.print(os);
    return os;
  }
};  // class Stack1

// 2.4.2 使用不同于类模板参数的新参数 U
template <typename T>
class Stack2 : public Stack<T> {
  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const Stack2<U>& stack);
};  // class Stack2
template <typename U>
std::ostream& operator<<(std::ostream& os, const Stack2<U>& stack) {
  PRINT_CURRENT_FUNCTION_NAME;
  stack.print(os);
  return os;
}

template <typename T>
class Stack3;
template <typename T>
std::ostream& operator<<(std::ostream& os, const Stack3<T>& stack);

template <typename T>
class Stack3 : public Stack<T> {
  // 通过 T 特化 operator<<
  friend std::ostream& operator<< <T>(std::ostream& os, const Stack3<T>& stack);
};  // class Stack3
template <typename T>
std::ostream& operator<<(std::ostream& os, const Stack3<T>& stack) {
  PRINT_CURRENT_FUNCTION_NAME;
  stack.print(os);
  return os;
}

/**
友元对比:
Stack1 中的 operator<< 是一个普通函数，和 Stack1<T> 一起实例化。
Stack2 中的 operator<< 是一个函数模板，在使用时进行实例化，
Stack3 中的 operator<< 时一个函数模板，在 Stack3<T> 通过 T 进行了特化，也就是 operator<< <T>
 */

template <template <typename...> typename TStack>
void run_stack() {
  PRINT_CURRENT_FUNCTION_NAME;
  TStack<int> intStack;
  intStack.push(42);
  intStack.push(7);
  std::cout << intStack << std::endl;
  intStack.pop();
  std::cout << intStack << std::endl;

  TStack<std::string> stringStack;
  stringStack.push("hello");
  stringStack.push("world");
  std::cout << stringStack << std::endl;
  stringStack.pop();
  std::cout << stringStack << std::endl;
  std::println();
}

void run_friends() {
  PRINT_CURRENT_FUNCTION_NAME;
  // 随着 Stack1<T> 而实例化的 operator<<
  // std::ostream &operator<<(std::ostream &, const Stack1<T> &)
  run_stack<Stack1>();
  // 根据实参实例化的 operator<<
  // std::ostream &operator<<(std::ostream &, const Stack2<U> &) [U = int]
  run_stack<Stack2>();
  // 通过 T 特化的 operator<< 友元
  // std::ostream &operator<<(std::ostream &, const Stack3<T> &) [T = int]
  run_stack<Stack3>();
}


// 2.5 类模板的特化 specialization
// 将某一个具体的模板参数类型 T 进行单独实现
template <>
class Stack1<double> : public Stack<double> {
public:
  friend std::ostream& operator<<(std::ostream& os, const Stack1<double>& stack) {
    PRINT_CURRENT_FUNCTION_NAME;
    stack.print(os);
    std::println("double biubiubiu = {}", stack.biubiubiu);
    return os;
  }
private:
  double biubiubiu{1.1};
};  // class Stack

void run_template_specialization() {
  PRINT_CURRENT_FUNCTION_NAME;
  Stack1<double> s;
  s.push(42.0);
  std::cout << s << std::endl;
}

// 2.6 偏特化 partial specialization
// 对模板进行特化，特化后引入一个新的模板参数。
/**
如下通过 Stack<U*> 特化 Stack<T>，其中 U 是一个新的模板参数，即偏特化
如果通过 Stack<int*> 特化 Stack<T>，则叫特化

一般我们会继续使用 T 作为新引入的模板参数，即
template <typename T>
class Stack<T*>;
 */
template <typename U>
class Stack<U*> {
private:
  std::vector<U*> elems;
public:
  void push(U *elem) {
    elems.push_back(elem);
  }
  U* pop() {
    U* top = this->top();
    elems.pop_back();
    return top;
  }
  U* top() const {
    assert(!elems.empty());
    return elems.back();
  }
  bool empty() const {
    return elems.empty();
  }
  void print(std::ostream& os) const {
    for (auto& elem : elems) {
      os << elem << " ";
    }
  }
};  // class Stack<U*>

template <typename T1, typename T2>
class Class1 {
};  // class Class1
template <typename T>
class Class1<T, T> {
};  // class Class1<T, T>
template <typename T>
class Class1<T, int> {
};  // class Class1<T, int>
template <typename T1, typename T2>
class Class1<T1*, T2*> {
};  // class Class1<T1*, T2*>

void run_partial_specialization() {
  PRINT_CURRENT_FUNCTION_NAME;
  Stack1<int*> s;
  s.push(new int{42});
  std::cout << s << std::endl;
  delete s.pop();

  Class1<int, float> c1;  // Class1<T1, T2>
  Class1<float, float> c2;  // 偏特化 Class1<T, T>
  Class1<float, int> c3;  // 偏特化 Class1<T, int>
  Class1<int*, float*> c4;  // 偏特化 Class1<T1*, T2*>

  // Ambiguous partial specializations
  // 匹配 Class1<T, T> 和 Class1<T, int>
  // Class1<int, int> c5;
  // 匹配 Class1<T, T> 和 Class1<T1*, T2*>
  // Class1<int*, int*> c6;
}


// 2.7 默认类模板实参
template <typename T, typename Container=std::vector<T>>
class Queue {
public:
  void print_container_type() {
    std::println("{}", typeid(elems).name());
  }
private:
  Container elems;
};  // class Queue

void run_default_template_argument() {
  PRINT_CURRENT_FUNCTION_NAME;
  Queue<int> q1;
  q1.print_container_type();
  Queue<int, std::stack<double>> q2;
  q2.print_container_type();
}

// 2.8 类型别名
// 2.8.1 类型定义和别名声明
// 类型定义
using IntStack = Stack<int>;
// 别名声明
typedef Stack<double> DoubleStack;
// 2.8.2 别名模板
template <typename T>
using NewBeeStack = Stack<T>;
template <typename T>
using NewBeeQueue = Queue<T, std::list<T>>;
// 2.8.3 成员类型的别名模板
template <typename T>
class Container {
  using type = std::vector<T>;
  using iterator = type::iterator;
};  // class Container
template <typename T>
using ContainerIterator = Container<T>::iterator;
// 2.8.4 类型特征后缀 _t
template <typename T>
using container_t = Container<T>::type;



// 2.9 类模板实参推导
namespace type_deduction {
template <int I>
struct IntToType {
};  // struct Int2Type
template <typename T>
struct Stack {
  std::vector<T> elems;
  Stack(const T &elem, IntToType<0>) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Stack(const T &elem, IntToType<1>): elems({elem}) {}
  Stack(T elem): elems({elem}) {
  // Stack(T elem): elems({ std::move(elem)}) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  void push(const T &elem) {
    elems.push_back(elem);
  }
};  // struct Stack
template <typename T>
struct Stack2 {
  std::vector<T> elems;
  Stack2(const T &elem) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct Stack2
}  // namespace type_deduction
void run_type_deduction() {
  PRINT_CURRENT_FUNCTION_NAME;
  using type_deduction::Stack;
  using type_deduction::IntToType;
  Stack<int> s1 = 0;  // c++17 以前
  Stack s2 = 0;  // c++17 及以后推导为 Stack<int>
// 2.9.1 字符串字面量的类模板实参推导
//  "abc" 为 const char[4]
//   A: 当 Stack 的构造函数按引用传递时，即 Stack(const T& elem), 按引用传递的类型不会发生退化 (1.2.1 中有提到)
//     那么 T 就会被推导为 const char[4] (TO: 根据 stdout 的结果看， T 被推导为了 char[4], 没有 const),
//     但是 char[4] (数组)不能被拷贝和赋值，所以 s4 调用的构造函数中无法正确初始化 elems
  Stack s3("abc", IntToType<0>());  // T -> char[4]
  // std::vector<char[4]> v;
  // v.push_back("abc");  // 错误，数组不能被拷贝
  // Stack s4("abc", IntToType<1>());  // T -> char[4]
  using type_deduction::Stack2;
  Stack2 s5 = "abc";  // T -> char[4]

//   B: 当 Stack 的构造函数按值传递时，即 Stack(T elem), const char[4] 会退化为 const char*，由此推导出
//     T 为 const char*，所以 elems 被初始化为 std::vector<const char*>，
  Stack s6 = "abc";  // T -> const char*

}


namespace deduction_guide {
template <typename T>
struct Stack {
  std::vector<T> elems;
  Stack(const T &elem): elems({elem}) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct Stack2
// 2.9.2 推导指引 deduction guide
// c++17 的语法
Stack(const char*) -> Stack<std::string>;
Stack(int) -> Stack<bool>;
}  // namespace deduction_guide

void run_deduction_guide() {
  PRINT_CURRENT_FUNCTION_NAME;
  using deduction_guide::Stack;
  {
    Stack s("abc");  // T -> std::string
    Stack s1{"bac"};
    // Stack s2 = "abc";  // 不能将字符串字面量传递给期望接受 std::string 类型的构造函数来【拷贝初始化】(即使用 = 赋值)
    Stack s3 = {"abc"};
  }
  {
    Stack s(1);  // T -> bool
    Stack s1{1};
    Stack s2 = 1;
    Stack s3 = {1};
  }
  // 类模板实参推导的结果是可拷贝的
  {
    Stack s("abc");
    Stack s1(s);  // 拷贝构造
  }
}

// 2.10 模板化聚合体 template aggregate
template <typename T>
struct ValueWithComment {
  T value;
  std::string comment;
};  // struct ValueWithComment

void run_template_aggregate() {
  PRINT_CURRENT_FUNCTION_NAME;
  ValueWithComment<int> v1{42, "the answer"};
  std::println("{} {}", v1.value, v1.comment);

  ValueWithComment v2 = {"hello", "world"};
  std::println("{} {}", v2.value, v2.comment);
}


int main() {
  run_friends();
  run_template_specialization();
  run_partial_specialization();
  run_default_template_argument();
  run_type_deduction();
  run_deduction_guide();
  run_template_aggregate();
  return 0;
}
