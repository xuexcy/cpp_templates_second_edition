/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 16:43:55
# Desc   :
########################################################################
*/
#pragma once

#include "../coord.h"

namespace static_poly {
class Circle {
 public:
  Circle(const Coord& center, double radius): center_(center), radius_(radius) {}
  void draw() const;
  Coord center_of_gravity() const;
 private:
  Coord center_;
  double radius_;
};  // class Circle

class Line {
 public:
  Line(const Coord& p1, const Coord& p2): p1_(p1), p2_(p2) {}
  void draw() const;
  Coord center_of_gravity() const;
 private:
  Coord p1_;
  Coord p2_;
};  // class Line
}
