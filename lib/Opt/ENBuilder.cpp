//==------ ENBuilder.cpp - Interface to base expression node builder -------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of base class ExprNodeBuilder.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/ENBuilder.h"

using namespace phaeton;

ExprNodeBuilder::~ExprNodeBuilder() {
  for (auto *en : AllocatedNodes)
    delete en;
}

#define GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ExprName)                            \
  ExprName##Expr *ExprNodeBuilder::create##ExprName##Expr(ExprNode *lhs,       \
                                                          ExprNode *rhs) {     \
    ExprName##Expr *result = ExprName##Expr::create(lhs, rhs);                 \
    AllocatedNodes.insert(result);                                             \
    return result;                                                             \
  }

GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Add)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Sub)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Mul)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ScalarMul)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Div)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ScalarDiv)
GEN_BUILDER_CREATE_EXPR_NODE_IMPL(Product)

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

TranspositionExpr *
ExprNodeBuilder::createTranspositionExpr(ExprNode *en,
                                         const CodeGen::TupleList &indexPairs) {
  TranspositionExpr *result = TranspositionExpr::create(en, indexPairs);
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
