//===--- Expr.h - Classes for representing expressions ----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Expression interface and subclasses.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_EXPR_H
#define PHAETON_AST_EXPR_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Expression : public ASTNode {
protected:
  Expression(ASTNodeKind NK) : ASTNode(NK) {}

public:
  virtual ~Expression() {}

  virtual void visit(ASTVisitor *Visitor) const override;

  virtual bool isIdentifier() const { return false; }
};

class Factor : public Expression {
protected:
  Factor(ASTNodeKind NK) : Expression(NK) {}

public:
  virtual ~Factor() {}

  virtual void visit(ASTVisitor *Visitor) const override = 0;
};

class BinaryExpr : public Expression {
public:
  BinaryExpr(ASTNodeKind NK, const Expression *Left, const Expression *Right)
      : Expression(NK), LeftExpr(Left), RightExpr(Right) {
    assert(NK == AST_NODE_KIND_TranspositionExpr ||
           NK == AST_NODE_KIND_ContractionExpr || NK == AST_NODE_KIND_AddExpr ||
           NK == AST_NODE_KIND_SubExpr || NK == AST_NODE_KIND_MulExpr ||
           NK == AST_NODE_KIND_DivExpr || NK == AST_NODE_KIND_ProductExpr);
  }

  const Expression *getLeft() const { return LeftExpr; }
  const Expression *getRight() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned Indent = 0) const final;

  static BinaryExpr *create(ASTNodeKind NK, const Expression *Left,
                            const Expression *Right) {
    return new BinaryExpr(NK, Left, Right);
  }

  virtual void visit(ASTVisitor *Visitor) const override;

private:
  const Expression *LeftExpr;
  const Expression *RightExpr;
};

class Identifier : public Factor {
public:
  Identifier(const std::string Name)
      : Factor(AST_NODE_KIND_Identifier), Name(Name) {}

  const std::string &getName() const { return Name; }

  virtual void _delete() const final {}

  virtual void dump(unsigned Indent = 0) const final;

  static const Identifier *create(const std::string &Name) {
    return new Identifier(Name);
  }

  virtual void visit(ASTVisitor *Visitor) const override;

  virtual bool isIdentifier() const override { return true; }

private:
  const std::string Name;
};

class Integer : public Factor {
public:
  Integer(int value) : Factor(AST_NODE_KIND_Integer), Value(value) {}

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
  BrackExpr(const ExprList *exprs)
      : Factor(AST_NODE_KIND_BrackExpr), Exprs(exprs) {}

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
  ParenExpr(const Expression *expr)
      : Factor(AST_NODE_KIND_ParenExpr), NestedExpr(expr) {}

  const Expression *getExpr() const { return NestedExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expression *expr) {
    return new ParenExpr(expr);
  }

  virtual void visit(ASTVisitor *visitor) const override;

private:
  const Expression *NestedExpr;
};

} // end namespace phaeton

#endif // PHAETON_AST_EXPR_H
