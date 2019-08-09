//==------ NaiveCG.h ----- Representation of CG via naive methods ----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines intefaces of direct code generation via a naive method.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_NAIVECG_H
#define PHAETON_CODEGEN_NAIVECG_H

#include "ph/AST/AST.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/TensorExprTree.h"

#include <map>
#include <string>

namespace phaeton {

class NaiveCodeGen : public CodeGen {
public:
  NaiveCodeGen(const Sema *S, const std::string &FuncName);

#define GEN_DCG_VISIT_EXPR_DECL(ExprName)                                      \
  virtual void visit##ExprName(const ExprName *) override;

  GEN_DCG_VISIT_EXPR_DECL(Stmt)
  GEN_DCG_VISIT_EXPR_DECL(BinaryExpr)
  GEN_DCG_VISIT_EXPR_DECL(Identifier)
  GEN_DCG_VISIT_EXPR_DECL(Integer)
  GEN_DCG_VISIT_EXPR_DECL(BrackExpr)
  GEN_DCG_VISIT_EXPR_DECL(ParenExpr)

#undef GEN_DCG_VISIT_EXPR_DECL

  void visitContraction(const Expr *E, const TupleList &Index);
  virtual void visitElementDirective(const ElementDirective *ED) override {}
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_NAIVECG_H
