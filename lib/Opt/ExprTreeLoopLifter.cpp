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
  for (CurrentPos = Assignments.begin(); CurrentPos != Assignments.end();
       ++CurrentPos) {
    ExpressionNode *RHS = CurrentPos->RHS;

    setRoot(RHS);
    setParent(nullptr);
    setChildIndex(-1);
    RHS->transform(this);
  }
}

void ExprTreeLifter::liftNode(ExpressionNode *Node) {
  ExpressionNode *Root = getRoot();
  ExpressionNode *Parent = getParent();
  unsigned ChildIndex = getChildIndex();

  if (Parent == nullptr) {
    // Note that since ExpressionNode 'Node' is at the top of the tree,
    // nothing to do here but visit children
    setParent(Node);
    transformChildren(Node);
    setParent(nullptr);
  } else {
    const std::string Tmp = getTmpWithDims(Node->getDims());
    ExpressionNode *NewNode =
        getExprNodeBuilder()->createIdentifierExpr(Tmp, Node->getDims());
    // Replace sub-expression ExpressionNode 'Node' which is to be lifted with
    // the identifier 'Tmp'
    Parent->setChild(ChildIndex, NewNode);
    // Recursively lift expressions out of the sub-expression ExpressionNode
    // 'Node'.
    setRoot(Node);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(Node);

    // Add a new assignment that assigns the lifted sub-expression node 'Node'
    // to the new identifier 'Tmp'.
    ExpressionNode *NewLHS =
        getExprNodeBuilder()->createIdentifierExpr(Tmp, Node->getDims());
    // The new assignment is inserted before 'CurrentPos' on purpose.
    Assignments.insert(CurrentPos, {NewLHS, Node});

    // Since the ExpressionNode 'Node' has been lifted, the expression tree
    // rooted at 'Root' must be re-visited.
    setRoot(Root);
    setParent(nullptr);
    setChildIndex(-1);
    transformChildren(Root);

    freeTmpWithDims(Tmp, Node->getDims());
  }

  setChildIndex(ChildIndex);
  setParent(Parent);
  setRoot(Root);
}

void ExprTreeLifter::transformNode(ExpressionNode *Node) {
  if (IsNodeToBeLifted(Node, getRoot())) {
    liftNode(Node);
  } else {
    transformChildren(Node);
  }
}

void ExprTreeLifter::transformChildren(ExpressionNode *Node) {
  ExpressionNode *Parent = getParent();
  unsigned ChildIndex = getChildIndex();

  setParent(Node);
  for (int I = 0; I < Node->getNumChildren(); ++I) {
    setChildIndex(I);
    Node->getChild(I)->transform(this);
  }

  setChildIndex(ChildIndex);
  setParent(Parent);
}

#define GEN_TRANSFORM_EXPR_NODE_IMPL(ExprName)                                 \
  void ExprTreeLifter::transform##ExprName##Expr(ExprName##Expr *E) {          \
    transformNode(E);                                                          \
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

void ExprTreeLifter::transformIdentifierExpr(IdentifierExpr *E) { return; }
