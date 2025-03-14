/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2024/12/31 15:54:00
# Desc   :
########################################################################
*/

#include "inline_guard_macro_deps/test.h"
#include "utils/main_decorator.h"
// #define DENABLE_PRINT_HI
void run() {
  Foo foo;
  foo.fun();
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}
