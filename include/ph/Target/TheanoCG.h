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

#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/NaiveCG.h"
#include "ph/Opt/ExprTreeVisitor.h"

#include <list>
#include <string>

namespace phaeton {

/// TheanoCG - This class is the entrance of code generator for Theano, previous
/// is Numpy.
class TheanoCG : public ExprTreeVisitor {
public:
  TheanoCG(CodeGen *Gen, const std::string &Prefix = "Th")
      : CG(Gen), ModulePrefix(Prefix) {}

  const std::string &getCode() const { return CG->getTgtLangCode(); }
  void genCode(const Program *Prog);

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

  std::string getResultTmp() const { return ResultTmp; }
  void setResultTmp(const std::string &Tmp) { ResultTmp = Tmp; }

#define GEN_VISIT_EXPR_NODE_DECL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *E) override;

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

  void visitBinOpExpr(const ExpressionNode *Node, const std::string &Operation);
  void visitTensorDotExpr(const ExpressionNode *Node, const std::string &Axes);

private:
  CodeGen *CG;
  const std::string ModulePrefix;
  std::string ResultTmp;

  // The following methods are the helper methods
  // that implement functionality from code generator 'CG'. We need to record
  // some information from these help methods.
  const Sema *getSema() const { return CG->getSema(); }
  const std::string &getCGFuncName() const { return CG->getCGFuncName(); }
  std::string getTmp() { return CG->getTmp(); }
  void addFuncArg(const std::string &Name) { CG->addFuncArg(Name); }
  void appendCode(const std::string &Code) { CG->appendCode(Code); }
};

} // end namespace phaeton

#endif // PHAETON_TARGET_THEANOCG_H
