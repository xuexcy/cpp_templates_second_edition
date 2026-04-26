/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 16:31:56
# Desc   :
########################################################################
*/
#pragma once

#include "../coord.h"
namespace static_poly {

template <typename GeoObj>
void my_draw(const GeoObj& obj) {
  obj.draw();
}
template <typename GeoObj1, typename GeoObj2>
Coord distance(const GeoObj1& x1, const GeoObj2& x2) {
  Coord c = x1.center_of_gravity() - x2.center_of_gravity();
  return c.abs();
}

}  // namespace static_poly
