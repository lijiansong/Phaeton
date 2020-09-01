//===--- ASTVisitor.h - Visitor for AST nodes -------------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ASTVisitor interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_ASTVISITOR_H
#define PHAETON_AST_ASTVISITOR_H

#include "ph/AST/ASTNode.h"
#include "ph/AST/Decl.h"
#include "ph/AST/ElementDirective.h"
#include "ph/AST/Expr.h"
#include "ph/AST/Program.h"
#include "ph/AST/Stmt.h"

namespace phaeton {

class ASTVisitor {
public:
  virtual void visitProgram(const Program *);

  template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
  void visitASTNodeList(const ASTNodeList<T, NK, Derived> *List);

#define GEN_VISIT_LIST_DECL(Derived)                                           \
  virtual void visit##Derived(const Derived *List);

  GEN_VISIT_LIST_DECL(DeclList)
  GEN_VISIT_LIST_DECL(StmtList)
  GEN_VISIT_LIST_DECL(ExprList)

#undef GEN_VISIT_LIST_DECL

#define GEN_PURE_VISIT_ASTNODE_DECL(NodeName)                                  \
  virtual void visit##NodeName(const NodeName *) = 0;

  GEN_PURE_VISIT_ASTNODE_DECL(Decl)
  GEN_PURE_VISIT_ASTNODE_DECL(Stmt)
  GEN_PURE_VISIT_ASTNODE_DECL(BinaryExpr)
  GEN_PURE_VISIT_ASTNODE_DECL(Identifier)
  GEN_PURE_VISIT_ASTNODE_DECL(Integer)
  GEN_PURE_VISIT_ASTNODE_DECL(BrackExpr)
  GEN_PURE_VISIT_ASTNODE_DECL(ElementDirective)

#undef GEN_PURE_VISIT_ASTNODE_DECL

#define GEN_VISIT_ASTNODE_DECL(NodeName)                                       \
  virtual void visit##NodeName(const NodeName *);

  GEN_VISIT_ASTNODE_DECL(Expression)
  GEN_VISIT_ASTNODE_DECL(Factor)
  GEN_VISIT_ASTNODE_DECL(ParenExpr)

#undef GEN_VISIT_ASTNODE_DECL
};

} // end namespace phaeton

#endif // PHAETON_AST_ASTVISITOR_H
