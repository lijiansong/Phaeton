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

std::map<ExprNode::ExprKind, std::string> ExprNode::ExprLabel = {
    {ExprNode_Add, "Add"},
    {ExprNode_Sub, "Sub"},
    {ExprNode_Mul, "Mul"},
    {ExprNode_Div, "Div"},
    {ExprNode_Contraction, "Contraction"},
    {ExprNode_Product, "Product"},
    {ExprNode_Stack, "Stack"},
    {ExprNode_ScalarMul, "ScalarMul"},
    {ExprNode_ScalarDiv, "ScalarDiv"},
    {ExprNode_Identifier, "Identifier"}};

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

#define GEN_EXPR_NODE_VISIT_IMPL(Kind)                                         \
  void Kind##Expr::visit(ExprTreeVisitor *v) const {                           \
    v->visit##Kind##Expr(this);                                                \
  }

GEN_EXPR_NODE_VISIT_IMPL(Add)
GEN_EXPR_NODE_VISIT_IMPL(Sub)
GEN_EXPR_NODE_VISIT_IMPL(Mul)
GEN_EXPR_NODE_VISIT_IMPL(Div)
GEN_EXPR_NODE_VISIT_IMPL(Product)
GEN_EXPR_NODE_VISIT_IMPL(Contraction)
GEN_EXPR_NODE_VISIT_IMPL(Stack)
GEN_EXPR_NODE_VISIT_IMPL(Identifier)
GEN_EXPR_NODE_VISIT_IMPL(ScalarMul)
GEN_EXPR_NODE_VISIT_IMPL(ScalarDiv)

#undef GEN_EXPR_NODE_VISIT_IMPL

#define GEN_EXPR_NODE_TRANSFORM_IMPL(Kind)                                     \
  void Kind##Expr::transform(ExprTreeTransformer *t) {                         \
    t->transform##Kind##Expr(this);                                            \
  }

GEN_EXPR_NODE_TRANSFORM_IMPL(Add)
GEN_EXPR_NODE_TRANSFORM_IMPL(Sub)
GEN_EXPR_NODE_TRANSFORM_IMPL(Mul)
GEN_EXPR_NODE_TRANSFORM_IMPL(Div)
GEN_EXPR_NODE_TRANSFORM_IMPL(Product)
GEN_EXPR_NODE_TRANSFORM_IMPL(Contraction)
GEN_EXPR_NODE_TRANSFORM_IMPL(Stack)
GEN_EXPR_NODE_TRANSFORM_IMPL(Identifier)
GEN_EXPR_NODE_TRANSFORM_IMPL(ScalarMul)
GEN_EXPR_NODE_TRANSFORM_IMPL(ScalarDiv)

#undef GEN_EXPR_NODE_TRANSFORM_IMPL

ScalarMulExpr::ScalarMulExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(ExprNode_ScalarMul, /*num_children*/ 2, rhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}

ScalarDivExpr::ScalarDivExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(ExprNode_ScalarDiv, /*num_children*/ 2, lhs->getDims()) {
  setChild(0, lhs);
  setChild(1, rhs);
}

ProductExpr::ProductExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(ExprNode_Product, 2) {
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
    : ExprNode(ExprNode_Contraction, 2), LeftIndices(leftIndices),
      RightIndices(rightIndices) {
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

  getChild(1)->dump(indent + str.length() + 1);
  FORMAT_OPT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getLeftIndices()) << "\n";

  getChild(0)->dump(indent + str.length() + 1);
  FORMAT_OPT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getRightIndices()) << "\n";

  FORMAT_OPT_INDENT(indent + 1)
  std::cout << ")\n";
}

StackExpr::StackExpr(const std::vector<ExprNode *> &members)
    : ExprNode(ExprNode_Stack, members.size()) {
  for (int i = 0; i < members.size(); i++)
    setChild(i, members[i]);

  ExprDimensions dims;
  dims.push_back(members.size());
  if (members.size()) {
    // For type checking, all members must have the same dimensions;
    const ExprDimensions &memberDims = members[0]->getDims();
    for (int i = 0; i < memberDims.size(); i++)
      dims.push_back(memberDims[i]);
  }

  setDims(dims);
}

void IdentifierExpr::dump(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_OPT_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getName() << "\")\n";
}

ExprNodeBuilder::~ExprNodeBuilder() {
  for (auto *en : AllocatedNodes)
    delete en;
}

#define GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Kind)                                \
  Kind##Expr *ExprNodeBuilder::create##Kind##Expr(ExprNode *lhs,               \
                                                  ExprNode *rhs) {             \
    Kind##Expr *result = Kind##Expr::create(lhs, rhs);                         \
    AllocatedNodes.insert(result);                                             \
    return result;                                                             \
  }

GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Add)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Sub)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Mul)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Div)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Product)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ScalarMul)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ScalarDiv)

#undef GEN_BUILDER_CREATE_EXPR_NODE_IMPL

ContractionExpr *ExprNodeBuilder::createContractionExpr(
    ExprNode *lhs, const CodeGen::List &leftIndices, ExprNode *rhs,
    const CodeGen::List &rightIndices) {
  ContractionExpr *result =
      ContractionExpr::create(lhs, leftIndices, rhs, rightIndices);
  AllocatedNodes.insert(result);
  return result;
}

StackExpr *
ExprNodeBuilder::createStackExpr(const std::vector<ExprNode *> &members) {

  StackExpr *result = StackExpr::create(members);
  AllocatedNodes.insert(result);
  return result;
}

IdentifierExpr *
ExprNodeBuilder::createIdentifierExpr(const std::string &name,
                                      const ExprNode::ExprDimensions &dims) {
  IdentifierExpr *result = IdentifierExpr::create(name, dims);
  AllocatedNodes.insert(result);
  return result;
}
