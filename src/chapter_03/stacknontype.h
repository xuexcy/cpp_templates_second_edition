/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/21 13:38:11
# Desc   :
########################################################################
*/

#include <array>
#include <cassert>
#include <cstddef>
#include <print>

namespace stacknontype {
template <typename T, std::size_t MaxSize>
class Stack {
public:
  Stack() = default;
  void push(const T& elem);
  void pop();
  const T& top() const;
  bool empty() const {
    return numElems == 0;
  }
  std::size_t size() const {
    return numElems;
  }
private:
  std::array<T, MaxSize> elems;
  std::size_t numElems = 0;
};  // class Stack

template <typename T, std::size_t MaxSize>
void Stack<T, MaxSize>::push(const T& elem) {
  assert(numElems < MaxSize);
  elems[numElems] = elem;
  ++numElems;
}

template <typename T, std::size_t MaxSize>
void Stack<T, MaxSize>::pop() {
  assert(!empty());
  --numElems;
}
template <typename T, std::size_t MaxSize>
const T& Stack<T, MaxSize>::top() const {
  assert(!empty());
  return elems[numElems - 1];
}

}  // namespace stacknontype
