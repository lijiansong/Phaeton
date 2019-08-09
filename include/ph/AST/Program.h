//===--- Program.h - Classes for representing programs ----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the subclass Program inherited from ASTNode.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_PROGRAM_H
#define PHAETON_AST_PROGRAM_H

#pragma once

#include "ph/AST/ASTNode.h"

namespace phaeton {

class DeclList;
class StmtList;
class ElementDirective;

class Program : public ASTNode {
public:
  Program(const DeclList *Decls, const StmtList *Stmts,
          const ElementDirective *ED = nullptr)
      : ASTNode(AST_NODE_KIND_Program), Decls(Decls), Stmts(Stmts), Elem(ED) {}

  const DeclList *getDecls() const { return Decls; }
  const StmtList *getStmts() const { return Stmts; }
  const ElementDirective *getElem() const { return Elem; }

  virtual void dump(unsigned Indent = 0) const final;

  virtual void _delete() const final;

  static const Program *create(const DeclList *Decls, const StmtList *Stmts,
                               const ElementDirective *ED = nullptr) {
    return new Program(Decls, Stmts, ED);
  }

  static void destroy(const Program *Prog) {
    Prog->_delete();
    delete Prog;
  }

  virtual void visit(ASTVisitor *Visitor) const override;

private:
  const DeclList *Decls;
  const StmtList *Stmts;
  const ElementDirective *Elem;
};

} // end namespace phaeton

#endif // PHAETON_AST_PROGRAM_H
