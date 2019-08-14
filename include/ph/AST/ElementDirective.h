//===--- ElementDirective.h - Class for element directive expr --*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the elements directive expression.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_AST_ELEMENT_DIRECTIVE_H
#define PHAETON_AST_ELEMENT_DIRECTIVE_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class Integer;
class Expr;

class ElementDirective : public ASTNode {
public:
  enum POSSpecifier {
    POS_First,
    POS_Last,
  };

public:
  ElementDirective(ASTNodeKind NK, POSSpecifier POSSpec, const Integer *Int,
                   const Expr *Id)
      : ASTNode(NK), POSSpec(POSSpec), Int(Int), Identifiers(Id) {
    assert(NK == AST_NODE_KIND_ElementDirective);
  }

  POSSpecifier getPOSSpecifier() const { return POSSpec; }
  const Integer *getInteger() const { return Int; }
  const Expr *getIdentifiers() const { return Identifiers; }

  virtual void _delete() const final;

  virtual void dump(unsigned int Indent = 0) const final;

  static const ElementDirective *create(ASTNodeKind NK, POSSpecifier POSSpec,
                                        const Integer *Int, const Expr *Id) {
    return new ElementDirective(NK, POSSpec, Int, Id);
  }

  virtual void visit(ASTVisitor *Visitor) const override;

private:
  const POSSpecifier POSSpec;
  const Integer *Int;
  const Expr *Identifiers;
};

} // end namespace phaeton

#endif // PHAETON_AST_ELEMENT_DIRECTIVE_H
