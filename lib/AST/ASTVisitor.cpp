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

template <typename T, ASTNode::NodeType nt, typename Derived>
void ASTVisitor::visitASTNodeList(const ASTNodeList<T, nt, Derived> *list) {
  for (const auto &i : *list)
    i->visit(this);
}

#define DEF_VISIT_LIST(Derived)                                                \
  void ASTVisitor::visit##Derived(const Derived *list) {                       \
    visitASTNodeList(list);                                                    \
  }

DEF_VISIT_LIST(DeclList)
DEF_VISIT_LIST(StmtList)
DEF_VISIT_LIST(ExprList)

void ASTVisitor::visitProgram(const Program *p) {
  p->getDecls()->visit(this);
  p->getStmts()->visit(this);
}

void ASTVisitor::visitExpr(const Expr *e) { e->visit(this); }

void ASTVisitor::visitFactor(const Factor *f) { f->visit(this); }

void ASTVisitor::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);
}
