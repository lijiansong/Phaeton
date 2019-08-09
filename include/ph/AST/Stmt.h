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
  Stmt(const Identifier *Id, const Expr *E)
      : ASTNode(AST_NODE_KIND_Stmt), Id(Id), RightExpr(E) {}

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned int Indent = 0) const final;

  static Stmt *create(const Identifier *Id, const Expr *E) {
    return new Stmt(Id, E);
  }

  virtual void visit(ASTVisitor *Visitor) const override;

private:
  const Identifier *Id;
  const Expr *RightExpr;
};

} // end namespace phaeton

#endif // PHAETON_AST_STMT_H
