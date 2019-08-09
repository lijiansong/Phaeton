//==---- ExprTreeVisitor.h - Representation of expr tree visitor -----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression tree visitor.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_OPT_EXPR_TREE_VISITOR_H
#define PHAETON_OPT_EXPR_TREE_VISITOR_H

#include "ph/Opt/TensorExprTree.h"

namespace phaeton {

/// ExprTreeVisitor - This class defines the common visitor interface for
/// expression tree over each expression node.
class ExprTreeVisitor {
public:
#define GEN_VISIT_EXPR_NODE_DECL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *E) = 0;

  GEN_VISIT_EXPR_NODE_DECL(Add)
  GEN_VISIT_EXPR_NODE_DECL(Sub)
  GEN_VISIT_EXPR_NODE_DECL(Mul)
  GEN_VISIT_EXPR_NODE_DECL(ScalarMul)
  GEN_VISIT_EXPR_NODE_DECL(Div)
  GEN_VISIT_EXPR_NODE_DECL(ScalarDiv)
  GEN_VISIT_EXPR_NODE_DECL(Contraction)
  GEN_VISIT_EXPR_NODE_DECL(Product)
  GEN_VISIT_EXPR_NODE_DECL(Stack)
  GEN_VISIT_EXPR_NODE_DECL(Transposition)
  GEN_VISIT_EXPR_NODE_DECL(Identifier)

#undef GEN_VISIT_EXPR_NODE_DECL
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_TREE_VISITOR_H
