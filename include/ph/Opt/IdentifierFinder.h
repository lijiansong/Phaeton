//==--- IdentifierFinder.h - Representation of Identifier expr finder ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the class IdentifierFinder.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_ID_FINDER_H
#define PHAETON_OPT_ID_FINDER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/TensorExprTree.h"

#include <string>

namespace phaeton {

/// IdentifierFinder - This pass works on the Identifier Expr. If the same
/// identifier appears on the LHS and the RHS of an assignment, but with
/// incompatible index structures, a temporary copy of the identifier is created
/// and used on the RHS.
class IdentifierFinder : public ExprTreeVisitor {
public:
  IdentifierFinder(const ExpressionNode *root) : Root(root) {}

  void visitChildren(const ExpressionNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    if (en->getName() == NameToFind) {
      Found = true;
      // Note that if the identifiers expression seen so far are still
      // compatible, set to 'Incompatible', i.e. we have to make sure that
      // 'IdIncompatible' is never unset once it has been set to 'true'.
      if (IdIncompatible == false)
        IdIncompatible = Incompatible;
    }
  }

  bool getIdIncompatible() const { return IdIncompatible; }

#define GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(ExprName)                          \
  virtual void visit##ExprName##Expr(const ExprName##Expr *en) override {      \
    visitChildren(en);                                                         \
  }

  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(Add)
  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(Sub)
  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(Mul)
  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(ScalarMul)
  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(Div)
  GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL(ScalarDiv)

#undef GEN_VISIT_COMPATIBLE_EXPR_NODE_IMPL

#define GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL(ExprName)                        \
  virtual void visit##ExprName##Expr(const ExprName##Expr *en) override {      \
    bool savedIncompatible = Incompatible;                                     \
    Incompatible = true;                                                       \
    visitChildren(en);                                                         \
    Incompatible = savedIncompatible;                                          \
  }

  GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL(Product)
  GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL(Stack)
  GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL(Transposition)
  GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL(Contraction)

#undef GEN_VISIT_INCOMPATIBLE_EXPR_NODE_IMPL

  virtual bool find(const std::string nameToFind) {
    NameToFind = nameToFind;
    Found = false;
    IdIncompatible = false;
    Incompatible = false;
    Root->visit(this);
    return Found;
  }

private:
  const ExpressionNode *Root;
  bool Incompatible;

  std::string NameToFind;
  bool Found;
  bool IdIncompatible;
};

} // end namespace phaeton

#endif // PHAETON_OPT_ID_FINDER_H
