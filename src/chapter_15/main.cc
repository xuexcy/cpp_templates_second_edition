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
#include <print>
#include <type_traits>
#include <utility>
#include <cxxabi.h>
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
// 可以指定部分模板参数，但必须从左到右, 所以需要首先指定不能推导的参数
template <typename Out, typename In>
Out convert(In p) {
  return p;
}
template <typename In, typename Out>
Out convert2(In p) {
  return p;
}
// 可以指定一个空的模板参数列表，来确保调用的是模板函数
void f123(int) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void f123(T) {
  PRINT_CURRENT_FUNCTION_NAME;
}

void run_explicit_template_argument() {
  PRINT_CURRENT_FUNCTION_NAME;
  {
    auto x = convert<double>(42);  // 显示指定 Out = double, 推导出 In = int
    SAME_TYPE(decltype(x), double);
  }
  {
    /*
      如果将不可推导的模板参数声明在后面, 那么就必须同时显示声明 In 和 Out
      In 可以通过函数参数 p 进行推导，但返回类型 Out 必须显示指定，所以当 Out 声明在 In 后面时,
      调用函数 convert2 时就要同时显示指定 In 和 Out 的类型
    */
    auto x = convert2<int, double>(42);
  }
  {
    f123(1);  // 调用 void f123(int)
    f123<>(1);  // 明确调用模板函数 f123, 通过参数 1 推导出类型 T = int
  }
  std::println();
}

// 15.10 基于初始化器和表达式的推导
// 15.10.1 auto 类型的规范
/*
1. auto 类型的变量永远不会是引用类型
2. auto& 总是引用类型
3. auto&& 类型转发引用(T&&)
*/
template <typename T>
struct XX {
  const T m;
};  // struct X

void run_auto() {
  PRINT_CURRENT_FUNCTION_NAME;
  int x{0};
  {
    auto y = x;  // auto -> int
    SAME_TYPE(decltype(y), int);
  }
  {
    auto& y = x;  // auto& -> int&
    SAME_TYPE(decltype(y), int&);
  }
  {
    // auto& y3 = 0;  // no, auto& 永远是引用类型，无法引用一个右值
  }
  {
    auto&& y = x;  // auto&& -> int&
    SAME_TYPE(decltype(y), int&);
  }
  {
    auto&& y = 0;  // auto&& -> int&&, 右值引用
    SAME_TYPE(decltype(y), int&&);
  }
  {
    auto const N = 400u;  // auto -> unsigned int, 变量 N 类型 const unsigned int
    auto* gp = (void*)nullptr;  // auto -> void, 变量 gp 类型 void*
    auto const XX<int>::*pm = &XX<int>::m;  // auto -> int, 变量 pm 类型, const XX<int>::int *

    // auto 必须是声明的“主”类型说明符
    // X<auto> xa = X<int>();  // auto 是模板实参
    // int const auto::*pm2 = &X<int>::m;  // auto 是声明符的一部分
  }

  std::println();
}

// 可推导的非类型参数
// C++17 之前，必须使用特定的类型来声明非类型模板实参，但是该类型可以是模板参数的类型
template <typename T, T V> struct SS;
SS<int, 42>* ps;  // 即要指定 T = int, 还要指定 V = 42
// C++17 增加了声明非类型模板参数的功能
template <auto V> struct SS2;
SS2<42>* ps2;  // 推导出 auto 为 int, V = 42
SAME_TYPE(decltype(ps2), SS2<static_cast<int>(42)>*);
SS2<42u>* ps3;
SAME_TYPE(decltype(ps3), SS2<static_cast<unsigned int>(42)>*);
// 非类型模板参数类型的一般约束依然有效
SS2<3.14>* pd;  // 书上说这里是错误的，但是从 c++20 开始浮点数可以做非类型实参的类型
SAME_TYPE(decltype(pd), SS2<static_cast<double>(3.14)>*);

// 使用 decltype 获取非类型实参的类型
template <auto V>
struct Value {
  using ArgType = decltype(V);
};  // struct Value
template <typename> struct PMClassT;
template <typename C, typename M> struct PMClassT<M C::*> {  // class C 的类型为 M 的成员变量指针
  using Type = C;
};
template <typename PM>
using PMClass = typename PMClassT<PM>::Type;

// auto 非类型模板参数也可以用于参数化类成员的模板
template <auto PMD>
struct CounterHandle {
  PMClass<decltype(PMD)>& c;
  CounterHandle(PMClass<decltype(PMD)>& c): c(c) {}
  void incr() { ++(c.*PMD); }
};  // struct CounterHandle
struct MyS {
  int i;
};  // struct MyS

void run_auto_for_class_member() {
  PRINT_CURRENT_FUNCTION_NAME;
  MyS s{41};
  /* 这里 auto 推导为 MyS 的成员 i
  在 CounterHandler 中通过 PMClass 偏特化获取其模板参数 PMD 所属的类型，以便接受这个类型的实例 s 用于构造
  */
  CounterHandle<&MyS::i> h(s);
  h.incr();
  std::println();
}

// auto 也可以用于非类型参数包
template <auto... Vs> struct Values {};  // 不要求每个非类型参数的类型都相同
Values<1, 2, 3> v1;
Values<1, 'x', nullptr> v2;
SAME_TYPE(decltype(v2), (Values<static_cast<int>(1), static_cast<char>('x'), static_cast<std::nullptr_t>(nullptr)>));
// 强制非类型模板参数的类型相同, 不过这样模板参数列表不能为空
template <auto V1, decltype(V1)... VRest> struct Values2 {};


// 15.10.2 使用 decltype 表示一个表达式的类型
/*
decltype(e)
1. 如果 e 是实体名称或类成员访问名称，则 decltype(e) 是实体或类成员声明的类型
2. 否则，如果 e 是表达式，decltype(e) 生成反应该表达式的类型和只类别的类型
  a. e 是 T 类型左值， decltype(e) -> T&
  b. e 是 T 类型将亡值, decltype(e) -> T&&
  c. e 是 T 类型纯右值, decltype(e) -> T
int a;
a 是实体
(a) 是表达式

*/
void run_decltype() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string&& s("abc");
  // 实体 s 的类型是 std::string&&
  using obj_type = decltype(s);
  // 表达式 (s) 指涉一个变量，因此是左值，decltype(e) 将得到左值的引用，即 std::string&
  using expr_type = decltype((s));

  SAME_TYPE(std::string&&, obj_type);
  static_assert(std::is_rvalue_reference_v<obj_type>);

  SAME_TYPE(std::string&, expr_type);
  static_assert(std::is_lvalue_reference_v<expr_type>);
  std::println();
}

// 15.10.3 decltype(auto)
int gg() { return 1; }
decltype(auto) f() {
  int r = gg();
  // return (r);  // 返回一个临时对象的引用(int&)
  return r;  // 返回类型为 int
}
// c++17 开始， decltype(auto) 可以用于推导非类型参数
template <decltype(auto) Val> struct N {
  using Type = decltype(Val);
  N() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("type of template argument: {}", cpp_utils::type_name<Type>);
  }

};
template <auto Val> struct N2 {
  using Type = decltype(Val);
  N2() {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("type of template argument: {}", cpp_utils::type_name<Type>);
  }
};


int v = 42;
extern int v;
constexpr int vv = 42;
void run_decltype_auto() {
  PRINT_CURRENT_FUNCTION_NAME;
  {
    int i = 42;
    const int& ref = i;
    auto x = ref;  // int
    decltype(auto) y = ref;  // const int&
  }
  {
    int x;
    decltype(auto) z = x;  // int
    SAME_TYPE(decltype(z), int);
    decltype(auto) r = (x);  // int&,  加上括号就是表达式，表达式返回了一个左值，r 的类型是左值引用
    SAME_TYPE(decltype(r), int&);
  }
  {
    constexpr int c = 42;
    SAME_TYPE(decltype(c), const int);
    SAME_TYPE(decltype((c)), const int&);

    SAME_TYPE(decltype(v), int);
    SAME_TYPE(decltype((v)), int&);

    // 非类型参数需要是常量表达式，或则是全局变量的引用
    {
      // c -> decltype(auto) -> int
      N<c> n1;
      SAME_TYPE(decltype(n1), N<static_cast<int>(42)>);

      // N<(c)> n2;  // (c) -> decltype(auto) -> const int&, 但不是全局变量的引用

      // N<v> n3;  // v 不是常量表达式

      N<(v)> n4;  // (v) -> decltype(auto) -> int&, 是全局变量的引用
      SAME_TYPE(decltype(n4), N<(int&)v>);

      N<vv> n5;  // vv -> decltype(auto) -> int, 是常量表达式
      SAME_TYPE(decltype(n5), N<static_cast<int>(42)>);

      N<(vv)> n6;  // (vv) -> decltype(auto) -> const int&, 是全局变量的引用
      SAME_TYPE(decltype(n6), N<(const int&)vv>);
    }
    {
      N2<c> n1;  // c -> auto -> int
      SAME_TYPE(decltype(n1), N2<static_cast<int>(42)>);

      N2<(c)> n2;  // (c) -> auto -> int
      SAME_TYPE(decltype(n2), N2<static_cast<int>(42)>);

      // N2<v> n3;  // v 不是常量表达式
      // N2<(v)> n4;  // (v) ->auto -> int, v 不是常量表达式

      N2<vv> n5;  // vv -> auto -> int, vv 是常量表达式
      SAME_TYPE(decltype(n5), N2<static_cast<int>(42)>);

      N2<(vv)> n6;  // (vv) -> auto -> int, 是全局变量的引用
      SAME_TYPE(decltype(n6), N2<static_cast<int>(vv)>);
    }
  }
  std::println();
}

// 15.10.4 auto 推导的特殊情况
// 任何使用推导的返回类型的函数模板都需要立即实例化
template <typename T, typename U>
auto addA(T t, U u) -> decltype(t + u) { return t + u; }
void addA(...);
template <typename T, typename U>
auto addB(T t, U u) -> decltype(auto) { return t + u; }
void addB(...);
struct AB {
  AB operator+(AB& other) { return *this; }
};  // struct AB
using AddResultA = decltype(addA(AB(), AB()));
SAME_TYPE(AB, AddResultA);
// 书上说 addB 的函数体没有完全实例化，但这里(clang++ 20.1.6)是可以推导的
using AddResultB = decltype(addB(AB(), AB()));
SAME_TYPE(AB, AddResultB);

// 15.10.5 结构化绑定
// c++17 structured binding
struct MaybeInt {
  bool valid;
  int value;
};  // struct MaybeInt
MaybeInt maybe_g() { return {false, 1}; }

auto ff() -> int(&)[2] {  // 返回对数组的引用
  static int d[2] = {1, 2};
  std::println("d[0] = {}, d[1] = {}", d[0], d[1]);
  return d;
}
/*
结构化 3 种情况
1. 简单类类型
2. 数组
3. 类似用于 std::tuple 类的 std::get<>()
  a. 特化 std::tuple_size, std::tuple_element
  b. e 具有任何名为 get 的成员, 例如 e.get<i>(), 或者使用 get<i>(e)
  c. 下面样例， E1 使用 get 成员模板函数, E2 使用 get<i>(e)
*/

struct E1 {
  int v1;
  double v2;
  // 实现 e.get<i>
  template <std::size_t Idx>
  auto& get() {
    PRINT_CURRENT_FUNCTION_NAME;
    if constexpr (Idx == 0) { return v1; }
    else { return v2; }
  }
  template <std::size_t Idx>
  const auto& get() const {
    PRINT_CURRENT_FUNCTION_NAME;
    if constexpr (Idx == 0) { return v1; }
    else { return v2; }
  }
};  // struct E1
template <>
class std::tuple_size<E1> : public std::integral_constant<std::size_t, 2>{
public:
  // static const unsigned value = 2;
};  // class std::tuple_size<E1>
template <>
class std::tuple_element<0, E1> {
public:
  using type = int;
};  // class std::tuple_element<0, E1>
template <>
class std::tuple_element<1, E1> {
public:
  using type = double;
};  // class std::tuple_element<0, E1>

struct E2 {
  int v1;
  double v2;

};  // struct M
template <>
class std::tuple_size<E2> {
public:
  static const unsigned value = 2;
};  // class std::tuple_size<E2>
template <>
class std::tuple_element<0, E2> {
public:
  using type = int;
};  // class std::tuple_element<0, E2>
template <>
class std::tuple_element<1, E2> {
public:
  using type = double;
};  // class std::tuple_element<0, E2>
template <std::size_t Idx>
auto& get(E2&);
template <>
auto& get<0>(E2& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  return e.v1;
}
template <>
auto& get<1>(E2& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  return e.v2;
}
template <std::size_t Idx>
const auto& get(const E2&);
template <>
const auto& get<0>(const E2& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  return e.v1;
}
template <>
const auto& get<1>(const E2& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  return e.v2;
}

void run_structured_binding() {
  PRINT_CURRENT_FUNCTION_NAME;
  {
    // 简单类型的结构化绑定
    const auto&& e = maybe_g();
    SAME_TYPE(const MaybeInt&&, decltype(e));

    // 结构化绑定的规则会将每个元素的类型推导为值类型，而不会保留引用或右值引用
    // 引用给到 struct MaybeInt, 而 v1, v2 是 MaybeInt 中的成员 valid 和 value，所以 decltype(v1) 是成员的类型(也就不包含引用)
    // 相当于 const MaybeInt&& e = maybe_g(); 其中 decltype(v1) 相当于 delctype(e.valid), 得到的是 valid 的类型
    const auto&& [v1, v2] = maybe_g();  // 这里会延长右值的声明周期
    SAME_TYPE(const bool, decltype(v1));
    const auto [v3, v4] = maybe_g();  // 这里会拷贝或move右值
    SAME_TYPE(const bool, decltype(v3));
  }
  {
    // 数组的结构化绑定
    double pt[3];
    auto& [x, y, z] = pt;
    x = 3.0;
    y = 4.0;
    z = 0.0;
  }
  {
    auto e = ff();  // auto 推导出 e 是数组的退化指针
    SAME_TYPE(int*, decltype(e));
    ++e[0];  // e[0] 指向 ff 中的 d[0], 等价于 ++d[0]
    ++e[1];
    // 数组在结构化绑定时不会退化为指针.
    // 先推导出返回类型为数组，然后将每个元素拷贝到这个数组
    auto [x, y] = ff();
    ++x;  // x 指向返回数组的拷贝区域, 不影响 d[0]
    ++y;
    SAME_TYPE(int, decltype(x));

    auto& [r, s] = ff();  // 这里不涉及拷贝
    ++r;  // r 指向 d[0], 等价于 ++d[0]
    ++s;
    SAME_TYPE(int, decltype(r));
    ff();

    /*
    当 r 是一个结构化绑定的变量时， decltype(r) 得到的是被绑定的成员的类型, 相当于
    struct E { int r; int s; };
    E& e = ff();  // e 是 int[2] 的引用, e.r = d[0], r.s = d[1]
    decltype(r) 就是 r 的实际类型 int, 而不是 int&
    只有 decltype(e) 才会得到引用类型(在数组不退化的情况下)，也就是 int(&)[2]
    */
    auto& e2 = ff();  // 引用不会发生退化
    SAME_TYPE(int(&)[2], decltype(e2));
  }
  {
    E1 e{1, 2.0};
    {
      auto& [v1, v2] = e;  // 使用 e.get<>
      const auto& [v3, v4] = e;  // 使用 e.get<>
    }
    {
      // 相当于
      std::tuple_element<0, E1>::type& v1 = e.get<0>();
      std::tuple_element<1, E1>::type& v2 = e.get<1>();
      const std::tuple_element<0, E1>::type& v3 = e.get<0>();
      const std::tuple_element<1, E1>::type& v4 = e.get<1>();
    }
  }
  {
    E2 e{1, 2.0};
    {
      auto& [v1, v2] = e;  // 使用 get<>
      const auto& [v3, v4] = e;  // 使用 get<>
    }
    {
      // 相当于
      std::tuple_element<0, E2>::type& v1 = get<0>(e);
      std::tuple_element<1, E2>::type& v2 = get<1>(e);
      const std::tuple_element<0, E2>::type& v3 = get<0>(e);  // e 不是 const, 这里调用的是非 const 版本
      const std::tuple_element<1, E2>::type& v4 = get<1>(const_cast<const E2&>(e));
    }
  }
  std::println();
}

// 15.10.6 泛型 lambda 表达式
// 编译器生成一个 lambda 类

// 15.11 别名模板
// 别名模板不能特化，别名只是换一个名字，特化可能会引入新的逻辑
template <typename T>
using A = T;
template <typename T>
void call_alias_template(A<T>*) {}

// template <>
// using A<int> = void
// 如果上面的可行，那么 A<int> 和 A<void> 都是 void
// 那么调用 call_alias_template(void*) 时, T 到底是 void 还是 int

// 15.12 类模板参数推导
// 通过构造时的实参推导类模板的参数类型
template <typename T1, typename T2, typename T3 = T2>
class CC {
public:
  CC(T1 x = T1{}, T2 y = T2{}, T3 z = T3{}) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class C

void run_class_template_argument_deduction() {
  PRINT_CURRENT_FUNCTION_NAME;
  CC c1(22, 44.3, "hi");  // T1 = int, T2 = double, T3 = const char*;
  CC c2(22, 44.3);  // T1 = int, T2 = double, T3 = double
  CC c3("hi", "guy");  // T1, T2, T3 = const char*
  // CC c4();  // T1, T2 未知
  // CC c5("hi");  // T2 未知

  // 所有参数必须有推导过程或默认实参来确定, 也就是说要么全部指定，要么全部推导，不能指定一些，推导另一些(函数的模板参数是可以指定一些，推导一些的)
  // CC<std::string> c10("hi", "my", "42");  // 不能推导 T2, 因为指定了 T1
  // CC<> c11(22, 44.3, 42);  // 没有指定 T1, 也没有指定 T2, 不要写 <>, 如果写了就是指定的意思，而不是推导的意思
  CC<std::string, std::string> c12("hi", "my");  // T1, T2, T3 = std::string
  std::println();
}

// 15.12.1 推导指引
// deduction guide
template <typename T>
struct SP {
  T a;
  SP(T b): a(b) {}
};  // struct SP
// 语法: 构造函数 -> 模板类类型
template <typename T> SP(T) -> SP<T>;

void run_deduction_guide() {
  PRINT_CURRENT_FUNCTION_NAME;
  SP x{12};  // T -> int; 等价于 SP<int> x{12}
  auto z = SP{12};  // T -> int, 等价于 auto z = S<int>{12}
  /*
  SP 称为占位符类类型 placeholder class type
  使用 SP 时，变量名必须跟在其后面
  */
  // SP* p = &x;  // 不允许这么写，因为 变量名并没有紧随 SP 后面
  SP<int>* p2 = &x;  // 由于 SP* 无法推导，就必须显示指定 SP<int>

  // SP s1(1), s2(2.0);  // s1 推导 T = int, s2 推导 T = double, 矛盾
}

// 15.12.2 隐式推导指引
// implicit deduction guide

// 15.12.3 其他
// 注入的类名 injected class name

// 转发引用
template <typename T>
struct Y {
  Y(const T&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Y(T&&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct Y&
void g(std::string s) {
  /*
  按照 Y(const T&), T = std::string, 需要将 std::string 转换为 const std::string
  按照 T(T&&), 根据转发引用的规则，T = std::string&, T&& = std::string&, 但一个左值引用用于构成可能造成悬空引用
  C++标准委员会决定，如果 T 是类模板参数，那么 T 推导为 std::string, 而不是 std::string&, 这样就会调用 T(const T&)
  */
  Y y = s;
}
void run_forward_reference_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::string s{"s"};
  g(s);
  std::println();
}

// 指引仅用于推导
template <typename T>
struct XA {
};  // struct XA
template <typename T>
struct YA {
  YA(const XA<T>&);
  YA(XA<T>&&);
};  // struct YA
// 推导指引并不和构造函数对应，也不考虑左值右值，只考虑类型，提供一个 XA<T> 类型用于构造 YA, 那么推导出来 YA<T>
// 然后初始化时对构造函数进行重载解析，决定调用哪个构造函数
template <typename T> YA(XA<T>) -> YA<T>;

int main() {
  run_deduction_process();
  run_character_literal();
  run_reference_collapse();
  run_forward_reference();
  run_perfect_forwarding();
  run_limitation_of_deduction();
  run_explicit_template_argument();
  run_auto();
  run_auto_for_class_member();
  run_decltype_auto();
  run_structured_binding();
  run_class_template_argument_deduction();
  run_forward_reference_2();
  return 0;
}



