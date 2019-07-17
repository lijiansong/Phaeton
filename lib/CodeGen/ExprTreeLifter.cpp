#include "ph/CodeGen/ExprTreeLifter.h"

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
    // node 'en' is at the top of the tree, nothing to do here but visit
    // children
    setParent(en);
    transformChildren(en);
    setParent(nullptr);
  } else {
    const std::string temp = getTemp();
    ExprNode *newNode =
        getENBuilder()->createIdentifierExpr(temp, en->getDims());

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

#define DEF_TRANSFORM_EXPR_NODE(Kind)                                          \
  void ExprTreeLifter::transform##Kind##Expr(Kind##Expr *en) {                 \
    transformNode(en);                                                         \
  }

DEF_TRANSFORM_EXPR_NODE(Add)
DEF_TRANSFORM_EXPR_NODE(Sub)
DEF_TRANSFORM_EXPR_NODE(Mul)
DEF_TRANSFORM_EXPR_NODE(Div)
DEF_TRANSFORM_EXPR_NODE(Contraction)
DEF_TRANSFORM_EXPR_NODE(Product)
DEF_TRANSFORM_EXPR_NODE(Stack)

#undef DECL_TRANSFORM_EXPR_NODE

void ExprTreeLifter::transformIdentifierExpr(IdentifierExpr *en) { return; }
