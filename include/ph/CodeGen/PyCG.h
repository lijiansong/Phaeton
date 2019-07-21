//==------ PyCG.h ------ Representation of code generation for python ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language python.
//
//===----------------------------------------------------------------------===//
#ifndef __PYCG_H__
#define __PYCG_H__

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/PyFragBuilder.h"

#include <string>

// for python Numpy, TODO: CG for SIMD intrinsics.
class NumpyDirectCG : public DirectCodeGen {
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
                           const TupleList &LeftAndRightIndices) override;

  virtual void visitAddExprEpilogue(const BinaryExpr *be) override;
  virtual void visitSubExprEpilogue(const BinaryExpr *be) override;
  virtual void visitMulExprEpilogue(const BinaryExpr *be) override;
  virtual void visitDivExprEpilogue(const BinaryExpr *be) override;
  virtual void visitProductExprEpilogue(const BinaryExpr *be) override;

  virtual void visitBrackExprEpilogue(const BrackExpr *be) override;
};

// FIXME: Refactoring graph codegen for numpy.
#endif // __PYCG_H__
