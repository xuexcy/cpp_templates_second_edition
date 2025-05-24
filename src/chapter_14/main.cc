/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/23 13:35:22
# Desc   : chapter_14 实例化
########################################################################
*/

#include <print>
#include "cpp_utils/util.h"

// 14.1 按需实例化
// 14.2 延迟实例化 : 只对需要的部分进行实例化
// 14.2.1 部分和完全实例化
template <typename T>
T f(T p) { return 2 * p; }
// 这里不需要 f 函数完全实例化，因为只需要知道函数的返回类型即可
// 此时，编译器只允许替换函数的声明，而不允许替换它的函数体
decltype(f(2)) x = 2;

template <typename T>
class Q {
  using Type = typename T::type;
};  // class Q
// Q<int> 的主体不会被替换，因为指针类型不需要 Q<int> 的完整定义
// 另外，如果完全实例化的话就会发生错误，因为 int 没有成员类型 type
Q<int>* p = nullptr;

template <typename T>
T v = T::default_value();
// 这里只使用了变量模板 v<int> 的类型(也就是 T = int), 所以等号右边的 T::default_value() 也没有被实例化
decltype(v<int>) s;

// 14.2.2 实例化组件
template <typename T>
class Safe {
};  // class Safe

template <int N>
class Danger {
  int arr[N];
};  // class Danger
template <typename T, int N, int N2=1, int N3=1>
class Tricky {
public:
  void no_body_here(Safe<T> = 3) {}
  void in_class() {
    Danger<N> no_boom_yet;
  }
  struct Nested {
    Danger<N> p_few;
  };  // struct Nested
  union {
    Danger<N2> anonymous;
    int align;
  };
  void unsafe(T (*p)[N3]);
  void error() {
    // 总是报错，因为数组大小不能是负数
    // Danger<-1> boom;
  }
};  // class Tricky

template <typename T>
class VirtualClass {
public:
  // 根据 class Tricky, 不是所有成员都需要定义
  // 但是构造和析构函数肯定会被调用，所以实例化类时一定会实例化这些函数的定义
  virtual ~VirtualClass() = default;

  // 虚成员也应该被定义, 否则可能会链接错误
  // 如果实例化类时，虚函数没有定义，那么就无法正确生成虚函数表 vtable
  // virtual T vmem();
  virtual T vmem() { return T(); }

};  // class VirtualClass

template <typename T>
class C {
public:
  // operator-> 要求返回类型为指针类型或其他类类型(因为非指针或非类类型没有成员，所以 -> 不起作用)
  // 比如 int 是一个基本类型，如果通过 C<int>()-> 得到一个 int，-> 后面有又应该写什么代码呢？
  // 但是, C++ 允许在 operator-> 在这种情况下没有被调用时，依然可以编译通过
  T operator->();
};  // class C

void run_instantiation() {
  PRINT_CURRENT_FUNCTION_NAME;
  {
    Safe<int> s;
    Tricky<int, 3> t;
    // 默认参数只有在被使用时才会实例化, 这里使用默认参数导致 Safe<T> = 3 编译错误
    // t.no_body_here();

    // 没有使用默认参数，不会实例化默认参数，所以没有问题
    t.no_body_here(s);
  }
  {  // 当实例化时，会实例化成员的声明，成员的定义只有使用时才会实例化
    // Tricky<int, 1, 1, -1> t;  // 此时实例化， unsafe 的声明会报错
  }
  {
    Tricky<int, -1> t;
    // 使用函数时，其函数体才会实例化，然后这里就会报错
    // t.in_class();

    // 使用内部 struct 时，其定义才会实例化，然后报错
    // decltype(t)::Nested n;
  }
  {
    // 匿名内部 union 会跟着实例化，然后报错
    // Tricky<int, 1, -1> t;
  }
  {  // 由于需要 vtable，类实例化时虚函数需要被定义
    VirtualClass<int> inst;
  }
  {
    // 即使 C<int> 的 operator-> 返回类型是 int (非指针、非类类型)
    // 也可以编译通过
    C<int> c;
    // 一旦我们准备调用 -> 时，就发现 -> 不知道跟什么样的代码，因为 int 没有成员
    // c->
  }
  std::println();
}

// 14.3 C++ 的实例化模型
// 14.3.1 两阶段查找
/*
1. 解析模板
2. 实例化模板
*/
namespace N {
template <typename>
void g() {}

enum E { e };
}  // namespace N

template <typename T>
void k(T p) {}

template <typename T>
void h(T p) {
  k<int>(p);
  // 初始普通查找用于确定名称是否为模板，当读到 "g<" 时，无法识别出 g 时一个模板，所以 "<" 被当成小于号
  // 于是解析出错
  // g<int>(p);
}

void run_two_phase_lookup() {
  h(N::e);
}

// 14.3.2 实例化点 POI
// 14.3.3 包含模型
// 这俩没看太懂

// 14.4 实现方案
// 14.4.1 贪婪实例化 : 链接器找到多个实例时，保留一个，丢弃其他所有
// 14.4.2 查询实例化 : 用数据库存储实例化结果，使用时进行查询，避免多次实例化相同的实体
// 14.4.3 迭代实例化 : 直接编译源码，预链接链接目标文件时，发现缺少实例化体就调用编译器生成缺少的实例化体

// 14.5 显示实例化 explicit instantiation
// 14.5.1 手动实例化
/*
在使用贪婪实例化方案时，每个编译单元都会进行实例化，导致对构建时间产生负面影响, 可以通过在编译单元中通过显示声明来禁止
在当前编译单元进行实例化
*/

#include "call_manual_instantiation.h"
void run_manual_instantiation() {
  PRINT_CURRENT_FUNCTION_NAME;
  call_manual_instantiation();
  std::println();
}

/*
实现方式:
1. 通过 .h 声明，但不定义，禁止其他编译单元 #include 时实例化
2. 通过 .tpp 进行定义
3. 通过 .cpp 进行手动实例化
提供给用户:
1. .h 文件
2. .cpp 的编译结果
好处:
1. 将定义隐藏到了 .tpp
2. 可以通过 .cpp 管理手动实例化, 前面的 manual_instantiation.h/.cpp 的两个函数存在的问题
  a. manual_instantiation() 定义暴露在了头文件中
  b. manual_instantiation_2() 的定义和手动实例化混在了同一个文件，不方便管理手动实例化
*/
#include "f.h"
void run_f() {
  PRINT_CURRENT_FUNCTION_NAME;
  f_function<int>();
  std::println();
}

// 14.5.2 显示实例化声明
// 当某个特化在多个不同的编译单元中使用时，显示实例化声明可以缩短编译和链接时间
#include "t.h"
void run_t() {
  PRINT_CURRENT_FUNCTION_NAME;
  t_function<int>();
  t_function<float>();
  t_function2<int>();

  // 禁用了常用特化的实例化: extern template
  // t_function<double>();

  /*
  并不是所有特化都会禁用, 比如
  1. 内联函数, 以便内联展开
  2. 具有可推导的 auto / decltype(auto) 类型的变量和具有可推导的返回类型的函数，以便确定它们的类型
  3. 作为常量表达式的变量，以便使用它们的值
  4. 引用类型的变量，以便解析它们指向哪个实体
  5. 类模板和别名模板，以便检查它们的结果类型

  */
  std::println();
}

/*
显示实例化声明:
  1. 在 .h 头文件中使用 extern template, 防止编译器在其他编译单元自动实例化
  2. 禁用自动实例化
手动实例化:
  1. 在 .cpp 源文件明确要求编译器生成哪些类型的实例化
  2. 显式生成特定类型的实例
两者结合起来，减少编译和链接时间，减少代码膨胀
*/

// 14.6 编译期的 if 语句
template <typename T>
bool if_f(T p) {
  if constexpr (sizeof(T) <= sizeof(long long)) {
    return p > 0;
  } else {
    p.compare(0) > 0;
  }
}
bool call_if_f(int n) {
  return f(n);
}
template <bool b>
struct DispatchIfF {
  template <typename T>
  static bool if_f(T p) {
    return p.compare(0) > 0;
  }
};  // struct DispatchIfF
template <>
struct DispatchIfF<true> {
  template <typename T>
  static bool if_f(T p) {
     return p > 0;
  }
};  // struct DispatchIfF<true>
template <typename T>
bool if_f_2(T p) {
  return DispatchIfF<sizeof(T) <= sizeof(long long)>::if_f(p);
}

void run_if_f() {
  PRINT_CURRENT_FUNCTION_NAME;
  // c++17 : if constexpr
  if_f(1);
  // c++14 : 使用模板特化来进行 dispatch
  if_f_2(1);
  std::println();
}

// 14.7 标准库
/*
在标准库中，显示实例化声明和定义了一些 basic_string, 以便通用实现可以在所有用户之间共享
extern template class basic_string<char>;
extern template class basic_string<wchar_t>;
*/

int main() {
  run_instantiation();
  run_two_phase_lookup();
  run_manual_instantiation();
  run_f();
  run_t();
  run_if_f();
  return 0;
}



