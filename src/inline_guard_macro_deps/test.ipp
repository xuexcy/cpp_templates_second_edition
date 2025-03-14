/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2024/12/31 15:54:42
# Desc   :
########################################################################
*/
#include "cpp_utils/util.h"

INLINE void Foo::fun() {
  PRINT_CURRENT_FUNCTION_NAME;
  #ifdef ENABLE_INLINE
  std::println("inline");
  #endif
  #ifdef ENABLE_PRINT_HI
  std::println("hi");
  #endif
}
