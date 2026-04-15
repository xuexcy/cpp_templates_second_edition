/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/14 22:48:53
# Desc   :
########################################################################
*/

#include "danger.h"

#include <print>

void clear(char* buf) {
  int k = 0;
  for (; k < Danger<void>::max; ++k) {
    buf[k] = '\0';
  }
  std::println("Danger<void>::max = {}", k);
}
