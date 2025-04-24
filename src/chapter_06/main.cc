/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/04/23 00:07:51
# Desc   : chapter 06: 移动语义和 enable_if<>
########################################################################
*/

#include <print>
#include <type_traits>
#include <utility>
#include "cpp_utils/util.h"

// 6.1 完美转发简介
class X {};  // class X

void g(X&) {
  std::println("g() for variable");
}
void g(const X&) {
  std::println("g() for constant");
}
void g(X&&) {
  std::println("g() for moveable object");
}
void f(X& val) {
  g(val);  // g(X&)
}
void f(const X& val) {
  g(val);  // g(const X&)
}
void f(X&& val) {
  g(val);             // g(X&)
  g(std::move(val));  // g(X&&)
}
void run_move1() {
  PRINT_CURRENT_FUNCTION_NAME;
  X v;
  const X c;

  f(v);             // f(X&)
  f(c);             // f(const X&)
  f(X());           // f(X&&)
  f(std::move(v));  // f(X&&)
  std::println();
}
template <typename T>
void f_with_template(T val) {
  PRINT_CURRENT_FUNCTION_NAME;
  g(val);
}

void run_move2() {
  PRINT_CURRENT_FUNCTION_NAME;
  X v;
  const X c;

  f_with_template(v);             // f(X&)
  f_with_template(c);             // f(const X&)
  f_with_template(X());           // f(X&&)
  f_with_template(std::move(v));  // f(X&&)
  std::println();
}

template <typename T>
void f_with_template_and_forward(T&& val) {
  PRINT_CURRENT_FUNCTION_NAME;
  g(std::forward<T>(val));
}

void run_move3() {
  PRINT_CURRENT_FUNCTION_NAME;
  X v;
  const X c;

  f_with_template_and_forward(v);             // f(X&)
  f_with_template_and_forward(c);             // f(const X&)
  f_with_template_and_forward(X());           // f(X&&)
  f_with_template_and_forward(std::move(v));  // f(X&&)
  std::println();
}

/*
模板参数 T&& 和形参 X&& 不一样
1. T&& 声明一个转发引用，可以接受可变、不可边或可移动的对象
2. X&& 声明一个右值引用,只能接受一个可移动对象
另外, typename T::iterator&& 也是声明一个右值引用
*/

// 6.2 特殊成员函数模板
class Person {
private:
  std::string name;
public:
  explicit Person(const std::string& n) : name(n) {
    std::println("copying string-CONSTR for '{}'", name);
  }
  explicit Person(std::string&& n) : name(std::move(n)) {
    std::println(" moving string-CONSTR for '{}'", name);
  }
  Person(const Person& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person(Person&& p) : name(std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};  // class Person

void run_person() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string name = "sname";
  Person p1(name);  // const std::string&
  Person p2("tmp");  // std::string&&
  Person p3(p1);  // const Person&
  Person p4(std::move(p1));  // Person&&
  std::println();
}

class Person2 {
private:
  std::string name;
public:
  template <typename STR>
  // explicit Person2(STR&& n) {// : name(std::forward<STR>(n)) {
  explicit Person2(STR&& n) : name(std::forward<STR>(n)) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("TMPL-CONSTR Person2 '{}'", name);
  }
  Person2(const Person2& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person2(Person2&& p) : name (std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};  // class Person2
void run_person2() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string name = "sname";
  Person2 p1(name);  // STR = std::string&
  Person2 p2("tmp");  // STR = const char(&)[4]
  // 编译出错
  // 根据 C++ 重载规则，对于非常量的 p1, 参数 STR&& 比 const Person2& 更匹配
  // 这样在构造函数中初始化 name(std::forward<STR>(p1)) 时就会编译错误
  // solution 思路:
  //  可以添加一个 Person(Person& p) 构造函数来解决这个问题，但是对于派生类对象，成员模板更为匹配
  //  在传递实参是一个 Person 对象时或一个可以转换成 Person 对象的表达式的情况下，真正需要的是禁止成员模板
  //  使用 enable_if 来实现
  // Person2 p3(p1);  // STR = Person2&
  Person2 p4(std::move(p1));  // 移动构造函数，Person2&&
  const Person2 p2c("ctmp");  // STR = const char(&)[5]
  Person2 p3c(p2c);  // 拷贝构造函数，const Person2&
  std::println();
}


// 6.3 通过 std::enable_if<> 禁用模板
/*
template< bool B, class T = void >
struct enable_if;
(since C++11)
*/
template <typename T>
typename std::enable_if<(sizeof(T) > 4)>::type foo() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("sizeof({}) > 4", typeid(T).name());
}

/*
返回类型为 class T = void
void foo() {
}
*/
// c++14 开始可以使用 std::enable_if_t
template <typename T>
std::enable_if_t<(sizeof(T) > 4), T> foo2() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("sizeof({}) > 4", typeid(T).name());
  return T();
}

// 在声明中使用 enable_if<> 相当拙劣，可以使用带有默认参数值的附加函数模板实参
template <typename T, typename = std::enable_if_t<(sizeof(T) > 4)>>
void foo3() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("sizeof({}) > 4", typeid(T).name());
}
// 或者
template <typename T>
using EnableIfSizeGreater4 = std::enable_if_t<(sizeof(T) > 4)>;
template <typename T, typename = EnableIfSizeGreater4<T>>
void foo4() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("sizeof({}) > 4", typeid(T).name());
}
template <typename T, size_t N>
using EnableIfSizeGreater = std::enable_if_t<(sizeof(T) > N)>;
template <typename T, typename = EnableIfSizeGreater<T, 4>>
void foo5() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("sizeof({}) > 4", typeid(T).name());
}

void run_foo() {
  PRINT_CURRENT_FUNCTION_NAME;
  // foo<char>();  // error: static assertion failed
  // foo<int>(); // error: static assertion failed
  foo<double>();

  // foo2<char>();  // error: static assertion failed
  // foo2<int>(); // error: static assertion failed
  foo2<double>();

  // foo2<char>();  // error: static assertion failed
  // foo2<int>(); // error: static assertion failed
  foo3<double>();

  // foo2<char>();  // error: static assertion failed
  // foo2<int>(); // error: static assertion failed
  foo4<double>();

  // foo2<char>();  // error: static assertion failed
  // foo2<int>(); // error: static assertion failed
  foo5<double>();
  std::println();
}

// 6.4 使用 enable_if<>
class Person3 {
private:
  std::string name;
public:
  // template <typename T>
  // using EnableIfString = std::enable_if_t<std::is_convertible_v<T, std::string>>;
  // template <typename STR, typename = EnableIfString<STR>>
  template <typename STR, typename = std::enable_if_t<std::is_convertible_v<STR, std::string>>>
  explicit Person3(STR&& n) : name(std::forward<STR>(n)) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("TMPL-CONSTR Person3 '{}'", name);
  }
  Person3(const Person3& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person3(Person3&& p) : name (std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};  // class Person3

/*
为什么不反向检查 STR 是否"不可转换为 Person3", 即 std::enable_if_t<!std::is_convertible_v<STR, Person3>>
此时我们正在定义一个(构造)函数，它通过"使能"成功可以将字符串"转换"为 Person3
"使能"即 std::enable_if, "转换"即"std::is_convertible
1. 构造函数"使能"成功，就允许字符串"转换"为 Person3
2. 构造函数"使能"成功取决于 STR 到 Person3 "转换"不成功
3. STR 是否可以"转换"为 Person3 取决于该构造函数定义成功
也就是构造函数的定义依赖于"使能", "使能"依赖于"转换", "转换"依赖于构造函数,如是并形成了依赖循环

切勿在影响 enable_if 使用条件的地方使用 enable_if.
比如该构造函数使用了 enable_if，且该构造函数可以影响 enable_if 的条件 !std::is_convertible_v<STR, Person3>

*/

void run_person3() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string name = "sname";
  Person3 p1(name);  // STR = std::string&
  Person3 p2("tmp");  // STR = const char(&)[4]
  // 和 Person2 对比，这里可以编译成功
  Person3 p3(p1);  //  拷贝构造函数，const Person3&
  Person3 p4(std::move(p1));  // 移动构造函数，Person3&&
  const Person3 p2c("ctmp");  // STR = const char(&)[5]
  Person3 p3c(p2c);  // 拷贝构造函数，const Person3&
  std::println();
}

/*
std::is_convertible<From, To> 检查类型 From 可 (构造、类型转换符或其他方式) 隐式转换为 To
std::is_constructible<To, Args...> 检查是否可以由参数 Args... 构造类型 To
*/

// 禁用特殊成员函数
// 不能使用 enable_if 来禁用预定义的拷贝/移动构造函数和赋值运算符
// 因为成员模板函数不算特殊成员函数,某些情况下会被忽略掉
class C {
public:
  template <typename T>
  C(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C

void run_forbid_special_method_function() {
  PRINT_CURRENT_FUNCTION_NAME;
  C c1(1);  // C(const T&) [T = int]
  C c2("abc");  // C(const T&) [T = const char(&)[4]]
  C c3(std::string("abc"));  // C(const T&) [T = const std::string&)]
  C c4("efg");  // C(const T&) [T = char(&)[4])]
  // 1. 编译器找到拷贝构造函数
  // 2. 编译器找到模板构造函数，没有发现比拷贝构造函数更合适的
  //    (在 Person2 中， 以 STR&& 为参数构造的 Person2 p3(p1); 比拷贝构造函数更合适)
  // 3. 选择拷贝构造函数
  C c5(c1); // C(const C&), 这不是成员模板函数，而是预定义的拷贝构造函数
  std::println();
}

class C2 {
public:
  C2(const C2&) = delete;
  template <typename T>
  C2(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C2

void run_forbid_special_method_function_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  C2 c1(1);  // C(const T&) [T = int]
  // 编译错误，因为拷贝构造函数被 delete 了
  // C2 c2(c1);
  std::println();
}

// 使用 const volatile 实参声明一个 delete 的拷贝构造函数
// 对于 nonvolatile 类型，优先级: const C3&   > const T&  > const C3 volatile&
class C3 {
public:
  C3(const volatile C3&) = delete;
  template <typename T>
  C3(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C3

void run_forbid_special_method_function_3() {
  PRINT_CURRENT_FUNCTION_NAME;
  C3 c1(1);  // C(const T&) [T = int]
  C3 c2(c1);  // C(const T&) [T = C3]
  volatile C3 c3(1);
  // 编译错误，因为 c3 是 volatile 的
  // C3 c4(c3);
  std::println();
}

class C4 {
public:
  C4(const C4&) = delete;
  template <typename T>
  C4(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C4

void run_forbid_special_method_function_4() {
  PRINT_CURRENT_FUNCTION_NAME;
  C4 c1(1);  // C(const T&) [T = int]
  // 编译错误
  // C4 c2(c1);
  volatile C4 c3(1);  // C3(const T &) [T = volatile C3]
  C4 c4(c3);  // 拷贝构造 const volatile C4&
  std::println();
}


class C5 {
public:
  C5(const C5&) = delete;
  C5(const volatile C5&) = delete;
  template <typename T>
  C5(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C5

void run_forbid_special_method_function_5() {
  PRINT_CURRENT_FUNCTION_NAME;
  C5 c1(1);  // C(const T&) [T = int]
  // 编译错误
  // C5 c2(c1);
  volatile C5 c3(1);
  // 编译错误
  // C5 c4(c3);
  std::println();
}

// 6.5 使用概念简化 enable_if<> 表达式
// C++20 开始支持概念 concept
class Person4 {
private:
  std::string name;
public:
  // note here
  template <typename STR>
  requires std::is_convertible_v<STR, std::string>
  explicit Person4(STR&& n) : name(std::forward<STR>(n)) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("TMPL-CONSTR Person4 '{}'", name);
  }
  Person4(const Person4& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person4(Person4&& p) : name (std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};

  // note here
template <typename STR>
concept ConvertibleToString = std::is_convertible_v<STR, std::string>;
class Person5 {
private:
  std::string name;
public:
  // note here
  template <typename STR>
  requires ConvertibleToString<STR>
  explicit Person5(STR&& n) : name(std::forward<STR>(n)) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("TMPL-CONSTR Person5 '{}'", name);
  }
  Person5(const Person5& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person5(Person5&& p) : name (std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};
class Person6 {
private:
  std::string name;
public:
  // note here
  template <ConvertibleToString STR>
  explicit Person6(STR&& n) : name(std::forward<STR>(n)) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("TMPL-CONSTR Person6 '{}'", name);
  }
  Person6(const Person6& p) : name(p.name) {
    std::println("COPY-CONSTR Person '{}'", name);
  }
  Person6(Person6&& p) : name (std::move(p.name)) {
    std::println("MOVE-CONSTR Person '{}'", name);
  }
};


int main() {
  run_move1();
  run_move2();
  run_move3();
  run_person();
  run_person2();
  run_foo();
  run_person3();
  run_forbid_special_method_function();
  run_forbid_special_method_function_2();
  run_forbid_special_method_function_3();
  run_forbid_special_method_function_4();
  run_forbid_special_method_function_5();
  return 0;
}
