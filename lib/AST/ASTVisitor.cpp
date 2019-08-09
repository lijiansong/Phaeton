//===--- ASTVisitor.cpp - Visitors for AST nodes --------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements visitor of the ASTVisitor class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/ASTVisitor.h"
#include "ph/AST/ASTUtils.h"

using namespace phaeton;

template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
void ASTVisitor::visitASTNodeList(const ASTNodeList<T, NK, Derived> *List) {
  for (const auto &I : *List)
    I->visit(this);
}

#define DEF_VISIT_LIST(Derived)                                                \
  void ASTVisitor::visit##Derived(const Derived *List) {                       \
    visitASTNodeList(List);                                                    \
  }

DEF_VISIT_LIST(DeclList)
DEF_VISIT_LIST(StmtList)
DEF_VISIT_LIST(ExprList)

void ASTVisitor::visitProgram(const Program *Prog) {
  Prog->getDecls()->visit(this);

  // Note: the element directive may not be present in a program,
  // so 'ed == nullptr' is possible. we need to process this corner case.
  const ElementDirective *ED = Prog->getElem();
  if (ED) {
    ED->visit(this);
  }

  Prog->getStmts()->visit(this);
}

void ASTVisitor::visitExpr(const Expr *E) { E->visit(this); }

void ASTVisitor::visitFactor(const Factor *F) { F->visit(this); }

void ASTVisitor::visitParenExpr(const ParenExpr *PE) {
  PE->getExpr()->visit(this);
}
