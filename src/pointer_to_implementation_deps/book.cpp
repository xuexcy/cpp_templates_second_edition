/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 22:48:09
# Desc    :
########################################################################
*/

#include <print>

#include "book.h"


namespace bad_case {
Book::Book(): contents_("菩萨保佑") {}
void Book::print() {
    std::println("contents: {}", contents_);
}
}  // namespace bad_case


