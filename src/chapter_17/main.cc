/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2026/04/16 13:45:39
# Desc   : 第 17 章 未来方向
########################################################################
*/

#include "cpp_utils/util.h"

// 17.1 宽松的 typename 规则
// 17.2 广义非类型模板参数
template <const char* msg>
class Diagnoser {
 public:
  void print();
};  // class Diagnoser

template <double Ratio>
class Converter {
 public:
  static double convert(double val) {
    return val * Ratio;
  }
};  // class Converter
using InchToMeter = Converter<0.0254>;

// C++11 引入了文本类(Literal, 字面量)类型的概念: 类类型可以在编译期计算出常量值
struct Point {
  int x;
  int y;
};  // struct Point
constexpr Point add(Point a, Point b) {
  return Point(a.x + b.x, a.y + b.y);
}
// p1, p2, result 都可以在编译期完成，看起来 add
bool operator==(Point a, Point b) {
  return (a.x - a.y) == (b.x - b.y);
}

void run_generalized_non_type_template_argument() {
  PRINT_CURRENT_FUNCTION_NAME;
  // Diagnoser<"Surprise!">.print();
  // Diagnoser<"Surprise!">.print();
  /* "Surprise!" 是指针地址，两个 "Surprise!" 的指针地址可能不同(比如在两个不同的编译单元实例化),
   但我们看起来 Diagnoser<"Surprise!"> 用的是同一个模板,
   所以C++标准禁止字符串文本作为模板的实参
  */
  constexpr Point p1(1, 2);
  constexpr Point p2(2, 3);
  constexpr Point result = add(p1, p2);
  // p1, p2, result 都可以在编译期被计算



}

/* 同样的，如果两个 Point 的 x, y 相等，那这两个 Point 是否相等?
字符串在编译期是否相同需要比较 const char* 地址, 但 Point 是否相同需要使用 operator== 函数的定义。
但是链接器不能调用 operator==, 而是通过比较名称(Name Mangling)来检查等价性，
比如 Point(1, 2) 在编译后的名称是 ClassPointInt1Int2, Point(2, 3)在编译后的名称是 ClassPointInt2Int3
通过比较名字，p1, p2 是两个不同的名字，是不相等的，但是，如果用户自定义了 operator==, 那就可能 p1 == p2
如果要实现在编译期判定 p1, p2 是否相等，就需要将 operator== 函数的逻辑编译到名称中，但这太复杂了

c++20 引入规则: 只有具有“简单相等”条件的文字类，才允许作为非类型模板参数类型。
1. 类成员都是标量
2. 没有 operator==
3. 比较是逐个成员比较，和链接器的名称修饰一样
*/
struct SimplePoint {
  int x, y;
};  // class SimplePoint
struct ComplexPoint {
  int x, y;
  constexpr bool operator==(ComplexPoint other) const {
    return (x - y) == (other.x - other.y);
  }
};  // struct ComplexPoint
template <SimplePoint P>
struct Ok {};
template <ComplexPoint P>
struct No {};
constexpr SimplePoint s1{1, 2};
constexpr SimplePoint s2{2, 3};
constexpr SimplePoint s3{1,2};
constexpr ComplexPoint c1{1, 2};
constexpr ComplexPoint c2{2, 3};
constexpr ComplexPoint c3{1, 2};
Ok<s1> o1;
Ok<s2> o2;
Ok<s3> o3;
// ComplexPoint 的实例应该不能做非类型模板参数，可能和编译器实现有关
No<c1> n1;
No<c2> n2;
No<c3> n3;

SAME_TYPE(decltype(o1), decltype(o3));
SAME_TYPE(decltype(n1), decltype(n3));


// 17.3 函数模板的偏特化(不支持偏特化)
// 1. 可以特化类的成员模板，而无需改变该类的定义。但是，不能添加一个重载成员，因为这需要改变类的定义。不能向std添加新的
// 模板，但是可以特化std中的某个模板
struct Token {
  const char* begin;
  int len;
};  // struct Token
namespace std {
template <>
struct hash<Token> {
  size_t operator()(const Token& token) const noexcept {
    return std::hash<const void*>()(token.begin) ^ (std::hash<int>()(token.len) << 1);
  }
};
}
// 2. 重载函数模板，函数的参数必须具备实质性的差异
template <typename T, typename R>
R convert(const T&);
// 不能通过重载来特化 R = void（函数参数没有改变）, 函数返回类型不参与重载解析
// template <typename T>
// void convert(const T&);

template <typename T>
void f1(int) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void g(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  t(1);
}
template <typename T>
void f2(int) {
  PRINT_CURRENT_FUNCTION_NAME;
}
template <typename T>
void f2(double) {
  PRINT_CURRENT_FUNCTION_NAME;
}
// 3. 合法的非重载函数的代码，在函数重载后可能就不再合法
void run_invalid_after_overload() {
  PRINT_CURRENT_FUNCTION_NAME;
  g(f1<int>);
  // g(f2<int>);  // 当有了 f2<T>(double) 这个重载后，就无法明确 f2<int> 到底是 f2<int>(int) 还是 f2<int>(double)
}

class S {
 private:
  int value{42};
 template <typename T>
 friend void foo(T);
};  // class S
template <typename T>
void foo(T t) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("{}", t.value);
}
template <typename T>
void foo(T* t) {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("{}", t->value);
}
// 4. 友元声明是指特定的函数模板或特定函数模板的实例化。函数模板的重载版本，不会自动具有赋予原始模板的那些权限
void run_overload_friend() {
  PRINT_CURRENT_FUNCTION_NAME;
  S s;
  foo(s);
  S* ptr = &s;
  // 只有 foo<T>(T) 是 S 的友元，而 foo<T>(T*) 不是
  // foo(ptr);  // 'value' is a private member of 'S'
}

void run_function_template_specialized() {
  PRINT_CURRENT_FUNCTION_NAME;
  run_invalid_after_overload();
  run_overload_friend();
}

// 17.4 命名模板实参简介
template <typename A = float, typename B = int, typename C = double>
void abc() {
  PRINT_CURRENT_FUNCTION_NAME;
}
void run_named_template_argument() {
  PRINT_CURRENT_FUNCTION_NAME;
  abc<>();  // abc<float, int, double>
  abc<int>();  // abc<int, int, double>
  abc<int, double>();  // abc<int, double, double>
  abc<float, double, int>();
  // 要修改后面的模板参数，不能省略前面的默认参数
  // abc</*float, */ double, int>(); // 无法省略第一个默认的 float 参数
}

// 17.5 重载类模板
// 暂时不支持重载类模板
template <typename T> class Array;  // 动态长度的数组
// template <typename T, unsigned Size> class Array;  // 静态长度的数组。但C++不支持重载类模板
template <typename T1, typename T2> class Pair;  // 一对字段
// template <int I1, int I2> class Pair;  // 一对静态类型值。但C++不支持重载类模板

// 17.6 中间包扩展的演绎
template <typename... Types>
struct Front;
template <typename FrontT, typename... Types>
struct Front<FrontT, Types...> {
  using Type = FrontT;
};  // struct Front<FrontT, Types...>
// 只有包扩展发生在参数或实参列表的末尾，包扩展的模板实参演绎才会起作用。
// 这意味着，从一个列表中提取第一个元素很简单, 但是，不能轻易提取列表的最后一个元素
SAME_TYPE(int, (Front<int, double, float>::Type));

template <typename... Types>
struct Back;
// template <typename BackT, typename... Types>
// struct Back<Types..., BackT> {  // 包扩展只能在最后一个参数
//   using Type = BackT;
// };  // struct Back<Types..., BackT>
//

// 17.7 void 的规则化
// auto&& r = f();  // f()返回void时出错
// decltype(auto) r = f();  // f() 返回void时出错

// 17.8 模板的类型检查
// requires/concepts

// 17.9 反射元编程
// 17.10 包管理工具
// 17.11 模块
int main() {
  run_generalized_non_type_template_argument();
  run_invalid_after_overload();
  run_overload_friend();
  run_function_template_specialized();
  run_named_template_argument();

  return 0;
}



