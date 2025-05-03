/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/03 12:10:49
# Desc   :
########################################################################
*/

#include <print>
#include <type_traits>
#include <utility>
#include <vector>

#include "cpp_utils/util.h"

// 11.1 可调用对象
/* callable
1. function pointer
2. 仿函数 functor: 重载了运算符 ()
3. 带有转换函数的类类型, 转换返回一个 function pointer 或函数引用
*/

// 11.1.1 函数对象的支持
template <typename Iter, typename Callable>
void foreach(Iter current, Iter end, Callable op) {
  while (current != end) {
    op(*current);
    ++current;
  }
}

void func(int i) {
  std::println("func() called for : {}", i);
}

class FuncObj {
public:
  void operator()(int i) const {
    std::println("FuncObj::op()", i);
  }
};  // class FuncObj


void run_foreach() {
  std::vector<int> primes{2, 3, 5, 7, 11, 13, 17, 19};
  foreach(primes.begin(), primes.end(), func);  // func 函数退化(decay)为指针
  foreach(primes.begin(), primes.end(), &func);  // func 的函数指针
  foreach(primes.begin(), primes.end(), FuncObj());  // 函数对象
  foreach(primes.begin(), primes.end(), [](int i) {
    std::println("lambda called for: {}", i);
  });

  std::println();
}

// 11.1.2 处理成员函数及额外的参数
template <typename Iter, typename Callable, typename... Args>
void foreach(Iter current, Iter end, Callable op, const Args&... args) {
  while (current != end) {
    std::invoke(op, args..., *current);
    ++current;
  }
}

class MyClass {
public:
  void memfunc(int i) const {
    std::println("MyClass::memfunc() called for: {}", i);
  }
};  // class MyClass

void run_foreach_for_member_function() {
  std::vector<int> primes{2, 3, 5, 7, 11, 13, 17, 19};
  foreach(primes.begin(), primes.end(), [](const std::string& prefix, int i) {
    std::println("{}{}", prefix, i);
  }, "- value: ");
  MyClass obj;
  foreach(primes.begin(), primes.end(), &MyClass::memfunc, obj);
  std::println();
}

// 11.1.3 封装函数调用
// 1. 使用 decltype(auto) 来支持返回引用类型
template <typename Callable, typename... Args>
decltype(auto) call(Callable&& op, Args&&... args) {
  // decltype(auto) ret = std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
  // ...
  // return ret;
  return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
}

// 2. 需要处理 decltype(auto) 为 void 的情况, 因为不能 return void
template <typename Callable, typename... Args>
decltype(auto) call2(Callable&& op, Args&&... args) {
  if constexpr (std::is_same_v<std::invoke_result_t<Callable, Args...>, void>) {
    std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
    return ;
  } else {
    return std::invoke(std::forward<Callable>(op), std::forward<Args>(args)...);
  }
}

// 11.2 实现泛型库的其他工具
// 11.2.1 类型特征
template <typename T>
class C {
  static_assert(!std::is_same_v<std::remove_cv_t<T>, void>,
      "invalid instantiation of class C for void type");
public:
  template <typename V>
  void f(V&& v) {
    PRINT_CURRENT_FUNCTION_NAME;
    if constexpr (std::is_reference_v<T>) {
      std::println("T is a reference type");
    }
    if constexpr (std::is_convertible_v<std::decay_t<T>, T>) {
      std::println("V can convert to type T");
    }
    if constexpr (std::has_virtual_destructor_v<T>) {
      std::println("T has virtual destructor");
    }
  }
};  // class C

#define SAME(a, b) static_assert(std::is_same_v<a, b>);
void run_type_traits() {
  C<int64_t> c;
  c.f(1);

  // 引用并不是常量
  SAME(const int&, std::remove_const_t<const int&>);
  SAME(int, std::remove_const_t<std::remove_reference_t<const int&>>);
  SAME(const int, std::remove_reference_t<std::remove_const_t<const int&>>);
  // 使用 std::decay
  SAME(int, std::decay_t<const int&>);

  std::make_unsigned_t<int64_t> a{1};
  SAME(uint64_t, decltype(a));
  SAME(uint64_t, std::make_unsigned_t<int64_t>);

  // 未定义的行为
  // In template: 'make_unsigned' is only compatible with non-bool integers and enum types, but was given 'const long long &'
  // std::make_unsigned_t<const int64_t&> b;

  SAME(int&&, std::add_rvalue_reference_t<int>);
  SAME(const int&&, std::add_rvalue_reference_t<const int>);
  // 左值引用保持不变: 使用引用折叠规则 reference collapsing rule
  // 左值引用和右值引用组合最终生成左值引用
  SAME(const int&, std::add_rvalue_reference_t<const int&>);

  // std::is_copy_assignable 只检查左值相关操作
  // std::is_assignable 会考虑只类别，检查能否将一个纯右值赋值给另一个纯右值

  // 一个 int 可以赋值给另一个 int,一下两者等价
  static_assert(std::is_copy_assignable_v<int>);
  static_assert(std::is_assignable_v<int&, int&>);

  static_assert(!std::is_assignable_v<int, int>);
  static_assert(std::is_swappable_v<int>);
  // 考虑值类别
  static_assert(!std::is_swappable_with_v<int, int>);
}

// 11.2.2 std::addressof
// 返回对象或函数的实际地址，但对象可能会重载运算符 &
template <typename T>
void f(T&& x) {
  auto p = &x;  // 运算符 & 可能被重载了
  auto q = std::addressof(x);
  // https://github.com/xuexcy/learning_more_cpp_idioms/blob/main/src/address_of.cc
  // T& -> const volatile char& -> char& -> T*
  auto r = reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(x)));
}

// 11.2.3 std::declval()
// 在不创建对象的情况下，假设由相应类型的可用对象
template <typename T1, typename T2,
  typename RT= std::decay_t<decltype(true ? std::declval<T1>() : std::declval<T2>())>>
RT max(T1 a, T2 b) {
  return b < a ? a : b;
}

// 11.3 完美转发临时变量
template <typename T>
void g(T&& t) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void ff(T&& t) {
  PRINT_CURRENT_FUNCTION_NAME;
  g(std::forward<T>(t));
}
// 在函数内可以使用 auto&& 来创建一个可以被转发的变量
template <typename T>
void foo(T x) {
  auto&& val = get(x);
  set(std::forward<decltype(val)>(val));
}

// 11.4 作为模板参数的引用
template <typename T>
void tmpl_param_is_reference(T) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("T is reference: {}", std::is_reference_v<T>);
}
void run_param_reference() {
  int i;
  int& r = i;
  tmpl_param_is_reference(i);  // false
  tmpl_param_is_reference(r);  // false, T 推导为所引用的类型
  tmpl_param_is_reference<int&>(i);  // true, 显示指定
  tmpl_param_is_reference<int&>(r);  // true, 显示指定
}

template  <typename T, T Z = T{}>
class RefMem {
private:
  T zero;
public:
  RefMem() : zero{Z} {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class RefMem

int null = 0;
void run_ref_mem() {
  RefMem<int> rm1, rm2;  // RefMem<int, 0>: RefMem<T = int, int Z = int{}>
  rm1 = rm2;

  // Non-const lvalue reference to type 'int' cannot bind to an initializer list temporary
  // int& 不能绑定到一个临时的初始化列表
  // RefMem<int&> rm3;

  // Value of type 'int' is not implicitly convertible to 'int &'
  // 0 不能转换为 int&
  // RefMem<int&, 0> rm4;

  extern int null;
  RefMem<int&, null> rm5, rm6;
  // Object of type 'RefMem<int &, null>' cannot be assigned because its copy assignment operator is implicitly deleted
  // 拷贝赋值操作符被隐式删除了，因为成员变量 int& zero 是一个引用，引用是赋能赋值的
  // rm5 = rm6;
}

// 上面的问题就是，对于非类型模板参数使用引用很麻烦
template <typename T, int& SZ>
class Arr {
private:
  std::vector<T> elems;
public:
  Arr() : elems(SZ) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  void print() const {
    for (int i = 0; i < SZ; ++i) {
      std::print("{} ", elems[i]);
    }
    std::println();
  }
};  // class Arr

int size = 3;
void run_arr() {
  // 编译错误
  // In template: 'allocate' declared as a pointer to a reference of type 'int &'
  // 在 vector 的实现细节中，有一个叫 allocate 的变量，它在此时被声明为指向 int& 的指针
  // 但 c++ 不允许声明指向引用的指针, 也就是 int& * allocate = nullptr; 是不行的
  // Arr<int&, size> y;

  Arr<int, size> x;  // elems{0, 0, 0}
  x.print();

  size += 100;
  // 运行时错误，因为 int& SZ = size; 现在 size 变了，print 中会多循环 100 次，超过 elems 的实际大小
  // x.print();

  // 为了在模板参数为引用实例化时仍然具有赋值操作符，std::pair<> 和 std::tuple<> 类实现了赋值操作符，而不是默认行为
  // 因为默认行为无法赋值引用，具体实现方式就是赋值引用指向的值
  int a{1}, b{2}, c{3}, d{4};
  std::pair<int&,int&> p1{a, b};  // {1, 2}
  std::pair<int&,int&> p2{c, d};  // {3, 4}
  p1 = p2;  // a = 3, b = 4;
  std::println("a = {}, b = {}, c = {}, d = {}", a, b, c, d);
}

// 可以使用断言来禁用引用
template <typename T>
class Arr2 {
  static_assert(!std::is_reference_v<T>, "Invalid instantiation of optional<T> for references");
};  // class Arr2
void run_arr2() {
  Arr2<int> a1;
  // Arr2<int&> a2;
}

// 11.5 推迟估算
template <typename T>
class Cont {
private:
  T* elem;
};  // class Cont

template <typename T>
class Cont2 {
private:
  T* elems;
public:
  typename std::conditional<std::is_move_constructible_v<T>, T&&, T&> foo();
};  // class Cont2

template <typename T>
class Cont3 {
private:
  T* elems;
public:
  template <typename D = T>
  std::conditional_t<std::is_move_constructible_v<D>, T&&, T&> foo();
};  // class Cont3

template <template <typename T> class C>
struct Node {
  std::string value;
  C<Node> next;
};  // struct Noe

void run_node() {
  // Node 可以和一个不完整类型使用
  // 当在 Node 定义 Cont<Node> 时, 由于 Node 还没有被完整定义，但由于 Cont 中定义了 Node 自身类型
  // 所以它可以编译成功
  Node<Cont> n1;

  // Cont2 中使用了 std::is_move_constructible, 它要求其参数是一个完整类型
  // 所以这里不能编译成功
  // Node<Cont2> n2;

  // Cont3 中将 foo 从普通函数换成了模板函数，这样 std::is_move_constructible 就会推迟到 foo 函数实例化时
  // 当 foo 实例化的时候, Node 已经被编译器完整可见了
  Node<Cont3> n3;
}

int main() {
  run_foreach();
  run_foreach_for_member_function();
  run_param_reference();
  run_ref_mem();
  run_arr();
  run_arr2();
  run_node();
  return 0;
}
