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
#pragma once

class Foo {
public:
  void fun();
};  // class Foo

#ifdef ENABLE_INLINE
#define INLINE inline
#include "test.ipp"
#endif

