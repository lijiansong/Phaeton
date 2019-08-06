//==--- ParentFinder.h - Representation of CodeGen for ParentFinder --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the class for ParentFinder.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_PARENT_FINDER_H
#define PHAETON_OPT_PARENT_FINDER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTree.h"

namespace phaeton {

/// ParentFinder - This pass can help to lift out expressions below
/// contractions. It may be helpful for testcase 'inverse Helmholtz';
/// FIXME: whether this pass is generally a useful trnasformation for all
/// testcases remains to be determined.
///
/// Note: there has also been a fix in the code that fuses two subsequent
/// assignments, i.e. definitions of local variables for the second assignment
/// are now emitted(code generated) before the first assignment. Note that this
/// may not work in general, i.e. if more than two assignments are fused.
class ParentFinder : public ExprTreeVisitor {
public:
  ParentFinder(const ExprNode *root) : Root(root) {}

  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++) {
      Parent = en;
      en->getChild(i)->visit(this);
      if (Found)
        return;
    }
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    if (en == NodeToFind) {
      Found = true;
    }
  }

#define GEN_VISIT_EXPR_NODE_IMPL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *en) override {      \
    if (en == NodeToFind) {                                                    \
      Found = true;                                                            \
      return;                                                                  \
    }                                                                          \
    visitChildren(en);                                                         \
  }

  GEN_VISIT_EXPR_NODE_IMPL(Add)
  GEN_VISIT_EXPR_NODE_IMPL(Sub)
  GEN_VISIT_EXPR_NODE_IMPL(Mul)
  GEN_VISIT_EXPR_NODE_IMPL(ScalarMul)
  GEN_VISIT_EXPR_NODE_IMPL(Div)
  GEN_VISIT_EXPR_NODE_IMPL(ScalarDiv)
  GEN_VISIT_EXPR_NODE_IMPL(Product)
  GEN_VISIT_EXPR_NODE_IMPL(Stack)
  GEN_VISIT_EXPR_NODE_IMPL(Transposition)
  GEN_VISIT_EXPR_NODE_IMPL(Contraction)

#undef GEN_VISIT_EXPR_NODE_IMPL

  virtual const ExprNode *find(const ExprNode *nodeToFind) {
    NodeToFind = nodeToFind;
    Parent = nullptr;
    Found = false;
    Root->visit(this);
    return Parent;
  }

private:
  const ExprNode *Root;
  const ExprNode *NodeToFind;
  const ExprNode *Parent;
  bool Found;
};

} // end namespace phaeton

#endif // PHAETON_OPT_PARENT_FINDER_H
