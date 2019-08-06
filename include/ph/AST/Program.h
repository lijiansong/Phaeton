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
class ElemDirect;

class Program : public ASTNode {
public:
  Program(const DeclList *decls, const StmtList *stmts,
          const ElemDirect *elem = nullptr)
      : ASTNode(NODETYPE_Program), Decls(decls), Stmts(stmts), Elem(elem) {}

  const DeclList *getDecls() const { return Decls; }
  const StmtList *getStmts() const { return Stmts; }
  const ElemDirect *getElem() const { return Elem; }

  virtual void dump(unsigned indent = 0) const final;

  virtual void _delete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts,
                               const ElemDirect *elem = nullptr) {
    return new Program(decls, stmts, elem);
  }

  static void destroy(const Program *p) {
    p->_delete();
    delete p;
  }

  virtual void visit(ASTVisitor *v) const override;

private:
  const DeclList *Decls;
  const StmtList *Stmts;
  const ElemDirect *Elem;
};

} // end namespace phaeton

#endif // PHAETON_AST_PROGRAM_H
