//==--- JuliaCG.cpp ----- Interface to code generation for Julia -----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for Theano code generation. Note that previous we
// generate code for numpy. This is a preparation for Google TPUs code
// generation.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/JuliaCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"
#include "ph/Support/ErrorHandling.h"

#include <vector>

using namespace phaeton;

void JuliaCG::genCode(const Program *Prog) {}

void JuliaCG::visitBinOpExpr(const ExprNode *Node,
                             const std::string &Operation) {
  const std::string Result = getResultTmp();
  std::string Tmps[2];

  // Note that for those 'BinOp', they must have two operands.
  assert(Node->getNumChildren() == 2);
  for (int I = 0; I < 2; ++I) {
    if (Node->getChild(I)->isIdentifier()) {
      Tmps[I] = Node->getChild(I)->getName();
    } else {
      Tmps[I] = getTemp();
      setResultTmp(Tmps[I]);
      Node->getChild(I)->visit(this);
    }
  }

  appendCode(Result + " = " + Tmps[0] + " " + Operation + " " + Tmps[1] + "\n");
  setResultTmp(Result);
}

void JuliaCG::visitAddExpr(const AddExpr *E) { visitBinOpExpr(E, "+"); }

void JuliaCG::visitSubExpr(const SubExpr *E) { visitBinOpExpr(E, "-"); }

void JuliaCG::visitMulExpr(const MulExpr *E) { visitBinOpExpr(E, "*"); }

void JuliaCG::visitDivExpr(const DivExpr *E) { visitBinOpExpr(E, "/"); }

void JuliaCG::visitScalarMulExpr(const ScalarMulExpr *E) {
  visitBinOpExpr(E, "*");
}

void JuliaCG::visitScalarDivExpr(const ScalarDivExpr *E) {
  visitBinOpExpr(E, "/");
}

void JuliaCG::visitTensorDotExpr(const ExprNode *Node,
                                 const std::string &Axes) {
  const std::string Result = getResultTmp();
  std::string Tmps[2];

  assert(Node->getNumChildren() == 2);
  for (int I = 0; I < 2; ++I) {
    if (Node->getChild(I)->isIdentifier()) {
      Tmps[I] = Node->getChild(I)->getName();
    } else {
      Tmps[I] = getTemp();
      setResultTmp(Tmps[I]);
      Node->getChild(I)->visit(this);
    }
  }

  appendCode(Result + " = " + getModulePrefix() + ".tensordot(" + Tmps[0] +
             ", " + Tmps[1] + ", " + "axes=" + Axes + ")\n");
  setResultTmp(Result);
}

void JuliaCG::visitContractionExpr(const ContractionExpr *ContrExpr) {
  CodeGen::TupleList Axes;
  Axes.push_back(ContrExpr->getLeftIndices());
  Axes.push_back(ContrExpr->getRightIndices());
  visitTensorDotExpr(ContrExpr, CodeGen::getTupleListString(Axes));
}

void JuliaCG::visitProductExpr(const ProductExpr *E) {
  visitTensorDotExpr(E, "0");
}

void JuliaCG::visitStackExpr(const StackExpr *Expr) {
  std::string Result = getResultTmp();

  std::string Stack;
  for (int I = 0; I < Expr->getNumChildren(); ++I) {
    const ExprNode *Child = Expr->getChild(I);

    if (Child->isIdentifier()) {
      Stack += Child->getName();
    } else {
      const std::string Tmp = getTemp();
      setResultTmp(Tmp);
      Child->visit(this);
      Stack += Tmp;
    }

    if (I != Expr->getNumChildren() - 1)
      Stack += ", ";
  }
  appendCode(Result + " = " + getModulePrefix() + ".stack([" + Stack + "])\n");

  setResultTmp(Result);
}

void JuliaCG::visitTranspositionExpr(const TranspositionExpr *E) {
  // FIXME: remove this later.
  appendCode("# transposition not implemented yet\n");
}

void JuliaCG::visitIdentifierExpr(const IdentifierExpr *E) {
  ph_unreachable(INTERNAL_ERROR
                 "code generation for identifier has been optimized out");
}
