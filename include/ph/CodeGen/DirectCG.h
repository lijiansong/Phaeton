//==------ DirectCG.h ----- Representation of CodeGen ----------------------==//
//
// This file defines intefaces of direct and naive code generation.
//
//===----------------------------------------------------------------------===//

#ifndef __DIRECT_CG_H__
#define __DIRECT_CG_H__

#include <map>
#include <string>

#include "ph/AST/AST.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/CodeGen/ExprTree.h"

// TODO: Refact naive code generation
class DirectCodeGen : public CodeGen {
public:
  DirectCodeGen(const Sema *sema);

  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override {}
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);
};

#endif // __DIRECT_CG_H__
