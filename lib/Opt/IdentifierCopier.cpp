//==--- IdentifierCopier.cpp ---- Interface to IdentifierCopier ------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the IdentifierCopier class.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/IdentifierCopier.h"

using namespace phaeton;

void IdentifierCopier::transformChildren(ExpressionNode *Node) {
  ExpressionNode *SavedParent = Parent;
  unsigned SavedChildIndex = ChildIndex;

  Parent = Node;
  for (int I = 0; I < Node->getNumChildren(); ++I) {
    ChildIndex = I;
    Node->getChild(I)->transform(this);
  }

  ChildIndex = SavedChildIndex;
  Parent = SavedParent;
}

void IdentifierCopier::transformAssignments() {
  for (CurrentPos = Assignments.begin(); CurrentPos != Assignments.end();
       ++CurrentPos) {
    CurrentLHS = static_cast<const IdentifierExpr *>(CurrentPos->LHS);
    // Note that index structures on 'LHS' and 'RHS' are definitely
    // incompatible if indices on the 'LHS' are permuted, so we need to make
    // some transformation.
    if (CurrentLHS->getPermute()) {
      Incompatible = true;
    } else {
      Incompatible = false;
    }
    Parent = nullptr;
    ChildIndex = -1;
    ReplaceName = "";
    CurrentPos->RHS->transform(this);
  }
}

/// These methods handle operations that leave index structures intact,
/// including element-wise and those scalar operations.
#define GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(ExprName)                      \
  void IdentifierCopier::transform##ExprName##Expr(ExprName##Expr *E) {        \
    transformChildren(E);                                                      \
  }

GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Add)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Sub)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Mul)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(ScalarMul)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Div)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(ScalarDiv)

#undef GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL

/// These methods handle operations that change index structure, including
/// contraction, product, stack and transposition operation.
#define GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(ExprName)                    \
  void IdentifierCopier::transform##ExprName##Expr(ExprName##Expr *E) {        \
    bool savedIncompatible = Incompatible;                                     \
    Incompatible = true;                                                       \
    transformChildren(E);                                                      \
    Incompatible = savedIncompatible;                                          \
  }
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Contraction)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Product)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Stack)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Transposition)

#undef GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL

void IdentifierCopier::transformIdentifierExpr(IdentifierExpr *Node) {
  // Note that current identifier should be different from 'LHS'.
  if (Node->getName() != CurrentLHS->getName()) {
    return;
  }

  // Note that identifiers with the same name must have the same type, here we
  // only check the dimension.
  assert(Node->getDims() == CurrentLHS->getDims());

  // If index structures of 'LHS' and the current identifier are compatible
  if (!Incompatible) {
    return;
  }

  // If we get here, that means current identifier must be replaced.
  // If there isn't a replacement for the identifier yet, we need to create a
  // name for it.
  const std::string Tmp = (ReplaceName != "") ? ReplaceName : getTmp();

  ExpressionNode *NewIdNode =
      getExprNodeBuilder()->createIdentifierExpr(Tmp, Node->getDims());
  Parent->setChild(ChildIndex, NewIdNode);

  // Note that if there wasn't a replacement identifier, we can simply assign
  // 'LHS' to it.
  if (ReplaceName == "") {
    ReplaceName = Tmp;
    ExpressionNode *CopyLHSId = getExprNodeBuilder()->createIdentifierExpr(
        CurrentLHS->getName(), CurrentLHS->getDims());
    ExpressionNode *NewRHSId = getExprNodeBuilder()->createIdentifierExpr(
        ReplaceName, Node->getDims());
    // Note: Replacement identifier for 'RHS' must be assigned the
    // value of 'LHS' before current assignment.
    // TODO: To make the insertion clear, we need to add a wrapper for
    // insertBefore and insertAfter.
    Assignments.insert(CurrentPos, {NewRHSId, CopyLHSId});
  }
}
