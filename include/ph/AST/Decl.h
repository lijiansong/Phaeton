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
    IO_SPEC_Empty = 0,
    IO_SPEC_Input = 1 << 0,
    IO_SPEC_Output = 1 << 1,
  };

public:
  Decl(ASTNodeKind NodeKind, const Identifier *Id, const Expr *Expr,
       InOutSpecifier IOSpec = IO_SPEC_Empty)
      : ASTNode(NodeKind), Id(Id), TypeExpr(Expr), InOutSpec(IOSpec) {
    assert(NodeKind == AST_NODE_KIND_VarDecl ||
           NodeKind == AST_NODE_KIND_TypeDecl);
  }

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getTypeExpr() const { return TypeExpr; }
  InOutSpecifier getInOutSpecifier() const { return InOutSpec; }

  virtual void _delete() const final;

  virtual void dump(unsigned int Indent = 0) const final;

  static const Decl *create(ASTNodeKind NodeKind, const Identifier *Id,
                            const Expr *Expr,
                            InOutSpecifier IOSpec = IO_SPEC_Empty) {
    return new Decl(NodeKind, Id, Expr, IOSpec);
  }

  virtual void visit(ASTVisitor *Visitor) const override;

private:
  const Identifier *Id;
  const Expr *TypeExpr;

  const InOutSpecifier InOutSpec;
};

} // end namespace phaeton

#endif // PHAETON_AST_DECL_H
