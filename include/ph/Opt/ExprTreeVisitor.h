//==---- ExprTreeVisitor.h - Representation of expr tree visitor -----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression tree visitor.
//
//===----------------------------------------------------------------------===//
#ifndef __EXPR_TREE_VISITOR_H__
#define __EXPR_TREE_VISITOR_H__

#include "ph/Opt/ExprTree.h"

class ExprTreeVisitor {

public:
#define GEN_VISIT_EXPR_NODE_DECL(Kind)                                         \
  virtual void visit##Kind##Expr(const Kind##Expr *e) = 0;

  GEN_VISIT_EXPR_NODE_DECL(Add)
  GEN_VISIT_EXPR_NODE_DECL(Sub)
  GEN_VISIT_EXPR_NODE_DECL(Mul)
  GEN_VISIT_EXPR_NODE_DECL(Div)
  GEN_VISIT_EXPR_NODE_DECL(Contraction)
  GEN_VISIT_EXPR_NODE_DECL(Product)
  GEN_VISIT_EXPR_NODE_DECL(Stack)
  GEN_VISIT_EXPR_NODE_DECL(Identifier)
  GEN_VISIT_EXPR_NODE_DECL(ScalarMul)
  GEN_VISIT_EXPR_NODE_DECL(ScalarDiv)

#undef GEN_VISIT_EXPR_NODE_DECL
};

#endif // __EXPR_TREE_VISITOR_H__
