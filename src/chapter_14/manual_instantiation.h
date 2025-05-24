/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 15:02:24
# Desc   :
########################################################################
*/
/*
编译单元 2
1. 手动实例化, 将实例化结果编译至当前单元(这样可以减少其他编译单元重复的实例化)
https://github.com/xuexcy/public_attachments/tree/main/draft/cpp/define_template_in_cpp_file
*/

#pragma once

#include "cpp_utils/util.h"

template <typename T>
void manual_instantiation() {
  PRINT_CURRENT_FUNCTION_NAME;
}

template <typename T>
void manual_instantiation_2();  // 只有一个声明，提供 .h 给用户看看有哪些接口，将定义隐藏在了 .cpp 中
