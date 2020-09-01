//==------ ENBuilder.cpp - Interface to base expression node builder -------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements interfaces of base class ExpressionNodeBuilder.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/ENBuilder.h"

using namespace phaeton;

#define GEN_BUILDER_CREATE_EXPR_NODE_IMPL(ExprName)                            \
  ExprName##Expr *ExpressionNodeBuilder::create##ExprName##Expr(               \
      ExpressionNode *lhs, ExpressionNode *rhs) {                              \
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

ContractionExpr *ExpressionNodeBuilder::createContractionExpr(
    ExpressionNode *LHS, const CodeGen::List &LeftIndex, ExpressionNode *RHS,
    const CodeGen::List &RightIndex) {
  ContractionExpr *Res =
      ContractionExpr::create(LHS, LeftIndex, RHS, RightIndex);
  AllocatedNodes.insert(Res);
  return Res;
}

StackExpr *ExpressionNodeBuilder::createStackExpr(
    const std::vector<ExpressionNode *> &Mems) {
  StackExpr *Res = StackExpr::create(Mems);
  AllocatedNodes.insert(Res);
  return Res;
}

TranspositionExpr *ExpressionNodeBuilder::createTranspositionExpr(
    ExpressionNode *Node, const CodeGen::TupleList &IndexPairs) {
  TranspositionExpr *Res = TranspositionExpr::create(Node, IndexPairs);
  AllocatedNodes.insert(Res);
  return Res;
}

IdentifierExpr *ExpressionNodeBuilder::createIdentifierExpr(
    const std::string &Name, const ExpressionNode::ExprDims &ED) {
  IdentifierExpr *Res = IdentifierExpr::create(Name, ED);
  AllocatedNodes.insert(Res);
  return Res;
}
