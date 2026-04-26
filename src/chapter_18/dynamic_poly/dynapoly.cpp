/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 15:33:56
# Desc   :
########################################################################
*/

#include "../coord.h"
#include "dynapoly.h"
#include "dynahier.h"

namespace dynamic_poly {
void my_draw(const GeoObj &obj) {
  obj.draw();
}

Coord distance(const GeoObj &x1, const GeoObj &x2) {
  Coord c = x1.center_of_gravity() - x2.center_of_gravity();
  return c.abs();
}

void draw_elements(const std::vector<GeoObj*>& elements) {
  for (auto elem : elements) {
    elem->draw();
  }
}

}  // namespace dynamic
