/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 15:26:45
# Desc   :
########################################################################
*/
#pragma once

template <typename T>
void t_function() {}

template <typename T>
void t_function2();

// 使用了 extern template 就会禁止常用特化的自动实例化
extern template void t_function<int>();  // 声明但没有定义
extern template void t_function<double>();  // 声明但没有定义
extern template void t_function2<int>();
