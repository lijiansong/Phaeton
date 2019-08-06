//===--- ElemDirect.h - Class for representing elem directive ---*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the elements directive.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_AST_ELEM_DIRECT_H
#define PHAETON_AST_ELEM_DIRECT_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Integer;
class Expr;

class ElemDirect : public ASTNode {
public:
  enum POSSpecifier {
    POS_First,
    POS_Last,
  };

public:
  ElemDirect(NodeType nt, POSSpecifier posspec, const Integer *i,
             const Expr *identifiers)
      : ASTNode(nt), POSSpec(posspec), I(i), Identifiers(identifiers) {
    assert(nt == NODETYPE_ElemDirect);
  }

  POSSpecifier getPOSSpecifier() const { return POSSpec; }
  const Integer *getInteger() const { return I; }
  const Expr *getIdentifiers() const { return Identifiers; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const ElemDirect *create(NodeType nt, POSSpecifier posspec,
                                  const Integer *i, const Expr *identifiers) {
    return new ElemDirect(nt, posspec, i, identifiers);
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const POSSpecifier POSSpec;
  const Integer *I;
  const Expr *Identifiers;
};

} // end namespace phaeton

#endif // PHAETON_AST_ELEM_DIRECT_H
