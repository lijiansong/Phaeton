//==------ ExprTree.cpp - Union class for expression tree ------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of ExpressionNode.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/TensorExprTree.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Opt/OptUtils.h"

#include <cassert>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

using namespace phaeton;

std::map<ExpressionNode::ExprKind, std::string> ExpressionNode::ExprLabel = {
    {EXPR_KIND_Add, "Add"},
    {EXPR_KIND_Sub, "Sub"},
    {EXPR_KIND_Mul, "Mul"},
    {EXPR_KIND_ScalarMul, "ScalarMul"},
    {EXPR_KIND_Div, "Div"},
    {EXPR_KIND_ScalarDiv, "ScalarDiv"},
    {EXPR_KIND_Contraction, "Contraction"},
    {EXPR_KIND_Product, "Product"},
    {EXPR_KIND_Stack, "Stack"},
    {EXPR_KIND_Transposition, "Transposition"},
    {EXPR_KIND_Identifier, "Identifier"}};

ExpressionNode::ExpressionNode(ExprKind Kind, int NumChildren,
                               const ExprDims &ED)
    : ExKind(Kind), NumChildren(NumChildren), Dims(ED) {
  for (int I = 0; I < getNumChildren(); ++I) {
    Children.push_back(nullptr);
  }
}

void ExpressionNode::dump(unsigned Indent) const {
  std::string Str = ExprLabel[getExprKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";

  for (int I = 0; I < getNumChildren(); ++I)
    getChild(I)->dump(Indent + Str.length() + 1);

  FORMAT_OPT_INDENT(Indent + 1)
  std::cout << ")\n";
}

void ExpressionNode::_delete() const {
  for (int I = 0; I < getNumChildren(); ++I) {
    getChild(I)->_delete();
    delete getChild(I);
  }
}

#define GEN_EXPR_NODE_CLASS_VISIT_IMPL(ExprName)                               \
  void ExprName##Expr::visit(ExprTreeVisitor *Visitor) const {                 \
    Visitor->visit##ExprName##Expr(this);                                      \
  }

GEN_EXPR_NODE_CLASS_VISIT_IMPL(Add)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Sub)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Mul)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(ScalarMul)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Div)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(ScalarDiv)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Product)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Contraction)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Stack)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Transposition)
GEN_EXPR_NODE_CLASS_VISIT_IMPL(Identifier)

#undef GEN_EXPR_NODE_CLASS_VISIT_IMPL

#define GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(ExprName)                           \
  void ExprName##Expr::transform(ExprTreeTransformer *T) {                     \
    T->transform##ExprName##Expr(this);                                        \
  }

GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Add)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Sub)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Mul)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(ScalarMul)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Div)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(ScalarDiv)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Product)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Contraction)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Stack)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Transposition)
GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL(Identifier)

#undef GEN_EXPR_NODE_CLASS_TRANSFORM_IMPL

ScalarMulExpr::ScalarMulExpr(ExpressionNode *LHS, ExpressionNode *RHS)
    : ExpressionNode(EXPR_KIND_ScalarMul, /*NumChildren=*/2, RHS->getDims()) {
  setChild(0, LHS);
  setChild(1, RHS);
}

ScalarDivExpr::ScalarDivExpr(ExpressionNode *LHS, ExpressionNode *RHS)
    : ExpressionNode(EXPR_KIND_ScalarDiv, /*NumChildren=*/2, LHS->getDims()) {
  setChild(0, LHS);
  setChild(1, RHS);
}

ProductExpr::ProductExpr(ExpressionNode *LHS, ExpressionNode *RHS)
    : ExpressionNode(EXPR_KIND_Product, /*NumChildren=*/2) {
  setChild(0, LHS);
  setChild(1, RHS);

  ExprDims LHSDims = LHS->getDims();
  const ExprDims &RHSDims = RHS->getDims();
  for (int I = 0; I < RHSDims.size(); ++I)
    LHSDims.push_back(RHSDims[I]);

  setDims(LHSDims);
}

ContractionExpr::ContractionExpr(ExpressionNode *LHS,
                                 const CodeGen::List &LeftIndices,
                                 ExpressionNode *RHS,
                                 const CodeGen::List &RightIndices)
    : ExpressionNode(EXPR_KIND_Contraction, /*NumChildren=*/2),
      LeftIndices(LeftIndices), RightIndices(RightIndices) {
  setChild(0, LHS);
  setChild(1, RHS);

  ExprDims LHSDims = LHS->getDims();
  ExprDims RHSDims = RHS->getDims();

  std::list<int> LeftIndicesList(LeftIndices.begin(), LeftIndices.end());
  LeftIndicesList.sort();
  std::list<int> RightIndicesList(RightIndices.begin(), RightIndices.end());
  RightIndicesList.sort();
  // Note that the following works at the precondition that 'LeftIndicesList'
  // and 'RightIndicesList' is sorted.
  int Erased = 0;
  for (const int Index : LeftIndicesList) {
    LHSDims.erase(LHSDims.begin() + Index - (Erased++));
  }
  Erased = 0;
  for (const int Index : RightIndicesList) {
    RHSDims.erase(RHSDims.begin() + Index - (Erased++));
  }

  for (int I = 0; I < RHSDims.size(); ++I) {
    LHSDims.push_back(RHSDims[I]);
  }

  setDims(LHSDims);
}

void ContractionExpr::dump(unsigned Indent) const {
  std::string Str = ExprLabel[getExprKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";

  getChild(0)->dump(Indent + Str.length() + 1);
  FORMAT_OPT_INDENT(Indent + Str.length() + 1)
  std::cout << CodeGen::getListString(getLeftIndices()) << "\n";

  getChild(1)->dump(Indent + Str.length() + 1);
  FORMAT_OPT_INDENT(Indent + Str.length() + 1)
  std::cout << CodeGen::getListString(getRightIndices()) << "\n";

  FORMAT_OPT_INDENT(Indent + 1)
  std::cout << ")\n";
}

StackExpr::StackExpr(const std::vector<ExpressionNode *> &Mems)
    : ExpressionNode(EXPR_KIND_Stack, Mems.size()) {
  for (int I = 0; I < Mems.size(); ++I) {
    setChild(I, Mems[I]);
  }

  ExprDims Dims;
  Dims.push_back(Mems.size());
  if (Mems.size()) {
    // For type checking, all members must have the same dimensions,
    // so we can use only the 0-th member here.
    const ExprDims &MemberDims = Mems[0]->getDims();
    for (int I = 0; I < MemberDims.size(); ++I) {
      Dims.push_back(MemberDims[I]);
    }
  }

  setDims(Dims);
}

TranspositionExpr::TranspositionExpr(ExpressionNode *Node,
                                     const CodeGen::TupleList &IndexPairs)
    : ExpressionNode(EXPR_KIND_Transposition, /*NumChildren=*/1),
      IndexPairs(IndexPairs) {
  setChild(0, Node);

  ExprDims DimsToTranspose = Node->getDims();
  for (const auto &Pair : IndexPairs) {
    assert(Pair.size() == 2);
    const int Dim0 = DimsToTranspose[Pair[0]];
    DimsToTranspose[Pair[0]] = DimsToTranspose[Pair[1]];
    DimsToTranspose[Pair[1]] = Dim0;
  }

  setDims(DimsToTranspose);
}

void TranspositionExpr::dump(unsigned Indent) const {
  std::string Str = ExprLabel[getExprKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";

  getChild(0)->dump(Indent + Str.length() + 1);
  FORMAT_OPT_INDENT(Indent + Str.length() + 1)
  std::cout << CodeGen::getTupleListString(getIndexPairs()) << "\n";

  FORMAT_OPT_INDENT(Indent + 1)
  std::cout << ")\n";
}

void IdentifierExpr::dump(unsigned Indent) const {
  std::string Str = ExprLabel[getExprKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << " \"" << getName() << "\")\n";
}

const std::string IdentifierExpr::getIndex(unsigned I) const {
  assert(I < Indices.size() &&
         "internal error: index out of bounds for identifier indices");
  return Indices.at(I);
}
