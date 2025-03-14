/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/09 15:51:04
# Desc    :
########################################################################
*/
#pragma once

#include <string>

/*
namespace bad_case {
template <class T>
class BookWithTemplate {
public:
    BookWithTemplate<T>();
    ~BookWithTemplate<T>();
    void print();
    void set_extra(T t);
private:
    class Impl;
    Impl* impl_{nullptr};
};

// 模板的实现必须在 .h 中(因为编译时需要使用完整定义展开)，但是此时并没有 BookWithTemplate::Impl 的完整定义
// 所以整个 bad_case::BookWithTemplate 无法编译成功
template <class T>
BookWithTemplate<T>::BookWithTemplate(): impl_(new BookWithTemplate::Impl()) {}
template <class T>
BookWithTemplate<T>::~BookWithTemplate() { delete impl_; }
template <class T>
void BookWithTemplate<T>::print() {
    impl_->print();
}
template <class T>
void BookWithTemplate<T>::set_extra(T t) {
    impl_->set_extra(t);
}

}  // namespace bad_case
*/


namespace solution_for_template {
template <class T>
std::string to_string(const T& t) {
    return std::to_string(t);
}
class BookBase {
public:
    BookBase();
    ~BookBase();
    void print(const std::string& extra_info);
private:
    std::string format_extra();
private:
    class Impl;
    Impl* impl_{nullptr};
};

template <class T>
class BookWithTemplate : private BookBase {
public:
    void print();
    void set_extra(const T& t);
private:
    std::string format_extra();
private:
    T extra_;
};

template <class T>
void BookWithTemplate<T>::print() {
    BookBase::print(format_extra());
}
template <class T>
void BookWithTemplate<T>::set_extra(const T& t) {
    extra_ = t;
}
template <class T>
std::string BookWithTemplate<T>::format_extra() {
    return to_string(extra_);
}

}  // namespace solution_for_template
