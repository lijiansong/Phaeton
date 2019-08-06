//==--- ExprTreeLoopLifter.cpp - Interfaces to expression tree lifter ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of ExprTreeLifter.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/ExprTreeLifter.h"

using namespace phaeton;

void ExprTreeLifter::transformAssignments() {
  for (curPos = Assignments.begin(); curPos != Assignments.end(); curPos++) {
    ExprNode *rhs = curPos->rhs;

    setRoot(rhs);
    setParent(nullptr);
    setChildIndex(-1);
    rhs->transform(this);
  }
}

void ExprTreeLifter::transformNode(ExprNode *en) {
  if (isNodeToBeLifted(en, getRoot()))
    liftNode(en);
  else
    transformChildren(en);
}

void ExprTreeLifter::liftNode(ExprNode *en) {
  ExprNode *root = getRoot();
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  if (parent == nullptr) {
    // ExprNode 'en' is at the top of the tree,
    // nothing to do here but visit children
    setParent(en);
    transformChildren(en);
    setParent(nullptr);
  } else {
    const std::string temp = getTempWithDims(en->getDims());
    ExprNode *newNode =
        getENBuilder()->createIdentifierExpr(temp, en->getDims());
    // Replace sub-expression 'en' which is to be lifted with the identifier
    // 'temp'
    parent->setChild(childIndex, newNode);
    // Recursively lift expressions out of the sub-expression ExprNode 'en'.
    setRoot(en);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(en);

    // Add a new assignment that assigns the lifted sub-expression node 'en'
    // to the new identifier 'temp'.
    ExprNode *newLHS =
        getENBuilder()->createIdentifierExpr(temp, en->getDims());
    // The new assignment is inserted before 'curPos' on purpose.
    Assignments.insert(curPos, {newLHS, en});

    // Since the ExprNode 'en' has been lifted, the expression tree
    // rooted at 'root' must be re-visited.
    setRoot(root);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(root);

    freeTempWithDims(temp, en->getDims());
  }

  setChildIndex(childIndex);
  setParent(parent);
  setRoot(root);
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

#define GEN_TRANSFORM_EXPR_NODE_IMPL(ExprName)                                 \
  void ExprTreeLifter::transform##ExprName##Expr(ExprName##Expr *en) {         \
    transformNode(en);                                                         \
  }

GEN_TRANSFORM_EXPR_NODE_IMPL(Add)
GEN_TRANSFORM_EXPR_NODE_IMPL(Sub)
GEN_TRANSFORM_EXPR_NODE_IMPL(Mul)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarMul)
GEN_TRANSFORM_EXPR_NODE_IMPL(Div)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarDiv)
GEN_TRANSFORM_EXPR_NODE_IMPL(Contraction)
GEN_TRANSFORM_EXPR_NODE_IMPL(Product)
GEN_TRANSFORM_EXPR_NODE_IMPL(Stack)
GEN_TRANSFORM_EXPR_NODE_IMPL(Transposition)

#undef GEN_TRANSFORM_EXPR_NODE_IMPL

void ExprTreeLifter::transformIdentifierExpr(IdentifierExpr *en) { return; }
