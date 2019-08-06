//===--- Stmt.h - Classes for representing statements -----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Stmt interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_STMT_H
#define PHAETON_AST_STMT_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Identifier;
class Expr;

class Stmt : public ASTNode {
public:
  Stmt(const Identifier *id, const Expr *expr)
      : ASTNode(NODETYPE_Stmt), Id(id), RightExpr(expr) {}

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const Identifier *Id;
  const Expr *RightExpr;
};

} // end namespace phaeton

#endif // PHAETON_AST_STMT_H
