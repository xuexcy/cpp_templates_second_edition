/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/23 15:20:36
# Desc   :
########################################################################
*/
#pragma once

#include <type_traits>
#include "cpp_utils/util.h"
struct Node {
  int value;
  Node* left{nullptr};
  Node* right{nullptr};
  Node(int i = 0): value(i) {}
};  // struct Node

// Node::*:  指向 Node 成员的指针
// Node* Node::*: 指向 Node 成员的指针，这个成员的类型是 Node*
static Node* Node::* const NodeLeft = &Node::left;
static const auto kNodeLeft = &Node::left;
static_assert(std::is_same_v<decltype(NodeLeft), decltype(kNodeLeft)>);

static const auto kNodeRight= &Node::right;

// Node* node = traverse(root, kNodeLeft, kNodeRight);
//
//    |
//    |
//    V
//
// Node* node = root->left->right;

template <typename... TP>
Node* traverse(Node* node, TP... paths) {
  // (init   op     ...     op pack)
  // (node   ->*    ...     ->* paths)
  // (((node ->* path1)  ->* path2) ... ->* pathN)
  PRINT_CURRENT_FUNCTION_NAME;
  // 带初始化器的折叠表达式
  return (node->* ... ->* paths);
}



