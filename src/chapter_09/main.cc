/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/01 15:44:42
# Desc   : chapter_09: 在实践中使用模板
########################################################################
*/


#include "myfirst.h"

// 9.1 包含模型简介
/*
模板在头文件中声明后，需要在同一个头文件中进行定义，否则其他编译单元在进行编译后将出现链接错误
当我们 #include "myfirst.h" 时，会增大本文件的大小，其增大的大小还包含 myfirst.h 中
#include <print> 和 #include <typeinfo> 的大小
这样也会增加编译所需的时间
解决办法:
1. 预编译头文件
2. 模板显示实例化


另外,非内联函数模板不会在调用处展开，而是在不同的编译单元中分别实例化，对同一个函数创建不同的新副本
当链接器发现同一个函数的多个定义不同时，需要报错
*/

void run_my_first() {
  print_type_of(1);
  std::println();
}

// 9.2 模板和 inline

// 9.3 预编译头文件
/*
c++ 头文件可能会非常大，需要编译很长时间,可以通过预编译头文件的方式来优化编译
假设许多文件都以相同的 N 行代码开始
1. 编译开头的 N 行代码
2. 将编译器的完整状态保存在一个预编译头文件中
3. 编译其他程序时加载保存的状态，并从程序的第 N+1 行开始编译

找到并编译这 N 行相同的代码需要消耗大量的时间，因此，保持头文件顺序的一致性，对编译非常有利
比如
a.h
#include <vector>
#include <list>

b.h
#include <list>
#include <vector>

就会禁用预编译头文件，因为两者头文件顺序不一致

另外，可以创建一个包含所有标准头文件的头文件，比如
std.h
#include <iostream>
#include <string>
#include <vector>
#include <list>

其他文件再 #include "std.h"

*/


// 9.4 破译大篇错误信息

int main() {
  run_my_first();
  return 0;
}


