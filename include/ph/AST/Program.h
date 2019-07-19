//===--- Program.h - Classes for representing programs ----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the subclass Program inherited from ASTNode.
//
//===----------------------------------------------------------------------===//

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#pragma once

#include "ph/AST/ASTNode.h"

class DeclList;
class StmtList;

class Program : public ASTNode {
private:
  const DeclList *Decls;
  const StmtList *Stmts;

public:
  Program(const DeclList *decls, const StmtList *stmts)
      : ASTNode(NT_Program), Decls(decls), Stmts(stmts) {}

  const DeclList *getDecls() const { return Decls; }
  const StmtList *getStmts() const { return Stmts; }

  virtual void dump(unsigned indent = 0) const final;

  virtual void _delete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts) {
    return new Program(decls, stmts);
  }

  static void destroy(const Program *p) {
    p->_delete();
    delete p;
  }

  virtual void visit(ASTVisitor *v) const override;
};

#endif // __PROGRAM_H__
