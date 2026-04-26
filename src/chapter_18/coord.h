/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 15:25:58
# Desc   :
########################################################################
*/
#pragma once

#include <cmath>
#include <format>
#include <string>

struct Coord {
  double x{0.0};
  double y{0.0};
  Coord operator-(const Coord& other) const {
    return {x - other.x, y - other.y};
  }
  Coord operator+(const Coord& other) const {
    return {x + other.x, y + other.y};
  }
  Coord operator/(double divisor) const {
    return {x / divisor, y / divisor};
  }
  Coord abs() const {
    return {std::fabs(x), std::fabs(y)};
  }
};  // struct Coord

template <>
struct std::formatter<Coord> : std::formatter<std::string> {
  auto format(const Coord& c, format_context& ctx) const {
    return formatter<string>::format(
      std::format("{{x = {}, y = {}}}", c.x, c.y), ctx);
  }
};
