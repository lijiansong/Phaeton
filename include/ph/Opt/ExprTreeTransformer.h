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
#define GEN_TRANSFORMER_EXPR_NODE_DECL(Kind)                                   \
  virtual void transform##Kind##Expr(Kind##Expr *e) = 0;

  GEN_TRANSFORMER_EXPR_NODE_DECL(Add)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Sub)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Mul)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Div)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Contraction)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Product)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Stack)
  GEN_TRANSFORMER_EXPR_NODE_DECL(Identifier)
  GEN_TRANSFORMER_EXPR_NODE_DECL(ScalarMul)
  GEN_TRANSFORMER_EXPR_NODE_DECL(ScalarDiv)

#undef GEN_TRANSFORMER_EXPR_NODE_DECL
};

#endif // __EXPR_TREE_TRANSFORMER_H__
