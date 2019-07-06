#include <string>

#include "tir/AST/AST.h"
#include "tir/CodeGen/PyCG.h"
#include "tir/Sema/Sema.h"
#include "tir/Sema/Type.h"

PythonFragBuilder::PythonFragBuilder(const std::string &prefix)
    : ModulePrefix(prefix) {}

void PythonFragBuilder::buildNumpyProgramPrologue(const CodeGen *cg) {
  Fragment = "# ----- Autogen kernel by TensorIR -----\n";
  Fragment += "import numpy as " + getModulePrefix() + "\n\n";
}

void PythonFragBuilder::buildNumpyDeclEpilogue(const Decl *d,
                                               const CodeGen *cg) {
  Fragment = "";
  if (d->getNodeType() == ASTNode::NT_TypeDecl)
    return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = cg->getSema()->getSymbol(name);
  const TensorType &type = sym->getType();

  Fragment = (name + " = " + getModulePrefix() + ".random.rand(" +
              type.getDimString() + ")\n");
}

void PythonFragBuilder::buildStmtEpilogue(const Stmt *s,
                                          const DirectCodeGen *cg) {
  const std::string &name = s->getIdentifier()->getName();
  const Expr *expr = s->getExpr();
  const std::string &temp = cg->getTempForExpr(expr);

  Fragment = (name + " = " + temp + "\n");
}

void PythonFragBuilder::buildTensorDotString(const std::string &result,
                                             const std::string &lhs,
                                             const std::string &rhs,
                                             const std::string &axes) {
  Fragment = (result + " = " + getModulePrefix() + ".tensordot(" + lhs + ", " +
              rhs + ", ");
  if (!axes.length())
    Fragment += ("axes=0)\n");
  else
    Fragment += ("axes=" + axes + ")\n");
}

void PythonFragBuilder::buildContractionEpilogue(
    const std::string &result, const std::string &lhs, const std::string &rhs,
    const CodeGen::TupleList &LeftAndRightIndices) {
  if (LeftAndRightIndices.empty())
    buildTensorDotString(result, lhs, rhs);
  else
    buildTensorDotString(result, lhs, rhs,
                         CodeGen::getTupleListString(LeftAndRightIndices));
}

void PythonFragBuilder::buildBinOpExprEpilogue(const BinaryExpr *be,
                                               const std::string &op,
                                               const DirectCodeGen *cg) {
  const std::string &tempL = cg->getTempForExpr(be->getLeft());
  const std::string &tempR = cg->getTempForExpr(be->getRight());
  const std::string &result = cg->getTempForExpr(be);

  buildBinOpExpr(result, op, tempL, tempR);
}

void PythonFragBuilder::buildAddExprEpilogue(const BinaryExpr *be,
                                             const DirectCodeGen *cg) {
  buildBinOpExprEpilogue(be, "+", cg);
}

void PythonFragBuilder::buildSubExprEpilogue(const BinaryExpr *be,
                                             const DirectCodeGen *cg) {
  buildBinOpExprEpilogue(be, "-", cg);
}
void PythonFragBuilder::buildMulExprEpilogue(const BinaryExpr *be,
                                             const DirectCodeGen *cg) {
  buildBinOpExprEpilogue(be, "*", cg);
}
void PythonFragBuilder::buildDivExprEpilogue(const BinaryExpr *be,
                                             const DirectCodeGen *cg) {
  buildBinOpExprEpilogue(be, "/", cg);
}

void PythonFragBuilder::buildProductExprEpilogue(const BinaryExpr *be,
                                                 const DirectCodeGen *cg) {
  const std::string &tempL = cg->getTempForExpr(be->getLeft());
  const std::string &tempR = cg->getTempForExpr(be->getRight());
  const std::string &result = cg->getTempForExpr(be);

  buildTensorDotString(result, tempL, tempR);
}

void PythonFragBuilder::buildBrackExprEpilogue(const BrackExpr *be,
                                               const DirectCodeGen *cg) {
  const ExprList &exprs = *be->getExprs();
  std::string result = cg->getTempForExpr(be);

  std::string stack = "[";
  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    if (i > 0)
      stack += ", ";
    stack += cg->getTempForExpr(e);
  }
  stack += "]";

  Fragment =
      (result + " = " + getModulePrefix() + ".stack(" + stack + ", axis=0)\n");
}

void PythonFragBuilder::buildContraction(const std::string &result,
                                         const std::string &lhs,
                                         const CodeGen::List &lhsIndices,
                                         const std::string &rhs,
                                         const CodeGen::List &rhsIndices) {
  CodeGen::TupleList indices;
  indices.push_back(lhsIndices);
  indices.push_back(rhsIndices);

  Fragment =
      (result + " = " + getModulePrefix() + ".tensordot(" + lhs + ", " + rhs +
       ", " + "axes=" + CodeGen::getTupleListString(indices) + ")\n");
}

void PythonFragBuilder::buildTensorProduct(const std::string &result,
                                           const std::string &lhs,
                                           const std::string &rhs) {
  Fragment = (result + " = " + getModulePrefix() + ".tensordot(" + lhs + ", " +
              rhs + ")\n");
}

void PythonFragBuilder::buildTensorStack(const std::string &result,
                                         const std::list<std::string> &temps) {
  Fragment = (result + " = " + getModulePrefix() + ".stack([");
  for (auto it = temps.begin(); it != temps.end(); it++) {
    Fragment += (*it);
    if (std::next(it) != temps.end())
      Fragment += (", ");
  }
  Fragment += ("])\n");
}

void PythonFragBuilder::buildAssignment(const std::string &result,
                                        const std::string &expr) {
  Fragment = (result + " = " + expr + "\n");
}

void PythonFragBuilder::buildBinOpExpr(const std::string &result,
                                       const std::string &op,
                                       const std::string &lhs,
                                       const std::string &rhs) {
  Fragment = (result + " = " + lhs + " " + op + " " + rhs + "\n");
}

void PythonFragBuilder::buildAddExpr(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  buildBinOpExpr(result, "+", lhs, rhs);
}

void PythonFragBuilder::buildSubExpr(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  buildBinOpExpr(result, "-", lhs, rhs);
}
void PythonFragBuilder::buildMulExpr(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  buildBinOpExpr(result, "*", lhs, rhs);
}
void PythonFragBuilder::buildDivExpr(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  buildBinOpExpr(result, "/", lhs, rhs);
}
