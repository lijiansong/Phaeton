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

#ifndef PHAETON_OPT_EXPR_NODE_BUILDER_H
#define PHAETON_OPT_EXPR_NODE_BUILDER_H

#include "ph/Opt/ExprTree.h"

#include <set>

namespace phaeton {

/// ExprNodeBuilder - This class is used to keep track of memory that has been
/// allocated for nodes of expression trees.
class ExprNodeBuilder {
public:
  ~ExprNodeBuilder();

#define GEN_BUILDER_CREATE_EXPR_NODE_DECL(ExprName)                            \
  ExprName##Expr *create##ExprName##Expr(ExprNode *lhs, ExprNode *rhs);

  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Add)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Sub)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Mul)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(ScalarMul)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Div)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(ScalarDiv)
  GEN_BUILDER_CREATE_EXPR_NODE_DECL(Product)

#undef GEN_BUILDER_CREATE_EXPR_NODE_DECL

  ContractionExpr *createContractionExpr(ExprNode *lhs,
                                         const CodeGen::List &leftIndices,
                                         ExprNode *rhs,
                                         const CodeGen::List &rightIndices);

  StackExpr *createStackExpr(const std::vector<ExprNode *> &members);

  TranspositionExpr *
  createTranspositionExpr(ExprNode *en, const CodeGen::TupleList &indexPairs);

  IdentifierExpr *createIdentifierExpr(const std::string &name,
                                       const ExprNode::ExprDimensions &dims);

private:
  /// set to keep track of memory that has been allocated for nodes of
  /// expression trees.
  std::set<const ExprNode *> AllocatedNodes;
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_NODE_BUILDER_H
