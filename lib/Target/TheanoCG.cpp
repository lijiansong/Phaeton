//==--- TheanoCG.cpp ----- Interface to code generation for Theano ---------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for Theano code generation. Note that previous we
// generate code for NumPy.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/TheanoCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Support/ErrorHandling.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#include <vector>

using namespace phaeton;

void TheanoCG::genCode(const Program *Prog) {
  appendCode("from theano import function as theano_function\n");
  appendCode("import theano.tensor as " + getModulePrefix() + "\n\n");

  CG->visitProgram(Prog);
  appendCode("\n");

  const Sema &Sema = *getSema();

  std::map<const TensorType *, std::string> EmittedTypes;
  for (const auto *Id : CG->getDeclaredIds()) {
    assert(Id->isIdentifier() && INTERNAL_ERROR"expected 'IdentifierExpr'");

    const std::string &Name = Id->getName();
    const TensorType *Type = Sema.getType(Id->getDims());
    assert(Type && INTERNAL_ERROR "expected valid type");

    std::string TypeName;
    if (EmittedTypes.count(Type)) {
      TypeName = EmittedTypes[Type];
    } else {
      TypeName = Sema.isNamedType(Type) ? Sema.getTypeSymbol(Type)->getName()
                                        : getTemp();
      appendCode(TypeName + " = " + getModulePrefix() +
                 ".TensorType('float64', (False,)*" +
                 std::to_string((long long)Type->getRank()) + ")\n");
      EmittedTypes[Type] = TypeName;
    }
    appendCode(Name + " = " + getModulePrefix() + ".TensorVariable(" +
               TypeName + ")\n");
  }

  for (const auto &Assign : CG->getAssignments()) {
    assert(Assign.LHS->isIdentifier() &&
           INTERNAL_ERROR "LHS must be indentifier");
    const std::string Result = Assign.LHS->getName();
    const ExprNode *Node = Assign.RHS;

    // Note that we need this if stmt because code generation
    // for identifiers has been optimized out, we need to visit it.
    if (Node->isIdentifier())
      appendCode(Result + " = " + Node->getName());
    else {
      setResultTmp(Result);
      Node->visit(this);
    }
  }

  if (Sema.inputs_size() == 0 || Sema.outputs_size() == 0) {
    return;
  }

#define GEN_TH_IO_SYMBOL_LIST(InOut)                                           \
  std::string InOut##List;                                                     \
  {                                                                            \
    InOut##List = "[";                                                         \
    bool First = true;                                                         \
    for (auto It = Sema.InOut##_begin(), IE = Sema.InOut##_end(); It != IE;    \
         It++) {                                                               \
      const Symbol *Sym = *It;                                                 \
      if (!First)                                                              \
        InOut##List += ", ";                                                   \
      InOut##List += Sym->getName();                                           \
      First = false;                                                           \
    }                                                                          \
    InOut##List += "]";                                                        \
  }

  // Handle variables with 'in' specifier.
  std::string InsList = "[";
  bool First = true;
  auto It = Sema.inputs_begin(), IE = Sema.inputs_end();
  for (; It != IE; It++) {
    const Symbol *Sym = *It;
    if (!First) {
      InsList += ", ";
    }
    InsList += Sym->getName();
    First = false;
  }
  InsList += "]";

  const std::string &FunctionName = getCGFuncName();
  std::string Output;
  if (Sema.outputs_size() == 1) {
    const Symbol *Sym = *Sema.outputs_begin();
    appendCode(getTemp() + " = " + FunctionName + "(" + InsList + ", " +
               Sym->getName() + ")\n");
  } else {
    // Handle variables with 'out' specifier.
    std::string OutsList = "[";
    bool First = true;
    auto It = Sema.outputs_begin(), IE = Sema.outputs_end();
    for (; It != IE; It++) {
      const Symbol *Sym = *It;
      if (!First) {
        OutsList += ", ";
      }
      OutsList += Sym->getName();
      First = false;
    }
    OutsList += "]";
    appendCode(getTemp() + " = " + FunctionName + "(" + InsList + ", " +
               OutsList + ")\n");
  }

  for (auto In = Sema.inputs_begin(), IE = Sema.inputs_end(); In != IE; In++) {
    const Symbol *Sym = *In;
    addFuncArg(Sym->getName());
  }
}

void TheanoCG::visitBinOpExpr(const ExprNode *Node,
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

void TheanoCG::visitAddExpr(const AddExpr *E) { visitBinOpExpr(E, "+"); }

void TheanoCG::visitSubExpr(const SubExpr *E) { visitBinOpExpr(E, "-"); }

void TheanoCG::visitMulExpr(const MulExpr *E) { visitBinOpExpr(E, "*"); }

void TheanoCG::visitDivExpr(const DivExpr *E) { visitBinOpExpr(E, "/"); }

void TheanoCG::visitScalarMulExpr(const ScalarMulExpr *E) {
  visitBinOpExpr(E, "*");
}

void TheanoCG::visitScalarDivExpr(const ScalarDivExpr *E) {
  visitBinOpExpr(E, "/");
}

void TheanoCG::visitTensorDotExpr(const ExprNode *Node,
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

void TheanoCG::visitContractionExpr(const ContractionExpr *ContrExpr) {
  CodeGen::TupleList Axes;
  Axes.push_back(ContrExpr->getLeftIndices());
  Axes.push_back(ContrExpr->getRightIndices());
  visitTensorDotExpr(ContrExpr, CodeGen::getTupleListString(Axes));
}

void TheanoCG::visitProductExpr(const ProductExpr *E) {
  visitTensorDotExpr(E, "0");
}

void TheanoCG::visitStackExpr(const StackExpr *Expr) {
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

void TheanoCG::visitTranspositionExpr(const TranspositionExpr *E) {
  // FIXME: remove this later.
  appendCode("# transposition not implemented yet\n");
}

void TheanoCG::visitIdentifierExpr(const IdentifierExpr *E) {
  ph_unreachable(INTERNAL_ERROR "code generation for identifier has been optimized out");
}
