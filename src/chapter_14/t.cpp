/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 15:28:08
# Desc   :
########################################################################
*/

#include "t.h"

template <typename T>
void t_function2() {}

template void t_function<int>();  // 在这里产生定义
template void t_function<float>();
template void t_function2<int>();
