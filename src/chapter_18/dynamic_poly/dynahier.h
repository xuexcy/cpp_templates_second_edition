/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 15:18:43
# Desc   :
########################################################################
*/
#pragma once

#include "../coord.h"

namespace dynamic_poly {

class GeoObj {
 public:
  virtual void draw() const = 0;
  virtual Coord center_of_gravity() const = 0;  // 重心
  virtual ~GeoObj() = default;
};  // class GeoObj

class Circle : public GeoObj {
 public:
  Circle(const Coord& center, double radius): center_(center), radius_(radius) {}
  virtual void draw() const override;
  virtual Coord center_of_gravity() const override;
 private:
  Coord center_;
  double radius_;
};  // class Circle

class Line : public GeoObj {
 public:
  Line(const Coord& p1, const Coord& p2): p1_(p1), p2_(p2) {}
  virtual void draw() const override;
  virtual Coord center_of_gravity() const override;
 private:
  Coord p1_;
  Coord p2_;
};  // class Line

}
