//==------ BangCG.h ------ Representation of code generation for Bang ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language Cambricon Bang.
//
//===----------------------------------------------------------------------===//

#ifndef __BANG_CG_H__
#define __BANG_CG_H__

#if 0
#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/PyFragBuilder.h"

#include <string>

// TODO: CG for Cambricon Bang with SIMD intrinsics.
class BangCG : public DirectCodeGen {
protected:
  PythonFragBuilder Builder;

public:
  NumpyDirectCG(const Sema *sema, const std::string &prefix = "np");

  virtual void visitProgramPrologue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;
  virtual void visitStmtEpilogue(const Stmt *s) override;

  virtual const std::string
  visitContractionEpilogue(const Expr *e, const std::string &lhs,
                           const std::string &rhs,
                           const TupleList &LeftAndRightIndices);

  virtual void visitAddExprEpilogue(const BinaryExpr *be) override;
  virtual void visitSubExprEpilogue(const BinaryExpr *be) override;
  virtual void visitMulExprEpilogue(const BinaryExpr *be) override;
  virtual void visitDivExprEpilogue(const BinaryExpr *be) override;
  virtual void visitProductExprEpilogue(const BinaryExpr *be) override;

  virtual void visitBrackExprEpilogue(const BrackExpr *be) override;
};
#endif

#endif // __BANG_CG_H__
