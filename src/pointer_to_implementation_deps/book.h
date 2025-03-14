/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 22:47:27
# Desc    :
########################################################################
*/
#pragma once

#include <string>

namespace bad_case {
class Book {
public:
    Book();
    ~Book() = default;
    void print();  // 用户只关心 print
private:
    std::string contents_;
};
}  // namespace solution




