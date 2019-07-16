//===--- Decl.h - Classes for representing declarations ---------*- C++ -*-===//
//
//  This file defines the Decl subclasses inherited from base class ASTNode.
//
//===----------------------------------------------------------------------===//

#ifndef __DECL_H__
#define __DECL_H__

#include "ph/AST/ASTNode.h"

class Identifier;
class Expr;

class Decl : public ASTNode {
public:
  enum InOutSpecifier {
    IO_Empty = 0,
    IO_Input = 1 << 0,
    IO_Output = 1 << 1,
  };

private:
  const Identifier *Id;
  const Expr *TypeExpr;

  const InOutSpecifier InOutSpec;

public:
  Decl(NodeType nt, const Identifier *id, const Expr *expr,
       InOutSpecifier iospec = IO_Empty)
      : ASTNode(nt), Id(id), TypeExpr(expr), InOutSpec(iospec) {
    assert(nt == NT_VarDecl || nt == NT_TypeDecl);
  }

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getTypeExpr() const { return TypeExpr; }
  const InOutSpecifier getInOutSpecifier() const { return InOutSpec; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id, const Expr *expr,
                            InOutSpecifier iospec = IO_Empty) {
    return new Decl(nt, id, expr, iospec);
  }

  virtual void visit(ASTVisitor *v) const override;
};

#endif // __DECL_H__
