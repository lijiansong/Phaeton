//==------ DirectCG.h ----- Representation of CodeGen ----------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines intefaces of direct code generation.
//
//===----------------------------------------------------------------------===//

#ifndef __DIRECT_CG_H__
#define __DIRECT_CG_H__

#include "ph/AST/AST.h"
#include "ph/CodeGen/CodeGen.h"

#include <map>
#include <string>

class DirectCodeGen : public CodeGen {
  // map for 'Expr' nodes in the AST to temporary variables:
  std::map<const Expr *, std::string> ExprTemps;

protected:
  const std::string addTempForExpr(const Expr *e);
  const std::string addNameForExpr(const Expr *e, const std::string &name);

public:
  const std::string getTempForExpr(const Expr *e) const;

public:
  DirectCodeGen(const Sema *sema);

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  virtual const std::string visitContraction(const Expr *e,
                                             const TupleList &indices);

  // TODO: add wrappers for prologue and epilogue
  virtual void visitProgramPrologue(const Program *p) {}
  virtual void visitProgramEpilogue(const Program *p) {}

  virtual void visitDeclPrologue(const Decl *d) {}
  virtual void visitDeclEpilogue(const Decl *d) {}
  virtual void visitStmtPrologue(const Stmt *d) {}
  virtual void visitStmtEpilogue(const Stmt *d) {}

  virtual void visitContractionPrologue(const Expr *e,
                                        const TupleList &indices) {}
  virtual const std::string
  visitContractionEpilogue(const Expr *e, const std::string &lhs,
                           const std::string &rhs,
                           const TupleList &LeftAndRightInds) {
    return std::string();
  }

  virtual void visitBinaryExprPrologue(const BinaryExpr *be) {}
  virtual void visitBinaryExprEpilogue(const BinaryExpr *be) {}

  virtual void visitContractionExprPrologue(const BinaryExpr *be) {}
  virtual void visitContractionExprEpilogue(const BinaryExpr *be) {}
  virtual void visitAddExprPrologue(const BinaryExpr *be) {}
  virtual void visitAddExprEpilogue(const BinaryExpr *be) {}
  virtual void visitSubExprPrologue(const BinaryExpr *be) {}
  virtual void visitSubExprEpilogue(const BinaryExpr *be) {}
  virtual void visitMulExprPrologue(const BinaryExpr *be) {}
  virtual void visitMulExprEpilogue(const BinaryExpr *be) {}
  virtual void visitDivExprPrologue(const BinaryExpr *be) {}
  virtual void visitDivExprEpilogue(const BinaryExpr *be) {}
  virtual void visitProductExprPrologue(const BinaryExpr *be) {}
  virtual void visitProductExprEpilogue(const BinaryExpr *be) {}

  virtual void visitIdentifierPrologue(const Identifier *id) {}
  virtual void visitIdentifierEpilogue(const Identifier *id) {}
  virtual void visitIntegerPrologue(const Integer *i) {}
  virtual void visitIntegerEpilogue(const Integer *i) {}
  virtual void visitBrackExprPrologue(const BrackExpr *be) {}
  virtual void visitBrackExprEpilogue(const BrackExpr *be) {}
  virtual void visitParenExprPrologue(const ParenExpr *pe) {}
  virtual void visitParenExprEpilogue(const ParenExpr *pe) {}
};

#endif // __DIRECT_CG_H__
