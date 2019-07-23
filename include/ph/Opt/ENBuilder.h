//==------ ENBuilder.h - Representation of expression node builder ---------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression node builder. Expression node
// builder is a factory class for memory management and memory scheduling.
// For DNN accelerator, we will hide the memory hierarchy, only expose the
// off-chip memory for programmers.
//
//===----------------------------------------------------------------------===//

#ifndef __EXPR_NODE_BUILDER_H__
#define __EXPR_NODE_BUILDER_H__

#include "ph/Opt/ExprTree.h"

#include <set>

class ExprNodeBuilder {
private:
  std::set<const ExprNode *> AllocatedNodes;

public:
  ~ExprNodeBuilder();

#define GEN_BUILDER_CREATE_EXPR_NODE_DECL(Kind)                                \
  Kind##Expr *create##Kind##Expr(ExprNode *lhs, ExprNode *rhs);

  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Add)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Sub)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Mul)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Div)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Product)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(ScalarMul)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(ScalarDiv)

#undef GEN_BUILDER_CREATE_EXPR_NODE_DECL

  ContractionExpr *createContractionExpr(ExprNode *lhs,
                                         const CodeGen::List &leftIndices,
                                         ExprNode *rhs,
                                         const CodeGen::List &rightIndices);

  StackExpr *createStackExpr(const std::vector<ExprNode *> &members);
  IdentifierExpr *createIdentifierExpr(const std::string &name,
                                       const ExprNode::ExprDimensions &dims);
};

#endif // __EXPR_NODE_BUILDER_H__
