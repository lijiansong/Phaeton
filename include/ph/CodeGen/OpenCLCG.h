//==------ OpenCLCG.h - Representation of code generation for OpenCL -------==//
//
// This file defines the base class CodeGen for target language OpenCL.
//
//===----------------------------------------------------------------------===//

#ifndef __OPENCL_CG_H__
#define __OPENCL_CG_H__

#if 0
#include <string>

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/PyFragBuilder.h"

class OpenCLCG : public DirectCodeGen {
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

#endif // __OPENCL_CG_H__
