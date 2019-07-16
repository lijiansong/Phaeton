//===--- Stmt.h - Classes for representing statements -----------*- C++ -*-===//
//
//  This file defines the Stmt interface.
//
//===----------------------------------------------------------------------===//

#ifndef __STMT_H__
#define __STMT_H__

#pragma once

#include "ph/AST/ASTNode.h"

class Identifier;
class Expr;

class Stmt : public ASTNode {
private:
  const Identifier *Id;
  const Expr *RightExpr;

public:
  Stmt(const Identifier *id, const Expr *expr)
      : ASTNode(NT_Stmt), Id(id), RightExpr(expr) {}

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};

#endif // __STMT_H__
