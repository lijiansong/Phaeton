#include <vector>

#include "tir/AST/AST.h"
#include "tir/CodeGen/PyCG.h"
#include "tir/Sema/Sema.h"
#include "tir/Sema/Type.h"

NumpyDirectCG::NumpyDirectCG(const Sema *sema, const std::string &prefix)
    : DirectCodeGen(sema), Builder(prefix) {}

void NumpyDirectCG::visitProgramPrologue(const Program *p) {
  Builder.buildNumpyProgramPrologue(this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildNumpyDeclEpilogue(d, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitStmtEpilogue(const Stmt *s) {
  Builder.buildStmtEpilogue(s, this);
  append(Builder.getFragment());
}

const std::string
NumpyDirectCG::visitContractionEpilogue(const Expr *e, const std::string &lhs,
                                        const std::string &rhs,
                                        const TupleList &LeftAndRightIndices) {
  const std::string result = getTemp();
  Builder.buildContractionEpilogue(result, lhs, rhs, LeftAndRightIndices);
  append(Builder.getFragment());
  return result;
}

void NumpyDirectCG::visitAddExprEpilogue(const BinaryExpr *be) {
  Builder.buildAddExprEpilogue(be, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitSubExprEpilogue(const BinaryExpr *be) {
  Builder.buildSubExprEpilogue(be, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitMulExprEpilogue(const BinaryExpr *be) {
  Builder.buildMulExprEpilogue(be, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitDivExprEpilogue(const BinaryExpr *be) {
  Builder.buildDivExprEpilogue(be, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitProductExprEpilogue(const BinaryExpr *be) {
  Builder.buildProductExprEpilogue(be, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitBrackExprEpilogue(const BrackExpr *be) {
  Builder.buildBrackExprEpilogue(be, this);
  append(Builder.getFragment());
}

NumpyGraphCG::NumpyGraphCG(const Sema *sema, const std::string &prefix)
    : GraphCodeGen(sema), Builder(prefix) {}

void NumpyGraphCG::visitProgramPrologue(const Program *p) {
  Builder.buildNumpyProgramPrologue(this);
  append(Builder.getFragment());
}

void NumpyGraphCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildNumpyDeclEpilogue(d, this);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitContraction(const std::string &result,
                                   const std::string &lhs,
                                   const List &lhsIndices,
                                   const std::string &rhs,
                                   const List &rhsIndices) {
  Builder.buildContraction(result, lhs, lhsIndices, rhs, rhsIndices);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitTensorProduct(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  Builder.buildTensorProduct(result, lhs, rhs);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitTensorStack(const std::string &result,
                                   const std::list<std::string> &temps) {
  Builder.buildTensorStack(result, temps);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitAssignment(const std::string &result,
                                  const std::string &expr) {
  Builder.buildAssignment(result, expr);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitAddExpr(const std::string &result,
                               const std::string &lhs, const std::string &rhs) {
  Builder.buildAddExpr(result, lhs, rhs);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitSubExpr(const std::string &result,
                               const std::string &lhs, const std::string &rhs) {
  Builder.buildSubExpr(result, lhs, rhs);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitMulExpr(const std::string &result,
                               const std::string &lhs, const std::string &rhs) {
  Builder.buildMulExpr(result, lhs, rhs);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitDivExpr(const std::string &result,
                               const std::string &lhs, const std::string &rhs) {
  Builder.buildDivExpr(result, lhs, rhs);
  append(Builder.getFragment());
}
