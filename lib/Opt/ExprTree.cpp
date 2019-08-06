//==------ ExprTree.cpp - Union class for expression tree ------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of ExprNode.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/ExprTree.h"
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

std::map<ExprNode::ExprKind, std::string> ExprNode::ExprLabel = {
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

ExprNode::ExprNode(ExprKind ek, int numChildren, const ExprDimensions &dims)
    : ExKind(ek), NumChildren(numChildren), Dims(dims) {
  for (int i = 0; i < getNumChildren(); i++)
    Children.push_back(nullptr);
}

void ExprNode::dump(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  for (int i = 0; i < getNumChildren(); i++)
    getChild(i)->dump(indent + str.length() + 1);

  FORMAT_OPT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ExprNode::_delete() const {
  for (int i = 0; i < getNumChildren(); i++) {
    getChild(i)->_delete();
    delete getChild(i);
  }
}

#define GEN_EXPR_NODE_CLASS_VISIT_IMPL(ExprName)                               \
  void ExprName##Expr::visit(ExprTreeVisitor *v) const {                       \
    v->visit##ExprName##Expr(this);                                            \
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
  void ExprName##Expr::transform(ExprTreeTransformer *t) {                     \
    t->transform##ExprName##Expr(this);                                        \
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

ScalarMulExpr::ScalarMulExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(EXPR_KIND_ScalarMul, /*num_children=*/2, rhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}

ScalarDivExpr::ScalarDivExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(EXPR_KIND_ScalarDiv, /*num_children=*/2, lhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}

ProductExpr::ProductExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(EXPR_KIND_Product, /*num_children=*/2) {
  setChild(0, lhs);
  setChild(1, rhs);

  ExprDimensions lhsDims = lhs->getDims();
  const ExprDimensions &rhsDims = rhs->getDims();
  for (int i = 0; i < rhsDims.size(); i++)
    lhsDims.push_back(rhsDims[i]);

  setDims(lhsDims);
}

ContractionExpr::ContractionExpr(ExprNode *lhs,
                                 const CodeGen::List &leftIndices,
                                 ExprNode *rhs,
                                 const CodeGen::List &rightIndices)
    : ExprNode(EXPR_KIND_Contraction, /*num_children=*/2),
      LeftIndices(leftIndices), RightIndices(rightIndices) {
  setChild(0, lhs);
  setChild(1, rhs);

  ExprDimensions lhsDims = lhs->getDims();
  ExprDimensions rhsDims = rhs->getDims();

  std::list<int> leftIndicesList(leftIndices.begin(), leftIndices.end());
  leftIndicesList.sort();
  std::list<int> rightIndicesList(rightIndices.begin(), rightIndices.end());
  rightIndicesList.sort();
  // The following works at the precondition that 'leftIndicesList'
  // and 'rightIndicesList' is sorted.
  int erased = 0;
  for (const int index : leftIndicesList) {
    lhsDims.erase(lhsDims.begin() + index - (erased++));
  }
  erased = 0;
  for (const int index : rightIndicesList) {
    rhsDims.erase(rhsDims.begin() + index - (erased++));
  }

  for (int i = 0; i < rhsDims.size(); i++)
    lhsDims.push_back(rhsDims[i]);

  setDims(lhsDims);
}

void ContractionExpr::dump(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  getChild(0)->dump(indent + str.length() + 1);
  FORMAT_OPT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getLeftIndices()) << "\n";

  getChild(1)->dump(indent + str.length() + 1);
  FORMAT_OPT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getRightIndices()) << "\n";

  FORMAT_OPT_INDENT(indent + 1)
  std::cout << ")\n";
}

StackExpr::StackExpr(const std::vector<ExprNode *> &members)
    : ExprNode(EXPR_KIND_Stack, members.size()) {
  for (int i = 0; i < members.size(); i++)
    setChild(i, members[i]);

  ExprDimensions dims;
  dims.push_back(members.size());
  if (members.size()) {
    // For type checking, all members must have the same dimensions,
    // so we can use only the 0-th member here.
    const ExprDimensions &memberDims = members[0]->getDims();
    for (int i = 0; i < memberDims.size(); i++)
      dims.push_back(memberDims[i]);
  }

  setDims(dims);
}

TranspositionExpr::TranspositionExpr(ExprNode *en,
                                     const CodeGen::TupleList &indexPairs)
    : ExprNode(EXPR_KIND_Transposition, /*num_children=*/1),
      IndexPairs(indexPairs) {
  setChild(0, en);

  ExprDimensions dimsToTranspose = en->getDims();
  for (const auto &p : indexPairs) {
    assert(p.size() == 2);
    const int dim0 = dimsToTranspose[p[0]];
    dimsToTranspose[p[0]] = dimsToTranspose[p[1]];
    dimsToTranspose[p[1]] = dim0;
  }

  setDims(dimsToTranspose);
}

void TranspositionExpr::dump(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  getChild(0)->dump(indent + str.length() + 1);
  FORMAT_OPT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getTupleListString(getIndexPairs()) << "\n";

  FORMAT_OPT_INDENT(indent + 1)
  std::cout << ")\n";
}

void IdentifierExpr::dump(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getName() << "\")\n";
}

const std::string IdentifierExpr::getIndex(unsigned i) const {
  assert(i < Indices.size() &&
         "internal error: index out of bounds for identifier indices");
  return Indices.at(i);
}
