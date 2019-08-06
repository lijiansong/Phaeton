//===--- Expr.h - Classes for representing expressions ----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Expr interface and subclasses.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_EXPR_H
#define PHAETON_AST_EXPR_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Expr : public ASTNode {
protected:
  Expr(NodeType nt) : ASTNode(nt) {}

public:
  virtual ~Expr() {}

  virtual void visit(ASTVisitor *v) const override;

  virtual bool isIdentifier() const { return false; }
};

class Factor : public Expr {
protected:
  Factor(NodeType nt) : Expr(nt) {}

public:
  virtual ~Factor() {}

  virtual void visit(ASTVisitor *v) const override = 0;
};

class BinaryExpr : public Expr {
public:
  BinaryExpr(NodeType nt, const Expr *left, const Expr *right)
      : Expr(nt), LeftExpr(left), RightExpr(right) {
    assert(nt == NODETYPE_TranspositionExpr || nt == NODETYPE_ContractionExpr ||
           nt == NODETYPE_AddExpr || nt == NODETYPE_SubExpr ||
           nt == NODETYPE_MulExpr || nt == NODETYPE_DivExpr ||
           nt == NODETYPE_ProductExpr);
  }

  const Expr *getLeft() const { return LeftExpr; }
  const Expr *getRight() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left, const Expr *right) {
    return new BinaryExpr(nt, left, right);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const Expr *LeftExpr;
  const Expr *RightExpr;
};

class Identifier : public Factor {
public:
  Identifier(const std::string name)
      : Factor(NODETYPE_Identifier), Name(name) {}

  const std::string &getName() const { return Name; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Identifier *create(const std::string &name) {
    return new Identifier(name);
  }

  virtual void visit(ASTVisitor *v) const override;

  virtual bool isIdentifier() const override { return true; }

private:
  const std::string Name;
};

class Integer : public Factor {
public:
  Integer(int value) : Factor(NODETYPE_Integer), Value(value) {}

  int getValue() const { return Value; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Integer *create(int value) { return new Integer(value); }

  virtual void visit(ASTVisitor *v) const override;

private:
  int Value;
};

class BrackExpr : public Factor {
public:
  BrackExpr(const ExprList *exprs) : Factor(NODETYPE_BrackExpr), Exprs(exprs) {}

  const ExprList *getExprs() const { return Exprs; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const ExprList *Exprs;
};

class ParenExpr : public Factor {
public:
  ParenExpr(const Expr *expr) : Factor(NODETYPE_ParenExpr), NestedExpr(expr) {}

  const Expr *getExpr() const { return NestedExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const Expr *NestedExpr;
};

} // end namespace phaeton

#endif // PHAETON_AST_EXPR_H
