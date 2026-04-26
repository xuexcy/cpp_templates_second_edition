/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2026/04/24 15:26:55
# Desc   :
########################################################################
*/
#pragma once

#include <vector>

#include "dynahier.h"

namespace dynamic_poly {

void my_draw(const GeoObj& obj);
Coord distance(const GeoObj& x1, const GeoObj& x2);
void draw_elements(const std::vector<GeoObj*>& elements);

}
