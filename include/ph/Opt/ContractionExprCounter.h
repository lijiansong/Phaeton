//==--- ContractionExprCounter.h - Representation of contraction expr ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the class for ContractionExprCounter.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_CONTRACTION_EXPR_COUNTER_H
#define PHAETON_OPT_CONTRACTION_EXPR_COUNTER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTreeVisitor.h"

#include <map>
#include <string>

namespace phaeton {

/// ContractionExprCounter - This pass helps to lift contraction expressions out
/// of expression trees only if:
/// 1. there are more than one contraction in the tree, and
/// 2. the contraction to be lifted is the most deeply nested of all
/// contractions in the expression tree.
///
/// Note that this pass should be associate with Transposition operation.
class ContractionExprCounter : public ExprTreeVisitor {
public:
  ContractionExprCounter(const ExprNode *root) : Root(root) {}

  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitContractionExpr(const ContractionExpr *en) override {
    ++Counter;
    Deepest = en;
    visitChildren(en);
  }

#define GEN_VISIT_EXPR_NODE_IMPL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *en) override {      \
    visitChildren(en);                                                         \
  }

  GEN_VISIT_EXPR_NODE_IMPL(Add)
  GEN_VISIT_EXPR_NODE_IMPL(Sub)
  GEN_VISIT_EXPR_NODE_IMPL(Mul)
  GEN_VISIT_EXPR_NODE_IMPL(Div)
  GEN_VISIT_EXPR_NODE_IMPL(ScalarMul)
  GEN_VISIT_EXPR_NODE_IMPL(ScalarDiv)
  GEN_VISIT_EXPR_NODE_IMPL(Product)
  GEN_VISIT_EXPR_NODE_IMPL(Stack)
  GEN_VISIT_EXPR_NODE_IMPL(Transposition)
  GEN_VISIT_EXPR_NODE_IMPL(Identifier)

#undef GEN_VISIT_EXPR_NODE_IMPL

  virtual void run() {
    Counter = 0;
    Deepest = nullptr;
    Root->visit(this);
  }

  unsigned getCount() const { return Counter; }

  const ExprNode *getDeepest() const { return Deepest; }

private:
  const ExprNode *Root;

  unsigned Counter;
  const ExprNode *Deepest;
};

} // end namespace phaeton

#endif // PHAETON_OPT_CONTRACTION_EXPR_COUNTER_H
