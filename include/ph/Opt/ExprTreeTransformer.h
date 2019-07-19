//==---- ExprTreeTransformer.h - Representation of expr tree transformer ---==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression tree transormer. Expression tree
// transformer doing some transformation over tensor expression nodes, which
// can help generate better code.
//
//===----------------------------------------------------------------------===//

#ifndef __EXPR_TREE_TRANSFORMER_H__
#define __EXPR_TREE_TRANSFORMER_H__

#include "ph/Opt/ExprTree.h"

class ExprTreeTransformer {
public:
#define DECL_TRANSFORM_EXPR_NODE(Kind)                                         \
  virtual void transform##Kind##Expr(Kind##Expr *e) = 0;

  DECL_TRANSFORM_EXPR_NODE(Add)
  DECL_TRANSFORM_EXPR_NODE(Sub)
  DECL_TRANSFORM_EXPR_NODE(Mul)
  DECL_TRANSFORM_EXPR_NODE(Div)
  DECL_TRANSFORM_EXPR_NODE(Contraction)
  DECL_TRANSFORM_EXPR_NODE(Product)
  DECL_TRANSFORM_EXPR_NODE(Stack)
  DECL_TRANSFORM_EXPR_NODE(Identifier)

#undef DECL_TRANSFORM_EXPR_NODE
};

#endif // __EXPR_TREE_TRANSFORMER_H__
