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
#include "ph/Opt/ExprTree.h"
#include "ph/Opt/ExprTreeTransformer.h"

#include <functional>
#include <list>
#include <string>

namespace phaeton {

/// ExprTreeLifter - This class can lift nodes out of expression trees and
/// replace them with temporary variables. This can help generate better C code.
class ExprTreeLifter : public ExprTreeTransformer {
public:
  using LiftPredicate = std::function<bool(const ExprNode *, const ExprNode *)>;
  ExprTreeLifter(CodeGen *cg, const LiftPredicate &lp)
      : CG(cg), Assignments(CG->getAssignments()), isNodeToBeLifted(lp) {}

#define GEN_TRANSFORM_EXPR_NODE_DECL(ExprName)                                 \
  virtual void transform##ExprName##Expr(ExprName##Expr *en) override;

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

  void transformNode(ExprNode *en);
  void transformChildren(ExprNode *en);
  void liftNode(ExprNode *en);

  void transformAssignments();

protected:
  void setRoot(ExprNode *r) { Root = r; }
  ExprNode *getRoot() const { return Root; }

  void setParent(ExprNode *p) { Parent = p; }
  ExprNode *getParent() const { return Parent; }

  void setChildIndex(int index) { ChildIndex = index; }
  int getChildIndex() const { return ChildIndex; }

  // helper methods that implement functionality from the code generator 'cg'
private:
  std::string getTempWithDims(const std::vector<int> &dims) {
    return CG->getTempWithDims(dims);
  }
  void freeTempWithDims(const std::string &name, const std::vector<int> &dims) {
    CG->freeTempWithDims(name, dims);
  }
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;
  ExprNode *Root;
  ExprNode *Parent;
  int ChildIndex;
  CodeGen::AssignmentsListTy::iterator curPos;
  const LiftPredicate isNodeToBeLifted;
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_TREE_LIFTER_H
