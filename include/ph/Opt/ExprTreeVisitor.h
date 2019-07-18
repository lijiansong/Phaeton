//==---- ExprTreeVisitor.h - Representation of expr tree visitor -----------==//
//
// This file defines interfaces of expression tree visitor.
//
//===----------------------------------------------------------------------===//
#ifndef __EXPR_TREE_VISITOR_H__
#define __EXPR_TREE_VISITOR_H__

#include "ph/Opt/ExprTree.h"

class ExprTreeVisitor {

public:
#define DECL_VISIT_EXPR_NODE(Kind)                                             \
  virtual void visit##Kind##Expr(const Kind##Expr *e) = 0;

  DECL_VISIT_EXPR_NODE(Add)
  DECL_VISIT_EXPR_NODE(Sub)
  DECL_VISIT_EXPR_NODE(Mul)
  DECL_VISIT_EXPR_NODE(Div)
  DECL_VISIT_EXPR_NODE(Contraction)
  DECL_VISIT_EXPR_NODE(Product)
  DECL_VISIT_EXPR_NODE(Stack)
  DECL_VISIT_EXPR_NODE(Identifier)

#undef DECL_VISIT_EXPR_NODE
};

#endif // __EXPR_TREE_VISITOR_H__
