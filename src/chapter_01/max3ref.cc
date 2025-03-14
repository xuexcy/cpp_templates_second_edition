/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/03/13 17:10:55
# Desc   :
########################################################################
*/

#include <cstring>

#include "cpp_utils/util.h"

template <typename T>
const T &max(const T &a, const T &b) {
  PRINT_CURRENT_FUNCTION_NAME;
  return a > b ? a : b;
}

const char *max(const char *a, const char *b) {
  PRINT_CURRENT_FUNCTION_NAME;
  return std::strcmp(a, b) > 0 ? a : b;
}

template <typename T>
const T &max(const T &a, const T &b, const T &c) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 编译器提示:
  // Returning reference to local temporary object clang(-Wreturn-stack-address)
  // In instantiation of function template specialization 'max<const char *>' requested here
  return ::max(::max(a, b), c);
}

int main() {
  // int a{7}, b{42}, c{68};
  // max<int>(a, b, c);  // max<int>(const int &, const int &, const int &)
  //  const int &tmp1 = max<int>(a, b);  // max<int>(const int &, const int &)
  //  const int &tmp2 = max<int>(tmp1, c); // max<int>(const int &, const int &)
  //  return tmp2;
  auto m1 = ::max(7, 42, 68);

  const char *s1 = "frederic";
  const char *s2 = "anica";
  const char *s3 = "lucas";
  // max<const char *>(s1, s2, s3);  // max<const char *>(const char *const &, const char *const &, const char *const &)
  //  const char *tmp1 = max(s1, s2);  // max(const char *, const char *)
  //  const char *tmp2 = max(tmp1, s3);
  //  return tmp2;
  auto m2 = ::max(s1, s2, s3);

  return 0;
}
