//==------ ExprTreeLifter.cpp - Interfaces to expression tree lifter -------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of ExprTreeLifter.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/ExprTreeLifter.h"

void ExprTreeLifter::transformNode(ExprNode *en) {
  if (isNodeToBeLifted(en))
    liftNode(en);
  else
    transformChildren(en);
}

void ExprTreeLifter::liftNode(ExprNode *en) {
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  if (parent == nullptr) {
    // ExprNode 'en' is at the top of the tree, nothing to do here but visit
    // children
    setParent(en);
    transformChildren(en);
    setParent(nullptr);
  } else {
    const std::string temp = getTemp();
    ExprNode *newNode =
        getExprNodeBuilder()->createIdentifierExpr(temp, en->getDims());

    parent->setChild(childIndex, newNode);

    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(en);

    Assignments.push_back({temp, en});
  }

  setChildIndex(childIndex);
  setParent(parent);
}

void ExprTreeLifter::transformChildren(ExprNode *en) {
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  setParent(en);
  for (int i = 0; i < en->getNumChildren(); i++) {
    setChildIndex(i);
    en->getChild(i)->transform(this);
  }

  setChildIndex(childIndex);
  setParent(parent);
}

#define GEN_TRANSFORM_EXPR_NODE_IMPL(Kind)                                     \
  void ExprTreeLifter::transform##Kind##Expr(Kind##Expr *en) {                 \
    transformNode(en);                                                         \
  }

GEN_TRANSFORM_EXPR_NODE_IMPL(Add)
GEN_TRANSFORM_EXPR_NODE_IMPL(Sub)
GEN_TRANSFORM_EXPR_NODE_IMPL(Mul)
GEN_TRANSFORM_EXPR_NODE_IMPL(Div)
GEN_TRANSFORM_EXPR_NODE_IMPL(Contraction)
GEN_TRANSFORM_EXPR_NODE_IMPL(Product)
GEN_TRANSFORM_EXPR_NODE_IMPL(Stack)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarMul)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarDiv)

#undef GEN_TRANSFORM_EXPR_NODE_IMPL

void ExprTreeLifter::transformIdentifierExpr(IdentifierExpr *en) { return; }
