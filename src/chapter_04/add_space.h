/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/23 15:59:18
# Desc   :
########################################################################
*/
#pragma once

#include <iostream>

template <typename T>
class AddSpace {
private:
  const T& ref;
public:
  AddSpace(const T& r): ref(r) {}
  friend std::ostream& operator<<(std::ostream& os, AddSpace<T> s) {
    return os << s.ref << ' ';
  }
};  // class AddSpace

template <typename... Args>
void fold_print(Args... args) {
  ((std::cout << "[") << ... << AddSpace(args)) << "]\n";
}
