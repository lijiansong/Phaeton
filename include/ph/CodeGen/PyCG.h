//==------ PyCG.h ------ Representation of code generation for python ------==//
//
// This file defines the base class CodeGen for target language python.
//
//===----------------------------------------------------------------------===//
#ifndef __PYCG_H__
#define __PYCG_H__

#include <list>
#include <string>

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/ExprTree.h"
#include "ph/CodeGen/GraphCG.h"

#if 0
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
#endif // if 0

class NumpyEmitter : public ExprTreeVisitor {
private:
  CodeGen *CG;

  const std::string ModulePrefix;

  std::string resultTemp;

public:
  NumpyEmitter(CodeGen *cg, const std::string &prefix = "np")
      : CG(cg), ModulePrefix(prefix) {}

  void genCode(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

  std::string getResultTemp() const { return resultTemp; }
  void setResultTemp(const std::string &temp) { resultTemp = temp; }

#define DECL_VISIT_EXPR_NODE(Kind)                                             \
  virtual void visit##Kind##Expr(const Kind##Expr *e) override;

  DECL_VISIT_EXPR_NODE(Add)
  DECL_VISIT_EXPR_NODE(Sub)
  DECL_VISIT_EXPR_NODE(Mul)
  DECL_VISIT_EXPR_NODE(Div)
  DECL_VISIT_EXPR_NODE(Contraction)
  DECL_VISIT_EXPR_NODE(Product)
  DECL_VISIT_EXPR_NODE(Stack)
  DECL_VISIT_EXPR_NODE(Identifier)
#undef DECL_VISIT_EXPR_NODE

  void visitBinOpExpr(const ExprNode *en, const std::string &op);
  void visitTensordotExpr(const ExprNode *en, const std::string &axes);

private:
  std::string getTemp() { return CG->getTemp(); }
  void append(const std::string &code) { CG->append(code); }

  const Sema *getSema() const { return CG->getSema(); }

  void addExprNode(const Expr *expr, ExprNode *en) {
    CG->addExprNode(expr, en);
  }

  const ExprNode *getExprNode(const Expr *expr) const {
    return CG->getExprNode(expr);
  }
};

#endif // __PYCG_H__
