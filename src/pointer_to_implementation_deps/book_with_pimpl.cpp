/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 19:49:53
# Desc    :
########################################################################
*/

#include <print>
#include <string>

#include "book_with_pimpl.h"

namespace solution {
class Book::Impl {
public:
    Impl(): title_("Hail, Hydra!"), contents_("菩萨保佑") {}
    void print() {
        std::println("title: {}, contents: {}", title_, contents_);
    }
private:
    std::string title_;
    std::string contents_;
};

Book::Book(): impl_(new Book::Impl()) {}
Book::~Book() { delete impl_; }
void Book::print() {
    impl_->print();
}

}  // namespace solution
