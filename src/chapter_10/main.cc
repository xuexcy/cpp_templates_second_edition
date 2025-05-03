/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/02 12:13:22
# Desc   :
########################################################################
*/

#include <iostream>
#include <string>

// 10.1 是"类模板"还是"模板类"
/*
类类型: struct, class, union
类: struct, class

类模板: 带模板参数的类, 重在模板
模板类: 由类模板生成的类，重在某个实例化后的类
*/

// 10.2 替换、实例化和特化

// 主类模板 primary template
template <typename T1, typename T2>
class MyClass {
};  // class MyClass
// 显示特化 explicit specialization
template <>
class MyClass<std::string, float> {
};  // class MyClass<std::string, float>
// 偏特化 partial specialization
template <typename T>
class MyClass<bool, T> {
};  // class MyClass<bool, T>

// 10.3 声明和定义
// declaration / definition
/*
声明： 在作用域中引入或重新引入一个名称
定义: 当声明的构造细节已知或必须分配存储空间时，声明就变成定义
*/
extern int v;  // 声明变量 v
int w;  // 定义变量 w

// 10.3.1 完整类型和非完整类型

// 非完整类型
// 1. 类类型被声明却没有定义
class C;  // C 是非完整类型
// 2. 没有指定边界的数组类型
void foo() {
  int size;
  std::cin >> size;
  int* arr = new int[size];  // arr 是非完整类型
}
// 3. 带有非完整元素类型的数组类型
extern C elems[10];  // elems 包含非完整类型 C;
extern int arr_2[];  // arr 包含非完整类型 extern int
// 4. void
// 5. 类型未定义或者枚举值未定义的枚举类型
enum Color {
  RED,
  BLUE,
};
// Color c = GREEN;
// 6. const / volatile 修饰的上述任意类型
const C* cp;  // cp 是非完整类型的指针

// 其他的都是完整类型
int arr_3[10];  // arr 包含完整类型

// 10.4 单一定义规则
// ODR: one definition rule
/*
1. 整个程序中，普通非内联函数和成员函数，以及非内联全局变量和静态数据成员只被定义一次
2. 每个编译单元(translation unit)中,类类型、模板(包括偏特化，但不包括全局特化)、内联函数和变量，最多只被定义一次
  并且不同编译单元中的定义都相同
*/

// 10.5 模板实参和模板形参
template <typename T, int N>
class ArrayInClass {
public:
  T array[N];
};  // class ArrayInClass
// 模板形参: T, N

void run_array_in_class() {
  // 模板实参(template argument): double, 10
  // 模板 id: ArrayInClass<double, 10>
  ArrayInClass<double, 10> ad;
  ad.array[0] = 1.0;
}

int main() {
  run_array_in_class();
  return 0;
}

