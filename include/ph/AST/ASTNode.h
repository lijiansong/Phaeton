//===--- ASTNode.h - Classes for representing AST nodes ---------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ASTNode base class.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_AST_NODE_H
#define PHAETON_AST_AST_NODE_H

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace phaeton {

class ASTVisitor;

class ASTNode {
public:
  enum NodeType {
    NODETYPE_Program,

    NODETYPE_DeclList,
    NODETYPE_StmtList,
    NODETYPE_ExprList,

    NODETYPE_VarDecl,
    NODETYPE_TypeDecl,

    NODETYPE_ElemDirect,

    NODETYPE_Stmt,

    NODETYPE_TranspositionExpr,
    NODETYPE_ContractionExpr,
    NODETYPE_AddExpr,
    NODETYPE_SubExpr,
    NODETYPE_MulExpr,
    NODETYPE_DivExpr,
    NODETYPE_ProductExpr,
    NODETYPE_Identifier,
    NODETYPE_Integer,
    NODETYPE_BrackExpr,
    NODETYPE_ParenExpr,

    NODETYPE_NodeType_Count
  };

public:
  NodeType getNodeType() const { return NdType; };

  virtual void _delete() const = 0;

  virtual void dump(unsigned indent = 0) const = 0;

  virtual void visit(ASTVisitor *v) const = 0;

protected:
  ASTNode(NodeType nt) : NdType(nt) {}
  virtual ~ASTNode() {}
  static std::map<NodeType, std::string> NodeLabel;

private:
  NodeType NdType;
};

template <typename T, ASTNode::NodeType nt, typename Derived>
class ASTNodeList : public ASTNode {
public:
  using Container = std::vector<T *>;

  ASTNodeList() : ASTNode(nt) {}
  ASTNodeList(T *);
  virtual ~ASTNodeList() {}

  void append(T *t) { elements.push_back(t); }

  int size() const { return elements.size(); }

  T *operator[](int i) const { return elements.at(i); }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Derived *create() { return new Derived(); }

  static Derived *create(T *t) { return new Derived(t); }

  static Derived *append(Derived *l, T *t) {
    l->append(t);
    return l;
  }

  typename Container::const_iterator begin() const { return elements.begin(); }
  typename Container::const_iterator end() const { return elements.end(); }

  virtual void visit(ASTVisitor *v) const override {}

private:
  Container elements;
};

class Expr;
class ExprList
    : public ASTNodeList<const Expr, ASTNode::NODETYPE_ExprList, ExprList> {
public:
  ExprList()
      : ASTNodeList<const Expr, ASTNode::NODETYPE_ExprList, ExprList>() {}
  ExprList(const Expr *e) : ASTNodeList(e) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Stmt;
class StmtList
    : public ASTNodeList<const Stmt, ASTNode::NODETYPE_StmtList, StmtList> {
public:
  StmtList()
      : ASTNodeList<const Stmt, ASTNode::NODETYPE_StmtList, StmtList>() {}
  StmtList(const Stmt *s) : ASTNodeList(s) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Decl;
class DeclList
    : public ASTNodeList<const Decl, ASTNode::NODETYPE_DeclList, DeclList> {
public:
  DeclList()
      : ASTNodeList<const Decl, ASTNode::NODETYPE_DeclList, DeclList>() {}
  DeclList(const Decl *d) : ASTNodeList(d) {}

  virtual void visit(ASTVisitor *v) const override;
};

} // end namespace phaeton

#endif // PHAETON_AST_AST_NODE_H
