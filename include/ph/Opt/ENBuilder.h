//==------ ENBuilder.h - Representation of expression node builder ---------==//
//
// This file defines interfaces of expression node builder. Expression node
// builder is a factory class for memory management and memory scheduling.
// For DNN accelerator, we will hide the memory hierarchy, only expose the
// off-chip memory for programmers.
//
//===----------------------------------------------------------------------===//

#ifndef __EXPR_NODE_BUILDER_H__
#define __EXPR_NODE_BUILDER_H__

#include <set>
#include "ph/Opt/ExprTree.h"

class ExprNodeBuilder {
private:
  std::set<const ExprNode *> AllocatedNodes;

public:
  ~ExprNodeBuilder();

#define DECL_BUILDER_CREATE_EXPR_NODE(Kind)                                    \
  Kind##Expr *create##Kind##Expr(ExprNode *lhs, ExprNode *rhs);

  DECL_BUILDER_CREATE_EXPR_NODE(Add)
  DECL_BUILDER_CREATE_EXPR_NODE(Sub)
  DECL_BUILDER_CREATE_EXPR_NODE(Mul)
  DECL_BUILDER_CREATE_EXPR_NODE(Div)
  DECL_BUILDER_CREATE_EXPR_NODE(Product)

#undef DECL_BUILDER_CREATE_EXPR_NODE

  ContractionExpr *createContractionExpr(ExprNode *lhs,
                                         const CodeGen::List &leftIndices,
                                         ExprNode *rhs,
                                         const CodeGen::List &rightIndices);

  StackExpr *createStackExpr(const std::vector<ExprNode *> &members);
  IdentifierExpr *createIdentifierExpr(const std::string &name,
                                       const ExprNode::ExprDimensions &dims);
};

#endif // __EXPR_NODE_BUILDER_H__
