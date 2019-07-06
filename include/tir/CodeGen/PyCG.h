//==------ PyCG.h ------ Representation of code generation for python ------==//
//
// This file defines the base class CodeGen for target language python.
//
//===----------------------------------------------------------------------===//
#ifndef __PYCG_H__
#define __PYCG_H__

#include <string>

#include "tir/CodeGen/DirectCG.h"
#include "tir/CodeGen/GraphCG.h"
#include "tir/CodeGen/PyFragBuilder.h"

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
                           const TupleList &LeftAndRightIndices);

  virtual void visitAddExprEpilogue(const BinaryExpr *be) override;
  virtual void visitSubExprEpilogue(const BinaryExpr *be) override;
  virtual void visitMulExprEpilogue(const BinaryExpr *be) override;
  virtual void visitDivExprEpilogue(const BinaryExpr *be) override;
  virtual void visitProductExprEpilogue(const BinaryExpr *be) override;

  virtual void visitBrackExprEpilogue(const BrackExpr *be) override;
};

class NumpyGraphCG : public GraphCodeGen {
protected:
  PythonFragBuilder Builder;

public:
  NumpyGraphCG(const Sema *sema, const std::string &prefix = "np");

  virtual void visitProgramPrologue(const Program *p) override;

  virtual void visitDeclEpilogue(const Decl *d) override;

  virtual void emitContraction(const std::string &result,
                               const std::string &lhs, const List &lhsIndices,
                               const std::string &rhs,
                               const List &rhsIndices) override;
  virtual void emitTensorProduct(const std::string &result,
                                 const std::string &lhs,
                                 const std::string &rhs) override;
  virtual void emitTensorStack(const std::string &result,
                               const std::list<std::string> &temps) override;
  virtual void emitAssignment(const std::string &result,
                              const std::string &expr) override;

  virtual void emitAddExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) override;
  virtual void emitSubExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) override;
  virtual void emitMulExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) override;
  virtual void emitDivExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) override;
};

#endif // __PYCG_H__
