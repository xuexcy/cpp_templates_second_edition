/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/04 15:37:35
# Desc   : chapter_12: 深入模板基础
########################################################################
*/

#include <print>
#include "cpp_utils/util.h"

// 12.1 参数化的声明
// C++ 模板: 类模板、函数模板、变量模板、别名模板
// 其可以被定义在命名空间作用域(namespace scope)和类作用域(class scope)
// 推导指引 deduction guide

// 类模板的命名空间
template <typename T>
class Data {
public:
  static constexpr bool copyable = true;
};  // class Data

// 函数模板的命名空间
template <typename T>
void log(T x) {

}

// 变量模板的命名空间 (从 C++14 开始)
template <typename T>
T zero = 0;

// 变量模板的命名空间 (从 C++14 开始)
template <typename T>
bool data_copyable = Data<T>::copyable;

// 别名模板的命名空间
template <typename T>
using DataList = Data<T*>;


class Collection {
public:
  // 类内部成员类模板的定义
  template <typename T>
  class Node {
  };  // class Node

  // 类内部(隐式内联)
  template <typename T>
  T* alloc() {
    return nullptr;
  }

  // 成员变量模板(从 C++14 开始)
  template <typename T>
  static T zero = 0;

  // 成员别名模板
  template <typename T>
  using NodePtr = Node<T>*;
};  // class Collection


template <typename T>
class List {
public:
  // 构造函数模板禁止隐式声明默认构造函数
  List() = default;
  template <typename U>
  class Handle;

  template <typename U>
  List(const List<U>&);

  template <typename U>
  static U zero;
};  // class List

// 类外部成员类模板的定义
template <typename T>
template <typename U>
class List<T>::Handle {

};  // class List<T>::Handle

template <typename T>
template <typename U>
List<T>::List(const List<U>& b) {}

template <typename T>
template <typename U>
U List<T>::zero = 0;

// 联合模板
template <typename T>
union AllocChunk {
  T object;
  unsigned char bytes[sizeof(T)];
  AllocChunk() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // union AllocChunk

void run_union() {
  PRINT_CURRENT_FUNCTION_NAME;
  AllocChunk<int> ac;
  std::println();
}

template <typename T>
class Stack;
template <typename T>
class Array;
// 默认调用实参
template <typename T>
void report_top(const Stack<T>&, int number = 10);
template <typename T>
void fill(Array<T>&, const T& = T{}) {  // 有默认构造实参
}

class Value {
public:
  explicit Value(int) {}
};  // class Value
void init(Array<Value>& array) {
  Value zero(0);
  fill(array, zero);
  // 编译错误，Value 没有默认构造函数
  // fill(array);
}

// 类模版中的非模板成员
template <int I>
class CupBoard {
  class Shelf;
  void open();
  enum Wood : unsigned char;
  static double total_weight;

  // C++17 开始，可以使用 inline 关键字在类模板内初始化静态成员
  inline static double total_weight_2{0.0};
};  // class CupBoard

template <int I>
class CupBoard<I>::Shelf {
};  // class CupBoard<I>::Shelf
template <int I>
void CupBoard<I>::open() {}
template <int I>
enum CupBoard<I>::Wood : unsigned char {
  Maple, Cherry, Oak
};  // enum CupBoard<I>::Wood
template <int I>
double CupBoard<I>::total_weight = 0.0;

//  12.1.1 虚成员函数
// 成员函数模板不能被声明为虚函数(因为无法确定虚函数表的大小)
template <typename T>
class Dynamic {
public:
  // 正确: 一个 Dynamic<T> 的实例对应一个析构函数
  virtual ~Dynamic() = default;
  // 错误: 在给定 Dynamic<T> 的实例的情况下 copy() 的实例数目未知
  // template <typename T2>
  // virtual void copy(const T2&);
};  // class Dynamic

// 12.1.2 模板的链接
int C;
class C;  // OK: 类名字和非类名字不在同一个空间
int X;
// 编译错误: 与类类型不同，类模板不能与其他类型的实体共享名称
// Redefinition of 'X' as different kind of symbol
// template <typename T>
// class X;  // ERROR: conflict with variable X

struct S;
// template <typename T>
// class S;  // ERROR: conflict with struct S

extern "C++"
template <typename T>
void normal();  // 默认就是 C++,  extern "C++" 可以省略

// extern "C"
// template <typename T>
// void invalid();  // 模板名称有链接，但是不能有 C 链接存在

// extern "Java"
// template <typename T>
// void java_link();  // extern "Java" 不是 C++ 标准，但是有可能某些编译器支持 Java 泛型

// 模板通常具有外部链接, 但有例外:
// 1. 具有静态说明符的命名空间作用域函数模板、未命名命名空间的直接或间接成员模板(它们具有内部链接)
// 2. 未命名类的成员模板(它没有外部，也没有内部链接)

template <typename T>
void external();  // 可以链接到其他文件
// 1. 具有静态说明符的命名空间作用域函数模板
template <typename T>
static void internal();  // 无法链接到其他文件
namespace  {
  // 1. 未命名命名空间的直接或间接成员模板
  template <typename T>
  void other_internal(); // 无法链接到其他文件
}  // 一个为命名的命名空间
// 2. 未命名类的成员模板
struct  {
  template <typename T>
  void f(T);  // 没有链接
} x; // struct

// 所有实例化都具有外部链接
template <typename T>
T one = T{};

const int one_int{};  // const 具有内部链接

// 所有实例化都具有外部链接
template <typename T>
const int max_volume{11};

// 12.2 模板参数
/*
1. 类型参数
2. 非类型参数
3. 模板的模板参数
*/
// 12.2.1 类型参数
// 类型参数的作用类似与类型别名
template <typename Allocator>
class MyList {
  // 编译错误
  // class Allocator* ptr;
  // 编译错误
  // friend class Allocator;

  Allocator* ptr;
  friend Allocator;
};  // class List

// 12.2.2 非类型参数
// 在编译期或链接期确定的常量值
/*
1. 整型或枚举类型
2. 指针类型
3. 指涉成员的指针类型
4. 左值引用类型
5. std::nullptr_t
6. 包含 auto 或者 decltype(auto)的类型
*/
template <typename T,  // 类型参数
    typename T::Allocator* Allocator>  // typename 做非类型参数的前缀
class MyList2;
template <class X*>  //  指针类型的非类型参数
class Y;

// 函数类型和数组类型做非类型参数时，要把它们隐式转换为退化后的指针类型
template <int buf[5]> class Lexer;  // int buf[5] 退化后为 int* 类型，所以下面的 int* buf 做参数算作重新声明
template <int* buf> class Lexer;  // 重新声明

template <int* buf> class Lexer2;
template <int buf[5]> class Lexer2 {
public:
  Lexer2() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // class Lexer2

template <int (*)()> struct FuncWrap;
template <int fun()>
struct FuncWrap {  // 重新声明，int fun() 退化为 int(*)() 类型，所以下面的 int (*)() 做参数算作重新声明
  FuncWrap() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
};  // struct FuncWrap

int foo() { return 1; }
int buf[5] = {1, 2, 3, 4, 5};
int buf2[6] = {1, 2, 3, 4, 5, 7};
void run_decay() {
  PRINT_CURRENT_FUNCTION_NAME;
  Lexer2<buf> lexer_1;
  Lexer2<buf2> lexer_2;  // 由于发生类退化，即使时 int[6] 也可以

  FuncWrap<foo> f;

  std::println();
}

// 非类型模板参数的声明，不能有 static、 mutable 等非类型修饰符, 可以有限定符 const volatile
// 如果限定符出现在参数类型的最外层，则会被编译器忽略
template <int const length> class Buffer;  // const 出现在参数类型的最外层, 在这里不起作用
// 等价于
// template <int length> class Buffer;

// 左值引用类型的非类型参数可用于表示左值
template <int& Counter>
struct LocalIncrement {
  LocalIncrement() { Counter = Counter + 1; }
  ~LocalIncrement() { Counter = Counter - 1; }
};  // struct LocalIncrement

int N = 8;
void run_counter() {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("Counter: {}", N);
  LocalIncrement<N> c1;
  std::println("Counter: {}", N);
  LocalIncrement<N> c2;
  std::println("Counter: {}", N);
  LocalIncrement<N> c3;
  std::println("Counter: {}", N);
  std::println();
}

// 12.2.3 模板的模板参数
template <template <typename X> class C>
void f(C<int>* p);

// 不能使用 struct
// template <template <typename X> struct C>
// void f2(C<int>* p);

// 不能使用 union
// template <template <typename X> union C>
// void f3(C<int>* p);

// c++17 开始可以使用 typename 代替 class
template <template <typename X> typename C>
void f4(C<int>* p);

// 模板的模板参数形参也可以有默认实参
// template <template <typename T, size_t N = 10> class Array>
// 通常不会将模板参数的名称用到其他模板参数中，所以一般不命名该参数
template <template <typename, size_t = 10> class Array>
class MyArray {
  Array<int> arr;  // Array<int, 10>
  Array<double, 11> arr2;
};  // class MyArray

// 12.2.4 模板参数包 template parameter pack
// 在名称前引入省略号 ...
template <typename... Types>
class Tuple;
using IntTuple = Tuple<int>;
using IntCharTuple = Tuple<int>;
using EmptyTuple = Tuple<>;
// 非类型模板参数包
template <typename T, unsigned... Dimension>
class MultiArray;
using Array1 = MultiArray<int, 1>;
using Array2 = MultiArray<int, 2, 2>;

template <typename T, template <typename, typename> class... Containers>
void test_containers();

// 函数模板允许多个模板参数包，参数包后面的每个模板参数都有默认值或者可以被推导
template <typename... TestTypes, typename T>  // T 可以通过函数参数类型推导出来
void run_tests(T value);

template <unsigned... > struct Tensor;
// 多个参数包
template <unsigned... Dims1, unsigned... Dims2>
auto compose(Tensor<Dims1...>, Tensor<Dims2...>);  // Tensor 的每个维度的类型都可以被推导

// 12.2.5 默认模板实参
template <typename T1, typename T2, typename T3, typename T4 = char, typename T5 = char>
class Quintuple;
// T4, T5 已经有默认实参了
template <typename T1, typename T2, typename T3 = char, typename T4, typename T5>
class Quintuple;

// T2 没有默认实参，编译错误
// template <typename T1 = char, typename T2, typename T3, typename T4, typename T5>
// class Quintuple;

template <typename R = void, typename T>
R* addressof(T& value);  // T 可以在实例化时推导出来, R 有默认实参时也不影响 T 没有

template <typename T = void>
class MyValue;
// 重复声明默认实参，编译错误
// template <typename T = void>
// class MyValue;

template <typename T>
class F;
// 偏特化中不能使用默认实参
// template <typename T = int>
// class F<T*>;

// 参数包中不能使用默认实参
// template <typename... Ts = int>
// struct G;

template <typename T>
struct H {
  T f();
};  // struct H
// 类模板成员的类外定义不能使用默认实参
// template <typename T = int>
// T H<T>::f() {}

struct I {
  // 友元类模板的声明不能使用默认实参
  // template<typename = void> friend struct F;
  // 友元函数模板的声明不能使用默认实参
  // template <typename = void> friend void f();
  // 友元函数模板的定义可以使用默认实参（前提是没有在其他地方声明）
  template <typename = void> friend void g() {}
};  // struct I

// 12.3 模板实参
// 12.3.1 函数模板实参
template <typename T>
T max(T a, T b) {
  return b < a ? a : b;
}
// SrcT 可以被推导, DstT 不能被推导
// 因为 SrcT 出现在了函数的形参列表中，而 DstT 没有出现在函数的形参列表中
// 于是，这种不能被推导的模板形参通常出现在参数列表的开头，以便可以显示指定他们，同时可以推导剩下的模板实参
template <typename DstT, typename SrcT>
DstT implicit_cast(const SrcT& x) {
  return x;
}

// 无法指定和推导 N, 因为函数模板可以被重载, 即使指定所有参数也不足以标识单个函数
template <typename... Ts, int N>
void f(double (&)[N+1], Ts... ps);

template <typename Func, typename T>
void apply(Func ptr, T x) {
  ptr(x);
}
template <typename T> void single(T) {}
template <typename T> void multi(T) {}
template <typename T> void multi(T*) {}
void run_parameter() {
  ::max<double>(1.0, -3.0);  // 显示指定模板实参
  ::max(1.0, -3.0);  // 实参类型被隐式推导为 double
  ::max<int>(1.0, 3.0);  // 显示指定 <int>，禁止推导
  double value = implicit_cast<double>(-1.0);  // 显示指定不可被推导的实参

  apply(&single<int>, 3);
  // 显示指定参数并不足以标识单个函数
  // apply(&multi<int>, 3);  // multi<int> 不是唯一的, 无法推导出 Func 的类型
}


// 12.3.2 类型实参
// 12.3.3 类型实参
template <typename T, T nontype_param>
class J;
// 1. 一个具有非右值类型的非俩模板实参
J<int, 33>* j_1{nullptr};
// 2. 一个编译期整数(或枚举)类型的常量值
int a;
// 3. 前面有一元运算符&的外部变量或函数名称
J<int*, &a> *j_2;
void f();
void f(int);
// 3. 前面有一元运算符&的外部变量或函数名称, 这里隐式省略了 f() 前面的 &
J<void(*)(int), f>* j_3;
template <typename T> void templ_func();
J<void(), &templ_func<double>>* j_4;
struct K {
  static bool b;
  int n;
  constexpr operator int() const { return 42; }
};  // struct K
// 4. 一个指涉成员的指针常量
J<bool&, K::b>* j_5;
// 4. 一个指涉成员的指针常量
J<int K::*, &K::n>* j_6;
// 隐式转换
J<long, K{}>* j_7;

// 无法做非类型实参
// 1. 浮点数
// 2. 字符串字面量: 同一个值的两个字符串字面量可能存在不同的地方, 可以用一个额外的变量来保存字符串

// 12.3.4 模板的模板实参

// 12.3.5 实参的等价性
template <typename T, int I>
class Mix;
Mix<int, 3*3>* p1;
Mix<int, 4+5>* p2;
// p1 和 p2 用的同一个实例化的模板 Mix<int, 9>

template <int N> struct O {};
template <int M, int N> void f(O<M+N>);  // #1
template <int N, int M> void f(O<N+M>);  // #2
template <int M, int N> void f(O<N+M>);  // #3
/*
#1 和 #2 等价，相当于声明
template <int first, int second>
void f(O<first+second>);

#3 相当于声明
template <int first, int second>
void f(O<second+first>);
这与 #1/#2 的声明是不同的
但是, #3 产生了和 #1/#2 相同的结果，因为 O<M+N> 和 O<N+M> 中的表达式得到了相同的结果
某些编译器中, N + 1 + 1 与 N + 2 完全相同
是否会出现错误完全取决于编译器
*/

// 12.4 变参模板 variadic
template <typename... Types>
class Tuple {
public:
  static constexpr std::size_t length = sizeof...(Types);
};  // class Tuple

// 12.4.1 包扩展 pack expansion
template <typename... Types>
class MyTuple : public Tuple<Types...> {
};  // class MyTuple : public Tuple<Types...>

template <typename... Types>
class PtrTuple : public Tuple<Types*...> {
};  // class PtrTuple : public Tuple<Types*...>

void run_variadic_template_argument() {
  PRINT_CURRENT_FUNCTION_NAME;
  Tuple<> t0;  // 包含空的列表

  int a1[Tuple<int>::length];
  int a3[Tuple<short, int, long>::length];

  MyTuple<int, float> t2;

  std::println();
}

// 12.4.2 包扩展的实际
template <typename... Mixins>
// 1. 基类列表中
class Point : public Mixins... {
  double x, y, z;
public:
  // 2. 构造函数的基类初始化列表中
  Point() : Mixins()... {}
  template <typename Visitor>
  void visit(Visitor v) {
    // 3. 调用参数列表中
    v(static_cast<Mixins&>(*this)...);
  }
  template <typename... T>
  // 4. 在初始化列表中
  void print(T... t) {
    (std::println("{}", t), ...);
  }
  // 5. 在模板参数列表中
  template <Mixins... m>  // 非类型模板参数
  struct Inner {
  };  // struct Inner
};  // class Point : public Mixins...

struct Color {
  char red, green, blue;
};  // struct Color
struct Label {
  std::string name;
};  // struct Label

template <typename... Ts>
struct Values {
  template <Ts... Vs>
  struct Holder {
  };  // struct Holder
};  // struct Values
int i;
Values<char, int, int*>::Holder<'a', 17, &i> pointer_inner;

// 12.4.3 函数参数包 function parameter pack
template <typename... Mixins>
class Point2 : public Mixins... {
public:
  Point2(Mixins... mixins) : Mixins(mixins)... {}
};  // class Point2 : public Mixins...
Point2<Color, Label> p{{0x7F, 0, 0x7F}, {"center"}};

// 12.4.4 多重和嵌套包扩展
template <typename F, typename... Types>
void forward(F f, const Types&... values) {
  f(Types(values)...);
  /* 相当于
  f(T1(v1), f(T2(v2), f(T3(v3));
  */
}
template <typename... OuterTypes>
class Nested {
  template <typename... InnerTypes>
  void f(const InnerTypes... inner_values) {
    g(OuterTypes(InnerTypes(inner_values)...)...);
    /* 相当于
    g(
      O1(I1(v1), I2(v2), I3(v3)),
      O2(I1(v1), I2(v2), I3(v3)),
      O3(I1(v1), I2(v2), I3(v3))
    );
    */
  }
};  // class Nested

// 如果已声明的成员类型不是函数类型，实例化后缺成了函数类型，那么就会出现程序错误
template<typename T>
struct A {
    T x;  // 声明为数据成员
};

using FuncType = void();  // 函数类型

A<int> a1;     // OK，x 是 int 类型的数据成员
// A<FuncType> a2; // 错误：x 变成了函数声明，而不是数据成员

// 12.4.6  折叠表达式
constexpr bool and_all() { return true; }
template <typename T>
constexpr bool and_all(T cond) { return cond;}
template <typename T, typename... Ts>
constexpr bool and_all(T cond, Ts... conds) {
  return cond & and_all(conds...);
}
constexpr bool this_is_true = and_all(true, false, true);
// 折叠的括号()是必须的
// 二元右折叠 (pack op ... op value)
// 二元左折叠 (value op ... op pack)
template <typename... T>
bool g() {
  return and_all(T()...);
  // return T1() && T2() && ... && true;
}

// cpp17 一元折叠
// 折叠的括号()是必须的
// 一元右折叠 (pack op ...)
// 一元左折叠 ()... op pack)
/* 空折叠
1. 空 && 产生 true
2. 空 || 产生 false
3. 空逗号运算符(,) 此剧个一个 void 表达式
*/
struct BooleanSymbol {
  bool value;
};  // struct BooleanSymbol
BooleanSymbol operator||(BooleanSymbol b1, BooleanSymbol b2) {
  return BooleanSymbol(b1.value || b2.value);
}
template <typename... BTs>
void symbolic(BTs... ps) {
  BooleanSymbol result = (ps || ...);
}
template <typename... BTs>
void symbolic2(BTs... ps) {
  BooleanSymbol result = (ps || ... || BooleanSymbol(false));
}
void run_symbolic() {
  // 当参数为空时，空的 || 会产生 false, 于是和函数中的 BooleanSymbol 类型不一致，导致编译错误
  // 所以尽量不使用一元折叠
  // symbolic();
  symbolic(BooleanSymbol(), BooleanSymbol());

  symbolic2();
  symbolic2(BooleanSymbol(), BooleanSymbol());
}

// 12.5 友元
// 12.5.1 类模板的友元类
template <typename T>
class Node;
template <typename T>
class Tree {
  // 类模板的实例声明为我其他类的友元时，类模板必须是可以见的
  // 这里 Node 是可以见
  friend class Node<T>;  // Node<int> 是 Tree<int> 的友元类
  // 这里 Factory 是不可见的，但它是一个普通类，依然正确
  friend class Factory;
  // 这里 Value 是不可见的, 编译错误
  //  friend class Value<T>;
  template <typename>
  friend class Node;  // 所有的 Node 都可以是 Tree<int> 的友元类
};  // class Tree

// 模板参数作为友元: cpp11
template <typename T>
class Wrap {
  friend T;  // 如果 T 不是类型，就可以忽略它
};  // class Wrap

void run_wrap() {
  Wrap<int> w;
  Wrap<void()> w2;  // T 是 void(), 忽略友元
}

// 12.5.2 类模板的友元函数
template <typename T1, typename T2>
void combine(T1, T2);
class Mixer {
  // 函数后面跟一个<>, 则可以将函数模板的实例声明为友元，类型如果可以推导，那就不用写
  friend void combine<>(int&, int&);
  friend void combine<double&, double&>(double&, double&);
  // Friend function specialization cannot be defined
  // 友元函数特化不允许出现定义
  // friend void combine<char&, char&>(char&, char&) {}
};  // class Mixer

void multiply(void*);  // #1
template <typename T>
void multiply(T);  // #2
class Comrades {
  friend void multiply(int) {}  // 定义一个新函数
  friend void ::multiply(void*);  // 指 #1 multiply 函数
  friend void ::multiply(int);  // 指 #2 multiply<int> 函数模板实例
  friend void ::multiply<double*>(double*);  // 指 #2 multiply<double> 函数模板实例，使用 <> 后模板必须可见

  // 受限的友元不能定义
  //   friend void ::error() {}
  // 不受限的友元可以定义
  friend void no_error() {}
};  // class Comrades

// 友元模板函数
template <typename T>
class Node2 {
  Node2<T>* allocate();
};  // class N

template <typename T>
class List2 {
  friend Node2<T>* Node2<T>::allocate();
};  // class List2

// 在类模板中定义的友元函数，只有在实际使用时才实例化
template <typename T>
class Creator {
  friend void feed(Creator<T>) {}
};  // class Creator

void run_creator() {
  Creator<void> one;
  feed(one);  // 此时才会实例化 feed(Creator<void>)
  Creator<double> two;
  feed(two);
}


// 12.5.3 友元模板

int main() {
  run_union();
  run_decay();
  run_counter();
  run_parameter();
  run_wrap();
  run_creator();
  return 0;
}
