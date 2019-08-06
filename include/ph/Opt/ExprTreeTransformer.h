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

#ifndef PHAETON_OPT_EXPR_TREE_TRANSFORMER_H
#define PHAETON_OPT_EXPR_TREE_TRANSFORMER_H

#include "ph/Opt/ExprTree.h"

namespace phaeton {
class ExprTreeTransformer {
public:
#define GEN_TRANSFORM_EXPR_NODE_DECL(ExprName)                                 \
  virtual void transform##ExprName##Expr(ExprName##Expr *e) = 0;

  GEN_TRANSFORM_EXPR_NODE_DECL(Add)
  GEN_TRANSFORM_EXPR_NODE_DECL(Sub)
  GEN_TRANSFORM_EXPR_NODE_DECL(Mul)
  GEN_TRANSFORM_EXPR_NODE_DECL(ScalarMul)
  GEN_TRANSFORM_EXPR_NODE_DECL(Div)
  GEN_TRANSFORM_EXPR_NODE_DECL(ScalarDiv)
  GEN_TRANSFORM_EXPR_NODE_DECL(Contraction)
  GEN_TRANSFORM_EXPR_NODE_DECL(Product)
  GEN_TRANSFORM_EXPR_NODE_DECL(Stack)
  GEN_TRANSFORM_EXPR_NODE_DECL(Transposition)
  GEN_TRANSFORM_EXPR_NODE_DECL(Identifier)

#undef GEN_TRANSFORM_EXPR_NODE_DECL
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_TREE_TRANSFORMER_H
