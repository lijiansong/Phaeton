//===--- ASTNode.h - Classes for representing AST nodes ---------*- C++ -*-===//
//
//  This file defines the ASTNode base class.
//
//===----------------------------------------------------------------------===//

#ifndef __ASTNODE_H_
#define __ASTNODE_H_

#pragma once

#include <cassert>
#include <map>
#include <string>
#include <vector>

// namespace tir {

class ASTVisitor;

class ASTNode {
public:
  enum NodeType {
    NT_Program,

    NT_DeclList,
    NT_StmtList,
    NT_ExprList,

    NT_VarDecl,
    NT_TypeDecl,

    NT_Stmt,

    NT_ContractionExpr,
    NT_AddExpr,
    NT_SubExpr,
    NT_MulExpr,
    NT_DivExpr,
    NT_ProductExpr,
    NT_Identifier,
    NT_Integer,
    NT_BrackExpr,
    NT_ParenExpr,

    NT_NODETYPE_COUNT
  };

private:
  NodeType NdType;

protected:
  ASTNode(NodeType nt) : NdType(nt) {}
  virtual ~ASTNode() {}

protected:
  static std::map<NodeType, std::string> NodeLabel;

public:
  NodeType getNodeType() const { return NdType; };

  virtual void _delete() const = 0;

  virtual void dump(unsigned indent = 0) const = 0;

  virtual void visit(ASTVisitor *v) const = 0;
};

template <typename T, ASTNode::NodeType nt, typename Derived>
class ASTNodeList : public ASTNode {
public:
  using Container = std::vector<T *>;

private:
  Container elements;

public:
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
};

class Expr;
class ExprList
    : public ASTNodeList<const Expr, ASTNode::NT_ExprList, ExprList> {
public:
  ExprList() : ASTNodeList() {}
  ExprList(const Expr *e) : ASTNodeList(e) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Stmt;
class StmtList
    : public ASTNodeList<const Stmt, ASTNode::NT_StmtList, StmtList> {
public:
  StmtList() : ASTNodeList() {}
  StmtList(const Stmt *s) : ASTNodeList(s) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Decl;
class DeclList
    : public ASTNodeList<const Decl, ASTNode::NT_DeclList, DeclList> {
public:
  DeclList() : ASTNodeList() {}
  DeclList(const Decl *d) : ASTNodeList(d) {}

  virtual void visit(ASTVisitor *v) const override;
};

//} // end of namespace tir

#endif // __ASTNODE_H_
