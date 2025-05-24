/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/05/24 14:49:36
# Desc   :
########################################################################
*/

#include "call_manual_instantiation.h"
#include "cpp_utils/util.h"


void call_manual_instantiation() {
  PRINT_CURRENT_FUNCTION_NAME;
  manual_instantiation<int>();
  manual_instantiation_2<int>();
}
