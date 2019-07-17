#include <vector>

#include "ph/AST/AST.h"
#include "ph/CodeGen/PyCG.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

void NumpyEmitter::genCode(const Program *p) {
  append("# ----- Autogen kernel by Phaeton -----\n");
  append("import numpy as " + getModulePrefix() + "\n\n");

  CG->visitProgram(p);
  append("\n");

  const Sema &sema = *getSema();

  for (const auto *d : CG->getDeclarations()) {
    if (d->getNodeType() == ASTNode::NT_TypeDecl)
      continue;

    const std::string &name = d->getIdentifier()->getName();
    const Symbol *sym = sema.getSymbol(name);
    const TensorType *type = &sym->getType();

    std::string typeName;
    if (CG->typeEmitted(type)) {
      typeName = CG->getEmittedTypeName(type);
    } else {
      typeName = (sema.isNamedType(type)) ? sema.getTypeSymbol(type)->getName()
                                          : getTemp();

      append(typeName + " = " + getModulePrefix() +
             ".TensorType('float64', (False,)*" +
             std::to_string(type->getRank()) + ")\n");
      CG->addEmittedType(type, typeName);
    }

    append(name + " = " + getModulePrefix() + ".TensorVariable(" + typeName +
           ")\n");
  }

  for (const auto *s : CG->getStatements()) {
    const ExprNode *en = getExprNode(s->getExpr());
    const std::string result = s->getIdentifier()->getName();

    if (en->isIdentifier())
      append(result + " = " + en->getName());
    else {
      setResultTemp(result);
      en->visit(this);
    }
  }

  if (sema.inputs_size() == 0 || sema.outputs_size() == 0)
    return;

#define IO_SYMBOL_LIST(inout)                                                  \
  std::string inout##List;                                                     \
  {                                                                            \
    inout##List = "[";                                                         \
    bool first = true;                                                         \
    for (auto i = sema.inout##_begin(), e = sema.inout##_end(); i != e; i++) { \
      const Symbol *sym = *i;                                                  \
      if (!first)                                                              \
        inout##List += ", ";                                                   \
      inout##List += sym->getName();                                           \
      first = false;                                                           \
    }                                                                          \
    inout##List += "]";                                                        \
  }

  IO_SYMBOL_LIST(inputs)

  std::string output;
  if (sema.outputs_size() == 1) {
    const Symbol *sym = *sema.outputs_begin();
    append(getTemp() + " = theano_function(" + inputsList + ", " +
           sym->getName() + ")\n");
  } else {
    IO_SYMBOL_LIST(outputs)
    append(getTemp() + " = theano_function(" + inputsList + ", " + outputsList +
           ")\n");
  }
}

void NumpyEmitter::visitBinOpExpr(const ExprNode *en, const std::string &op) {
  const std::string result = getResultTemp();
  std::string temps[2];

  assert(en->getNumChildren() == 2);
  for (int i = 0; i < 2; i++) {
    if (en->getChild(i)->isIdentifier()) {
      temps[i] = en->getChild(i)->getName();
    } else {
      temps[i] = getTemp();
      setResultTemp(temps[i]);
      en->getChild(i)->visit(this);
    }
  }

  append(result + " = " + temps[0] + " " + op + " " + temps[1] + "\n");
  setResultTemp(result);
}

void NumpyEmitter::visitAddExpr(const AddExpr *en) { visitBinOpExpr(en, "+"); }

void NumpyEmitter::visitSubExpr(const SubExpr *en) { visitBinOpExpr(en, "-"); }

void NumpyEmitter::visitMulExpr(const MulExpr *en) { visitBinOpExpr(en, "*"); }

void NumpyEmitter::visitDivExpr(const DivExpr *en) { visitBinOpExpr(en, "/"); }

void NumpyEmitter::visitTensordotExpr(const ExprNode *en,
                                      const std::string &axes) {
  const std::string result = getResultTemp();
  std::string temps[2];

  assert(en->getNumChildren() == 2);
  for (int i = 0; i < 2; i++) {
    if (en->getChild(i)->isIdentifier()) {
      temps[i] = en->getChild(i)->getName();
    } else {
      temps[i] = getTemp();
      setResultTemp(temps[i]);
      en->getChild(i)->visit(this);
    }
  }

  append(result + " = " + getModulePrefix() + ".tensordot(" + temps[0] + ", " +
         temps[1] + ", " + "axes=" + axes + ")\n");
  setResultTemp(result);
}

void NumpyEmitter::visitContractionExpr(const ContractionExpr *en) {
  CodeGen::TupleList axes;
  axes.push_back(en->getLeftIndices());
  axes.push_back(en->getRightIndices());
  visitTensordotExpr(en, CodeGen::getTupleListString(axes));
}

void NumpyEmitter::visitProductExpr(const ProductExpr *en) {
  visitTensordotExpr(en, "0");
}

void NumpyEmitter::visitStackExpr(const StackExpr *en) {
  std::string result = getResultTemp();
  append(result + " = " + getModulePrefix() + ".stack([");

  for (int i = 0; i < en->getNumChildren(); i++) {
    const ExprNode *child = en->getChild(i);

    if (child->isIdentifier()) {
      append(child->getName());
    } else {
      const std::string temp = getTemp();
      setResultTemp(temp);
      child->visit(this);
      append(temp);
    }

    if (i != en->getNumChildren() - 1)
      append(", ");
  }

  append("])\n");
  setResultTemp(result);
}

void NumpyEmitter::visitIdentifierExpr(const IdentifierExpr *en) {
  assert(0 &&
         "internal error: code emission for identifier has been optimized out");
}

#if 0
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
#endif // if 0
