/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/23 19:21:55
# Desc   :
########################################################################
*/
#pragma once

#include <string>
class Customer {
private:
  std::string name;

public:
  Customer(const std::string& n) : name(n) {}
  std::string get_name() const { return name; }
};  // class Customer

struct CustomerEq {
  bool operator()(const Customer& lhs, const Customer& rhs) const {
    return lhs.get_name() == rhs.get_name();
  }
};  // struct CustomerEq

struct CustomerHash {
  std::size_t operator()(const Customer& c) const {
    return std::hash<std::string>{}(c.get_name());
  }
};  // struct CustomerHash

template <typename... Bases>
struct Overloader : Bases... {
  // c++ 17 开始可以使用 using 来继承基类的成员函数
  using Bases::operator()...;  // 继承所有基类的 operator()
};  // struct Overloader
