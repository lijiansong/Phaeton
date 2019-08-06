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

  // Note: the element directive may not be present in a program,
  // so 'ed == nullptr' is possible. we need to process this corner case.
  const ElemDirect *ed = p->getElem();
  if (ed)
    ed->visit(this);

  p->getStmts()->visit(this);
}

void ASTVisitor::visitExpr(const Expr *e) { e->visit(this); }

void ASTVisitor::visitFactor(const Factor *f) { f->visit(this); }

void ASTVisitor::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);
}
