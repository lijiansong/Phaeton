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

void IdentifierCopier::transformAssignments() {
  for (curPos = Assignments.begin(); curPos != Assignments.end(); curPos++) {
    curLhs = static_cast<const IdentifierExpr *>(curPos->lhs);

    // The index structures on the 'lhs' and 'rhs' are definitely
    // incompatible if indices on the 'lhs' are permuted
    if (curLhs->permute())
      Incompatible = true;
    else
      Incompatible = false;

    Parent = nullptr;
    ChildIndex = -1;

    replacementName = "";
    curPos->rhs->transform(this);
  }
}

void IdentifierCopier::transformChildren(ExprNode *en) {
  ExprNode *savedParent = Parent;
  unsigned savedChildIndex = ChildIndex;

  Parent = en;
  for (int i = 0; i < en->getNumChildren(); i++) {
    ChildIndex = i;
    en->getChild(i)->transform(this);
  }

  ChildIndex = savedChildIndex;
  Parent = savedParent;
}

/// These methods handle operations that leave the index structure intact, such
/// as elementwise and scalar operations.
#define GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Kind)                          \
  void IdentifierCopier::transform##Kind##Expr(Kind##Expr *en) {               \
    transformChildren(en);                                                     \
  }

GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Add)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Sub)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Mul)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(ScalarMul)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(Div)
GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL(ScalarDiv)

#undef GEN_TRANSFORM_COMPATIBLE_EXPR_NODE_IMPL

// These methods handle operations that change the index structure.
#define GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Kind)                        \
  void IdentifierCopier::transform##Kind##Expr(Kind##Expr *en) {               \
    bool savedIncompatible = Incompatible;                                     \
    Incompatible = true;                                                       \
    transformChildren(en);                                                     \
    Incompatible = savedIncompatible;                                          \
  }
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Contraction)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Product)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Stack)
GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL(Transposition)

#undef GEN_TRANSFORM_INCOMPATIBLE_EXPR_NODE_IMPL

void IdentifierCopier::transformIdentifierExpr(IdentifierExpr *en) {
  // the current identifier is different from 'lhs'
  if (en->getName() != curLhs->getName())
    return;

  // identifiers with the same name must have the same type
  assert(en->getDims() == curLhs->getDims());

  // index structures of 'lhs' and the current identifier are compatible
  if (!Incompatible)
    return;

  // if we get here, the current identifier must be replaced

  // if there isn't a replacement identifier yet, create a name for it
  const std::string temp =
      (replacementName != "") ? replacementName : getTemp();

  ExprNode *newIdNode =
      getENBuilder()->createIdentifierExpr(temp, en->getDims());
  Parent->setChild(ChildIndex, newIdNode);

  // if there wasn't a replacement identifier, assign the 'lhs' to it
  if (replacementName == "") {
    replacementName = temp;
    ExprNode *copyLhsId = getENBuilder()->createIdentifierExpr(
        curLhs->getName(), curLhs->getDims());
    ExprNode *newRhsId =
        getENBuilder()->createIdentifierExpr(replacementName, en->getDims());
    // the replacement identifier for the 'rhs' must be assigned the
    // value of the 'lhs' BEFORE the current assignment
    Assignments.insert(curPos, {newRhsId, copyLhsId});
  }
}
