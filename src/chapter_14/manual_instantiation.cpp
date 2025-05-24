/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 15:03:08
# Desc   :
########################################################################
*/

#include "manual_instantiation.h"


template <typename T>
void manual_instantiation_2() {  // 将模板定义隐藏起来，这样用户就看不到源代码，也无法创建其他实例化
  PRINT_CURRENT_FUNCTION_NAME;
}

template void manual_instantiation<int>();  // 手动实例化
template void manual_instantiation_2<int>();  // 手动实例化
