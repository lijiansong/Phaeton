#ifndef __EXPR_H__
#define __EXPR_H__
#pragma once

#include "ph/AST/ASTNode.h"

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
private:
  const Expr *LeftExpr;
  const Expr *RightExpr;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Expr *right)
      : Expr(nt), LeftExpr(left), RightExpr(right) {
    assert(nt == NT_ContractionExpr || nt == NT_AddExpr || nt == NT_SubExpr ||
           nt == NT_MulExpr || nt == NT_DivExpr || nt == NT_ProductExpr);
  }

  const Expr *getLeft() const { return LeftExpr; }
  const Expr *getRight() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left, const Expr *right) {
    return new BinaryExpr(nt, left, right);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class Identifier : public Factor {
private:
  const std::string Name;

public:
  Identifier(const std::string name) : Factor(NT_Identifier), Name(name) {}

  const std::string &getName() const { return Name; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Identifier *create(const std::string &name) {
    return new Identifier(name);
  }

  virtual void visit(ASTVisitor *v) const override;

  virtual bool isIdentifier() const override { return true; }
};

class Integer : public Factor {
private:
  int Value;

public:
  Integer(int value) : Factor(NT_Integer), Value(value) {}

  int getValue() const { return Value; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Integer *create(int value) { return new Integer(value); }

  virtual void visit(ASTVisitor *v) const override;
};

class ExprList;

class BrackExpr : public Factor {
private:
  const ExprList *Exprs;

public:
  BrackExpr(const ExprList *exprs) : Factor(NT_BrackExpr), Exprs(exprs) {}

  const ExprList *getExprs() const { return Exprs; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class ParenExpr : public Factor {
private:
  const Expr *NestedExpr;

public:
  ParenExpr(const Expr *expr) : Factor(NT_ParenExpr), NestedExpr(expr) {}

  const Expr *getExpr() const { return NestedExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};
#endif // __EXPR_H__
