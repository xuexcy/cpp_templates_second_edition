/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 14:46:30
# Desc   :
########################################################################
*/
/*
编译单元 1
1. 显示声明模板, 禁止在这个编译单元进行实例化
2. 显示声明模板, 以便函数 call_manual_instantiation() 调用
*/

#pragma once

template <typename T> void manual_instantiation(); // 显示声明, 但不定义, 禁止在这个编译单元进行实例化
template <typename T> void manual_instantiation_2();

void call_manual_instantiation();
