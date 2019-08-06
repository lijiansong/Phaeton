//===--- Decl.h - Classes for representing declarations ---------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Decl subclasses inherited from base class ASTNode.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_DECL_H
#define PHAETON_AST_DECL_H

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Identifier;
class Expr;

class Decl : public ASTNode {
public:
  enum InOutSpecifier {
    IO_Empty = 0,
    IO_Input = 1 << 0,
    IO_Output = 1 << 1,
  };

public:
  Decl(NodeType nt, const Identifier *id, const Expr *expr,
       InOutSpecifier iospec = IO_Empty)
      : ASTNode(nt), Id(id), TypeExpr(expr), InOutSpec(iospec) {
    assert(nt == NODETYPE_VarDecl || nt == NODETYPE_TypeDecl);
  }

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getTypeExpr() const { return TypeExpr; }
  InOutSpecifier getInOutSpecifier() const { return InOutSpec; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id, const Expr *expr,
                            InOutSpecifier iospec = IO_Empty) {
    return new Decl(nt, id, expr, iospec);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const Identifier *Id;
  const Expr *TypeExpr;

  const InOutSpecifier InOutSpec;
};

} // end namespace phaeton

#endif // PHAETON_AST_DECL_H
