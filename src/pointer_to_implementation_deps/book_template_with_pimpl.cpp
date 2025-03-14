/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/09 15:53:51
# Desc    :
########################################################################
*/

#include <print>
#include <string>

#include "book_template_with_pimpl.h"
/*
namespace bad_case {
template <class T>
class BookWithTemplate<T>::Impl {
public:
    Impl(): title_("Hail, Hydra!"), contents_("菩萨保佑") {}
    void print() {
        std::println(
            "title: {}, contents: {}, sizeof<extra>: {}, extra: {}",
             title_, contents_, sizeof(T), extra_);
    }
    void set_extra(T t) { extra_ = t; }
private:
    std::string title_;
    std::string contents_;
    T extra_;
};

// template <class T>
// BookWithTemplate<T>::BookWithTemplate(): impl_(new BookWithTemplate::Impl()) {}
// template <class T>
// BookWithTemplate<T>::~BookWithTemplate() { delete impl_; }
// template <class T>
// void BookWithTemplate<T>::print() {
//     impl_->print();
// }

}  // bad_case
*/

namespace solution_for_template {
class BookBase::Impl {
public:
    Impl(): title_("Hail, Hydra!"), contents_("菩萨保佑") {}
    void print(const std::string& extra_info) {
        std::println(
            "title: {}, contents: {}, extra_info: {}",
            title_, contents_, extra_info);
    }
private:
    std::string title_;
    std::string contents_;
};
BookBase::BookBase(): impl_(new Impl) {}
BookBase::~BookBase() { delete impl_; }
void BookBase::print(const std::string& extra_info) {
    impl_->print(extra_info);
}
}

