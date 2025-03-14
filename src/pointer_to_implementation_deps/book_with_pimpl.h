/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 19:49:48
# Desc    :
########################################################################
*/
#pragma once

namespace solution {
class Book {
public:
    Book();
    ~Book();
    void print();  // 用户只关心 print
private:
    class Impl;
    Impl* impl_{nullptr};
};
}  // namespace solution
