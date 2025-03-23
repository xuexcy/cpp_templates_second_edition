/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/21 15:25:12
# Desc   :
########################################################################
*/
#include <array>

template <typename T, auto MaxSize>
class Stack {
public:
  using size_type = decltype(MaxSize);
private:
  std::array<T, MaxSize> elems;
  size_type numElems{0};
public:
  Stack() = default;
  size_type size() const { return numElems; }

};  // class Stack
