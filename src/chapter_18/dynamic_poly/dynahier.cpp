/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 15:22:51
# Desc   :
########################################################################
*/

#include "dynahier.h"
#include <print>

#include "cpp_utils/util.h"

namespace dynamic_poly {
void Circle::draw() const {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("center: {}, radius: {}", center_, radius_);
}
Coord Circle::center_of_gravity() const {
  PRINT_CURRENT_FUNCTION_NAME;
  return center_;
}
void Line::draw() const {
  PRINT_CURRENT_FUNCTION_NAME;
  std::println("point_1: {}, point_2: {}", p1_, p2_);
}
Coord Line::center_of_gravity() const {
  PRINT_CURRENT_FUNCTION_NAME;
  return (p1_ + p2_) / 2;
}

}
