/**
########################################################################
#
# Copyright (c) 2026 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2026/04/24 15:02:19
# Desc   : 第 18 章 模板的多态
########################################################################
*/
// polymorphism

// 18.1 动多态
// 抽象基类 ABC: abstract base class
#include "cpp_utils/util.h"

#include "coord.h"
#include "dynamic_poly/dynapoly.h"
#include "dynamic_poly/dynahier.h"
void run_abstract_base_class() {
  PRINT_CURRENT_FUNCTION_NAME;
  using namespace dynamic_poly;
  Line l{Coord{0, 0}, Coord{5, 0}};
  Circle c1{{0, 0}, 1};
  Circle c2{{3, 4}, 5};
  my_draw(l);
  my_draw(c1);
  std::println("distance of c1 and c2: {}", distance(c1, c2));
  std::println("distance of l and c1: {}", distance(l, c1));

  std::vector<GeoObj*> coll;
  coll.push_back(&l);
  coll.push_back(&c1);
  coll.push_back(&c2);
  draw_elements(coll);
}

// 18.2 静多态
#include "static_poly/statichier.h"
#include "static_poly/staticpoly.h"
void run_static_polymorphism() {
  PRINT_CURRENT_FUNCTION_NAME;
  using namespace static_poly;
  Line l{Coord{0, 0}, Coord{5, 0}};
  Circle c1{{0, 0}, 1};
  Circle c2{{3, 4}, 5};
  my_draw(l);
  my_draw(c1);
  std::println("distance of c1 and c2: {}", distance(c1, c2));
  std::println("distance of l and c1: {}", distance(l, c1));

  //std::vector<GeoObj*> coll;
  //coll.push_back(&l);
  //coll.push_back(&c1);
  //coll.push_back(&c2);
  //draw_elements(coll);
}

// 18.3 动多态与静多态
/*
动多态:
  - 绑定: 接口由公共基类预先确定
  - 动态: 接口绑定在运行期完成
静多态:
  - 非绑定: 接口不是预先确定
  - 静态: 接口绑定在编译期完成
*/
// 18.4 使用概念 concept
// 约束模板参数类型
template <typename T>
concept CGeoObj = requires(T x) {
  { x.draw() } -> std::same_as<void>;
  { x.center_of_gravity() } -> std::same_as<Coord>;
};
template <typename T>
requires CGeoObj<T>
void my_draw(const T& obj) {
  obj.draw();
}
template <typename T1, typename T2>
requires CGeoObj<T1> && CGeoObj<T2>
Coord distance(const T1& x1, const T2& x2) {
  return (x1.center_of_gravity() - x2.center_of_gravity()).abs();
}

// 18.5 新形式的设计模式
// 桥接模式

// 18.6 泛型程序设计
// STL

// 18.7 后记
int main() {
  run_abstract_base_class();
  run_static_polymorphism();
  return 0;
}

