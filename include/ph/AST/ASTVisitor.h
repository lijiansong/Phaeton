#ifndef __ASTVISITOR_H__
#define __ASTVISITOR_H__

#include "ph/AST/ASTNode.h"
#include "ph/AST/Decl.h"
#include "ph/AST/Expr.h"
#include "ph/AST/Stmt.h"
#include "ph/AST/Program.h"

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
#undef DECL_VISIT_LIST

#define DECL_PURE_VISIT_ASTNode(NodeName)                                               \
  virtual void visit##NodeName(const NodeName *) = 0;

  DECL_PURE_VISIT_ASTNode(Decl)
  DECL_PURE_VISIT_ASTNode(Stmt)

  DECL_PURE_VISIT_ASTNode(BinaryExpr)
  DECL_PURE_VISIT_ASTNode(Identifier)
  DECL_PURE_VISIT_ASTNode(Integer)
  DECL_PURE_VISIT_ASTNode(BrackExpr)
#undef DECL_PURE_VISIT_ASTNode

#define DECL_VISIT_ASTNode(NodeName)                                               \
  virtual void visit##NodeName(const NodeName *);

  DECL_VISIT_ASTNode(Expr)
  DECL_VISIT_ASTNode(Factor)
  DECL_VISIT_ASTNode(ParenExpr)
#undef DECL_VISIT_ASTNode
};

#endif // __ASTVISITOR_H__
