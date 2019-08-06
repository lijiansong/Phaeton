//==------ TheanoCG.h ------ Representation of code generation for TH ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language TH.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_THEANOCG_H
#define PHAETON_TARGET_THEANOCG_H

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"
#include "ph/Opt/ExprTreeVisitor.h"

#include <list>
#include <string>

namespace phaeton {

/// TheanoCG - This class is the entrance of code generator for Theano, previous
/// is Numpy.
class TheanoCG : public ExprTreeVisitor {
public:
  TheanoCG(CodeGen *cg, const std::string &prefix = "T")
      : CG(cg), ModulePrefix(prefix) {}

  void genCode(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

  std::string getResultTemp() const { return resultTemp; }
  void setResultTemp(const std::string &temp) { resultTemp = temp; }

#define GEN_VISIT_EXPR_NODE_DECL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *e) override;

  GEN_VISIT_EXPR_NODE_DECL(Add)
  GEN_VISIT_EXPR_NODE_DECL(Sub)
  GEN_VISIT_EXPR_NODE_DECL(Mul)
  GEN_VISIT_EXPR_NODE_DECL(ScalarMul)
  GEN_VISIT_EXPR_NODE_DECL(Div)
  GEN_VISIT_EXPR_NODE_DECL(ScalarDiv)
  GEN_VISIT_EXPR_NODE_DECL(Contraction)
  GEN_VISIT_EXPR_NODE_DECL(Product)
  GEN_VISIT_EXPR_NODE_DECL(Stack)
  GEN_VISIT_EXPR_NODE_DECL(Transposition)
  GEN_VISIT_EXPR_NODE_DECL(Identifier)

#undef GEN_VISIT_EXPR_NODE_DECL

  void visitBinOpExpr(const ExprNode *en, const std::string &op);
  void visitTensordotExpr(const ExprNode *en, const std::string &axes);

private:
  CodeGen *CG;
  const std::string ModulePrefix;
  std::string resultTemp;

  // The following methods are the helper methods
  // that implement functionality from the code generator 'CG'.
  const Sema *getSema() const { return CG->getSema(); }
  const std::string &getFunctionName() const { return CG->getFunctionName(); }
  std::string getTemp() { return CG->getTemp(); }
  void append(const std::string &code) { CG->append(code); }
  void addFunctionArgument(const std::string &name) {
    CG->addFunctionArgument(name);
  }
};

} // end namespace phaeton

#endif // PHAETON_TARGET_THEANOCG_H
