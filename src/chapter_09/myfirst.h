/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/05/01 15:57:13
# Desc   :
########################################################################
*/
#pragma once

#include <print>
#include <typeinfo>

// 模板声明
template <typename T>
void print_type_of(const T&);

// 模板实现/定义
template <typename T>
void print_type_of(const T&) {
  std::println("{}", typeid(T).name());
}
