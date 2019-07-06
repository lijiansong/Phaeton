#ifndef __ASTVISITOR_H__
#define __ASTVISITOR_H__

#include "tir/AST/ASTNode.h"
#include "tir/AST/Decl.h"
#include "tir/AST/Expr.h"
#include "tir/AST/Stmt.h"
#include "tir/AST/Program.h"

class ASTVisitor {
public:
  virtual void visitProgram(const Program *);

  template <typename T, ASTNode::NodeType nt, typename Derived>
  void visitASTNodeList(const ASTNodeList<T, nt, Derived> *list);

#define DECL_VISIT_LIST(Derived)                                               \
  virtual void visit##Derived(const Derived *list);

  DECL_VISIT_LIST(DeclList)
  DECL_VISIT_LIST(StmtList)
  DECL_VISIT_LIST(ExprList)

#define DECL_PURE_VISIT_ASTNode(NodeName)                                               \
  virtual void visit##NodeName(const NodeName *) = 0;

  DECL_PURE_VISIT_ASTNode(Decl)
  DECL_PURE_VISIT_ASTNode(Stmt)

  DECL_PURE_VISIT_ASTNode(BinaryExpr)
  DECL_PURE_VISIT_ASTNode(Identifier)
  DECL_PURE_VISIT_ASTNode(Integer)
  DECL_PURE_VISIT_ASTNode(BrackExpr)

#define DECL_VISIT_ASTNode(NodeName)                                               \
  virtual void visit##NodeName(const NodeName *);

  DECL_VISIT_ASTNode(Expr)
  DECL_VISIT_ASTNode(Factor)
  DECL_VISIT_ASTNode(ParenExpr)
};

#endif // __ASTVISITOR_H__
