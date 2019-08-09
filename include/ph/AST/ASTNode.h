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
  enum ASTNodeKind {
    AST_NODE_KIND_Program,

    AST_NODE_KIND_DeclList,
    AST_NODE_KIND_StmtList,
    AST_NODE_KIND_ExprList,

    AST_NODE_KIND_VarDecl,
    AST_NODE_KIND_TypeDecl,

    AST_NODE_KIND_ElementDirective,

    AST_NODE_KIND_Stmt,

    AST_NODE_KIND_TranspositionExpr,
    AST_NODE_KIND_ContractionExpr,
    AST_NODE_KIND_AddExpr,
    AST_NODE_KIND_SubExpr,
    AST_NODE_KIND_MulExpr,
    AST_NODE_KIND_DivExpr,
    AST_NODE_KIND_ProductExpr,
    AST_NODE_KIND_Identifier,
    AST_NODE_KIND_Integer,
    AST_NODE_KIND_BrackExpr,
    AST_NODE_KIND_ParenExpr,

    AST_NODE_KIND_ASTNodeKind_Count
  };

public:
  ASTNodeKind getASTNodeKind() const { return NodeKind; };

  virtual void _delete() const = 0;

  virtual void dump(unsigned Indent = 0) const = 0;

  virtual void visit(ASTVisitor *Visitor) const = 0;

protected:
  ASTNode(ASTNodeKind NK) : NodeKind(NK) {}
  virtual ~ASTNode() {}
  static std::map<ASTNodeKind, std::string> NodeLabel;

private:
  ASTNodeKind NodeKind;
};

template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
class ASTNodeList : public ASTNode {
public:
  using Container = std::vector<T *>;

  ASTNodeList() : ASTNode(NK) {}
  ASTNodeList(T *);
  virtual ~ASTNodeList() {}

  void append(T *Tpl) { Elements.push_back(Tpl); }

  int size() const { return Elements.size(); }

  T *operator[](int Index) const { return Elements.at(Index); }

  virtual void _delete() const final;

  virtual void dump(unsigned int Indent = 0) const final;

  static Derived *create() { return new Derived(); }

  static Derived *create(T *Type) { return new Derived(Type); }

  static Derived *append(Derived *List, T *Tpl) {
    List->append(Tpl);
    return List;
  }

  typename Container::const_iterator begin() const { return Elements.begin(); }
  typename Container::const_iterator end() const { return Elements.end(); }

  virtual void visit(ASTVisitor *Visitor) const override {}

private:
  Container Elements;
};

class Expr;
class ExprList : public ASTNodeList<const Expr, ASTNode::AST_NODE_KIND_ExprList,
                                    ExprList> {
public:
  ExprList()
      : ASTNodeList<const Expr, ASTNode::AST_NODE_KIND_ExprList, ExprList>() {}
  ExprList(const Expr *E) : ASTNodeList(E) {}

  virtual void visit(ASTVisitor *Visitor) const override;
};

class Stmt;
class StmtList : public ASTNodeList<const Stmt, ASTNode::AST_NODE_KIND_StmtList,
                                    StmtList> {
public:
  StmtList()
      : ASTNodeList<const Stmt, ASTNode::AST_NODE_KIND_StmtList, StmtList>() {}
  StmtList(const Stmt *S) : ASTNodeList(S) {}

  virtual void visit(ASTVisitor *Visitor) const override;
};

class Decl;
class DeclList : public ASTNodeList<const Decl, ASTNode::AST_NODE_KIND_DeclList,
                                    DeclList> {
public:
  DeclList()
      : ASTNodeList<const Decl, ASTNode::AST_NODE_KIND_DeclList, DeclList>() {}
  DeclList(const Decl *D) : ASTNodeList(D) {}

  virtual void visit(ASTVisitor *Visitor) const override;
};

} // end namespace phaeton

#endif // PHAETON_AST_AST_NODE_H
