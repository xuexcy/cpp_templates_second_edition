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
#include <utility>
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

// 完美转发并不完美, 例如，它无法表达式具有的特定的常量值
void g(int*) {
  PRINT_CURRENT_FUNCTION_NAME;
}
void g(...) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void forward_to_g(T&&x) {
  g(std::forward<T>(x));
}

// 前面都是转发参数，对于返回类型也有相同的问题，需要使用 decltype(auto) 来转发返回值类型
template <typename F, typename... Args>
auto forward_to_g(F&& f, Args&&... args) -> decltype(f(std::forward<Args>(args)...)) {
  return f(std::forward<Args>(args)...);
}
template <typename F, typename... Args>
decltype(auto) forward_to_g(F&& f, Args&&... args) {
  return f(std::forward<Args>(args)...);
}

// 15.6.4 推导的意外情况
void int_lvalues(int&);  // 接受 int 类型的左值
template <typename T>
void lvalues(T&);  // 接受任意类型的左值
void int_rvalues(int&&);  // 接受 int 类型的右值
template <typename T>
void anything(T&&);  // 接受任意类型
// 只有 "模板参数&&" ，并且模板参数由该函数声明，才能接受任意类型，以及对推导行为适用
template <typename T>
struct X {
  X(X&&) {  // X 不是模板参数，接受 X<T> 类型的右值
    PRINT_CURRENT_FUNCTION_NAME;
  }
  X(T&&) {  // 构造函数不是函数模板(其实也可以辩解为 T 不是这个构造函数声明的模板参数)
    PRINT_CURRENT_FUNCTION_NAME;
  }
  template <typename U>
  X(X<U>&&) {  // X<U> 不是模板参数，接受 X<U> 类型的右值
    PRINT_CURRENT_FUNCTION_NAME;
  }
  template <typename U>
  X(U, T&&) {  // 这是一个函数模板，但是 T 不是这个函数声明的模板参数
    PRINT_CURRENT_FUNCTION_NAME;
  }
  template <typename U>
  X(U&&) {  // 接受任意的类型，并使用推导规则
    PRINT_CURRENT_FUNCTION_NAME;
  }

};  // struct X

// 从前面可以看出， "模板参数&&" 可以接受任意的类型(左值或右值), 如果我们希望只接收右值，可以使用 enable_if
template <typename T>
typename std::enable_if_t<!std::is_lvalue_reference_v<T>> rvalue(T&&) {
  PRINT_CURRENT_FUNCTION_NAME;
}

void run_perfect_forwarding() {
  PRINT_CURRENT_FUNCTION_NAME;
  C v;
  const C c;
  forward_to_c(v);  // T = C&, 调用 c(C&)
  forward_to_c(c);  // T = const C&, 调用 c(const C&)
  forward_to_c(C());  // T = C, 调用 c(C&&)
  forward_to_c(std::move(v));  // T = C, 调用 c(C&&)

  g(0);  // 0 可以转换为 nullptr, 调用 g(int*)
  forward_to_g(0);  // 0 是 int, 调用 g(...)
  g(nullptr);
  forward_to_g(nullptr);  // 调用 g(int*)

  {
    X<int> x(1);  // X(T&&), T = int
    X x2(x);  // X(U&&), T = int, U = X<int>&,  不是 X(X&&), 因为 x 不是右值
    auto x3 = std::move(x);  // X(X&&), T = int

    X<double> dx(1);
    X<int> x5(dx);  // X(U&&), T = int, U = X<double>&, 不是 X(X<U>&&), 因为 dx 不是右值
    X<int> x6 = std::move(dx);  // X(X<U>&&), T = int, U = double
    int i = 1;
    // X<int> x7(1.0, i);  i 不是右值，无法调用 X(U, T&&)
    X<int> x8(1.0, 1);  // X(U, T&&), T = int, U = double
  }
  {
    int i;
    // rvalue(i);  // 使用了 enable_if ，参数必须是右值
    rvalue(1);
  }

  std::println();
}

// 15.7 SFINAE
// substitution failure is no an error 替换失败不是错误

// 15.7.1 即时上下文 immediate context
// SFINAE 用于防止试图形成无效的类型或表达式

// 15.8 推导的限制
// 15.8.1 可行的实参转换
template <typename T>
struct B {
};  // struct B
template <typename T>
struct D : public B<T> {
};  // struct D
template <typename T>
void f(B<T>*) {}

// 当推导的上下文中不包含模板参数，那么所有的隐式转换都是允许的
template <typename T>
int f(T, typename T::X) { return 1; }
struct V {
  V() = default;
  struct X {
    X(double) {}
  };  // struct X
};  // struct V

// 15.8.2 类模板实参

// 15.8.3 默认调用实参
// 只有当调用没有提供实参时，才会实例化默认形参 T()
template <typename T>
void init(T* loc, const T& val = T()) {
  *loc = val;
}
struct S {
  S(int, int) {}
};  // struct S

// 默认调用实参不能用于推导模板实参
template <typename T>
void f985(T x = 42) {}

// 15.8.4 异常规范
// 与默认调用实参一样，异常规范只在需要时实例化，也就是不参与模板实参推导
template <typename T>
void f211(T, int) noexcept(nobibi(T())) {}
template <typename T>
void f211(T, ...) {}

void run_limitation_of_deduction() {
  PRINT_CURRENT_FUNCTION_NAME;
  D<long> dl;
  f(&dl);  // T = long, 调用 f(B<long>*)
  {
    V v;
    int r = f(v, 7.0);  // 先将 T 推导为 V, 导致第 2 个参数的类型为 V::X, 而它可以由 double 进行构造
  }
  {
    S s(0, 0);
    init(&s, S(7, 42));  // T = S。第 2 个参数的默认参数 T() 对 S 无效，但是本次调用提供实参，所以不需要实例化默认实参 T()
    // init(&s);  // No matching constructor for initialization of 'S'
  }
  {
    f985<int>();  // T = int, 调用 f985<int>(42)
    // f985();  // 不能用默认调用实参 42 推导模板实参 T
  }
  {
    int i{};
    // f211(i, i);
    /*
    第 1 个 f211 更匹配，所以重载解析选择第 1 个 f211, 但是其异常声明中调用了一个不存在的函数，导致编译失败
    同样的，如果 f211 声明抛出异常，比如 void f211(T, int) throw (typename T::not_exist_type)， 也会编译失败
    两种情况都是因为函数中的异常声明并不参与模板参数推导
    */
  }
  std::println();
}

// 15.9 显示函数模板参数

int main() {
  run_deduction_process();
  run_character_literal();
  run_reference_collapse();
  run_forward_reference();
  run_perfect_forwarding();
  run_limitation_of_deduction();
  return 0;
}



