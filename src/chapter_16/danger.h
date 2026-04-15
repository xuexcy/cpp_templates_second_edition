/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/14 15:50:34
# Desc   :
########################################################################
*/
#pragma once

template <typename T>
class Danger;

template <>
class Danger<void> {
 public:
  enum { max = 100 };
};  // class Danger<void>

void clear(char* buf);
