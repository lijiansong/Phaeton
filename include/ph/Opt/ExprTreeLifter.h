//==------ ExprTreeLifter.h - Representation of expr tree lifer ------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression tree lifter. Expression tree
// lifter lifts tensor 'Contraction' and 'Stack' nodes out of expression
// trees and replaces them with temporary variables, which can help
// generate better code.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_EXPR_TREE_LIFTER_H
#define PHAETON_OPT_EXPR_TREE_LIFTER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/TensorExprTree.h"

#include <functional>
#include <list>
#include <string>

namespace phaeton {

/// ExprTreeLifter - This class can lift nodes out of expression trees and
/// replace them with temporary variables. This can help generate better C code.
class ExprTreeLifter : public ExprTreeTransformer {
public:
  using LiftPredicate =
      std::function<bool(const ExpressionNode *, const ExpressionNode *)>;
  ExprTreeLifter(CodeGen *Gen, const LiftPredicate &Predicate)
      : CG(Gen), Assignments(CG->getAssignments()),
        IsNodeToBeLifted(Predicate) {}

#define GEN_TRANSFORM_EXPR_NODE_DECL(ExprName)                                 \
  virtual void transform##ExprName##Expr(ExprName##Expr *E) override;

  GEN_TRANSFORM_EXPR_NODE_DECL(Add)
  GEN_TRANSFORM_EXPR_NODE_DECL(Sub)
  GEN_TRANSFORM_EXPR_NODE_DECL(Mul)
  GEN_TRANSFORM_EXPR_NODE_DECL(ScalarMul)
  GEN_TRANSFORM_EXPR_NODE_DECL(Div)
  GEN_TRANSFORM_EXPR_NODE_DECL(ScalarDiv)
  GEN_TRANSFORM_EXPR_NODE_DECL(Contraction)
  GEN_TRANSFORM_EXPR_NODE_DECL(Product)
  GEN_TRANSFORM_EXPR_NODE_DECL(Stack)
  GEN_TRANSFORM_EXPR_NODE_DECL(Transposition)
  GEN_TRANSFORM_EXPR_NODE_DECL(Identifier)

#undef GEN_TRANSFORM_EXPR_NODE_DECL

  void transformNode(ExpressionNode *);
  void transformChildren(ExpressionNode *);
  void liftNode(ExpressionNode *);

  void transformAssignments();

protected:
  void setRoot(ExpressionNode *Node) { Root = Node; }
  ExpressionNode *getRoot() const { return Root; }

  void setParent(ExpressionNode *Node) { Parent = Node; }
  ExpressionNode *getParent() const { return Parent; }

  void setChildIndex(int Index) { ChildIndex = Index; }
  int getChildIndex() const { return ChildIndex; }

  /// helper methods that implement functionality from the code generator 'CG'
private:
  std::string getTmpWithDims(const std::vector<int> &Dims) {
    return CG->getTmpWithDims(Dims);
  }
  void freeTmpWithDims(const std::string &Name, const std::vector<int> &Dims) {
    CG->freeTmpWithDims(Name, Dims);
  }
  ExpressionNodeBuilder *getExprNodeBuilder() {
    return CG->getExprNodeBuilder();
  }
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;
  ExpressionNode *Root;
  ExpressionNode *Parent;
  int ChildIndex;
  CodeGen::AssignmentsListTy::iterator CurrentPos;
  const LiftPredicate IsNodeToBeLifted;
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_TREE_LIFTER_H
