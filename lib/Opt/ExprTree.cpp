#include <cassert>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include "ph/Opt/ExprTree.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/OptUtils.h"

std::map<ExprNode::ExprKind, std::string> ExprNode::ExprLabel = {
    {EK_Add, "Add"},
    {EK_Sub, "Sub"},
    {EK_Mul, "Mul"},
    {EK_Div, "Div"},
    {EK_Contraction, "Contraction"},
    {EK_Product, "Product"},
    {EK_Stack, "Stack"},
    {EK_Identifier, "Identifier"}};

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

#define DEF_EXPR_NODE_CLASS_VISIT(Kind)                                        \
  void Kind##Expr::visit(ExprTreeVisitor *v) const {                           \
    v->visit##Kind##Expr(this);                                                \
  }

DEF_EXPR_NODE_CLASS_VISIT(Add)
DEF_EXPR_NODE_CLASS_VISIT(Sub)
DEF_EXPR_NODE_CLASS_VISIT(Mul)
DEF_EXPR_NODE_CLASS_VISIT(Div)
DEF_EXPR_NODE_CLASS_VISIT(Product)
DEF_EXPR_NODE_CLASS_VISIT(Contraction)
DEF_EXPR_NODE_CLASS_VISIT(Stack)
DEF_EXPR_NODE_CLASS_VISIT(Identifier)

#undef DEF_EXPR_NODE_CLASS_VISIT

#define DEF_EXPR_NODE_CLASS_TRANSFORM(Kind)                                    \
  void Kind##Expr::transform(ExprTreeTransformer *t) {                         \
    t->transform##Kind##Expr(this);                                            \
  }

DEF_EXPR_NODE_CLASS_TRANSFORM(Add)
DEF_EXPR_NODE_CLASS_TRANSFORM(Sub)
DEF_EXPR_NODE_CLASS_TRANSFORM(Mul)
DEF_EXPR_NODE_CLASS_TRANSFORM(Div)
DEF_EXPR_NODE_CLASS_TRANSFORM(Product)
DEF_EXPR_NODE_CLASS_TRANSFORM(Contraction)
DEF_EXPR_NODE_CLASS_TRANSFORM(Stack)
DEF_EXPR_NODE_CLASS_TRANSFORM(Identifier)

#undef DEF_EXPR_NODE_CLASS_TRANSFORM

ProductExpr::ProductExpr(ExprNode *lhs, ExprNode *rhs)
    : ExprNode(EK_Product, 2) {
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
    : ExprNode(EK_Contraction, 2), LeftIndices(leftIndices),
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
    : ExprNode(EK_Stack, members.size()) {
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

#define DEF_BUILDER_CREATE_EXPR_NODE(Kind)                                     \
  Kind##Expr *ExprNodeBuilder::create##Kind##Expr(ExprNode *lhs,               \
                                                  ExprNode *rhs) {             \
    Kind##Expr *result = Kind##Expr::create(lhs, rhs);                         \
    AllocatedNodes.insert(result);                                             \
    return result;                                                             \
  }

DEF_BUILDER_CREATE_EXPR_NODE(Add)
DEF_BUILDER_CREATE_EXPR_NODE(Sub)
DEF_BUILDER_CREATE_EXPR_NODE(Mul)
DEF_BUILDER_CREATE_EXPR_NODE(Div)
DEF_BUILDER_CREATE_EXPR_NODE(Product)

#undef DEF_BUILDER_CREATE_EXPR_NODE

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
