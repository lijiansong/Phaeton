//==----------- StackRemover.cpp ---- Interface to StackRemover ------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the StackRemover class.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/StackRemover.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Support/ErrorHandling.h"

#include <functional>

using namespace phaeton;

bool StackRemover::isDeclaredId(const ExpressionNode *Node) const {
  if (!Node->isIdentifier()) {
    return false;
  }
  for (const auto *Id : CG->getDeclaredIds()) {
    if (Node->getName() == Id->getName()) {
      return true;
    }
  }
  return false;
}

ExpressionNode *StackRemover::buildReplacement(ExpressionNode *OriginalNode,
                                               bool IsForLHS) {
  assert(OriginalNode->isIdentifier());
  const std::string &Name = OriginalNode->getName();
  assert(Replacements.count(Name));
  const IdentifierExpr *Id = Replacements[Name];

  IdentifierExpr *ReplaceExpr =
      ExprNodeBuilder->createIdentifierExpr(Id->getName(), Id->getDims());

  for (unsigned I = 0; I < Id->getNumIndices(); ++I) {
    ReplaceExpr->addIndex(Id->getIndex(I));
  }
  for (unsigned I = 0; I < OriginalNode->getNumIndices(); ++I) {
    ReplaceExpr->addIndex(OriginalNode->getIndex(I));
  }

  if (!Id->getPermute()) {
    return ReplaceExpr;
  } else if (IsForLHS) {
    ReplaceExpr->setPermute(true);
    ReplaceExpr->setIndexPairs(Id->getIndexPairs());
    return ReplaceExpr;
  } else {
    // Notice: On 'RHS' transpositions are not represented within an
    // 'IdentifierExpr' directly, instead, a 'TranspositionExpr'
    // must be used.
    return ExprNodeBuilder->createTranspositionExpr(
        ReplaceExpr, ReplaceExpr->getIndexPairs());
  }
  // Note that all branches of the above if else statement have a return,
  // we should NOT get here.
  ph_unreachable(INTERNAL_ERROR "should not be here");
}

void StackRemover::transformChildren(ExpressionNode *Node) {
  unsigned ChildIndex = getChildIndex();
  ExpressionNode *Parent = getParent();

  setParent(Node);
  for (int I = 0; I < Node->getNumChildren(); ++I) {
    setChildIndex(I);
    Node->getChild(I)->transform(this);
  }

  setChildIndex(ChildIndex);
  setParent(Parent);
}

void StackRemover::transformIdentifierExpr(IdentifierExpr *Id) {
  if (Replacements.count(Id->getName()) == 0) {
    return;
  }
  ExpressionNode *Replacement = buildReplacement(Id, /* IsForLHS =*/false);
  ExpressionNode *Parent = getParent();
  if (Parent == nullptr) {
    CurrentPos->RHS = Replacement;
  } else {
    Parent->setChild(getChildIndex(), Replacement);
  }
}

void StackRemover::transformAssignments() {
  // 1. We need to lift all tensor stack expressions up to the top level.
  const auto &nodeLiftPredicate = [](const ExpressionNode *Node,
                                     const ExpressionNode *Root) {
    return Node->isStackExpr();
  };
  ExprTreeLifter Lifter(CG, nodeLiftPredicate);
  Lifter.transformAssignments();

  // 2. We need to find all stack expressions and remove them.
  CurrentPos = Assignments.begin();
  while (CurrentPos != Assignments.end()) {
    IdentifierExpr *LHS = static_cast<IdentifierExpr *>(CurrentPos->LHS);
    ExpressionNode *RHS = CurrentPos->RHS;

    if (!RHS->isStackExpr()) {
      ++CurrentPos;
      continue;
    }

    const auto NextPos = std::next(CurrentPos);
    const std::string &LHSName = LHS->getName();

    for (unsigned I = 0; I < RHS->getNumChildren(); ++I) {
      ExpressionNode *Child = RHS->getChild(I);

      // Note here we build 'LHS' for assigning the i-th child.
      IdentifierExpr *Id =
          ExprNodeBuilder->createIdentifierExpr(LHSName, LHS->getDims());
      for (unsigned J = 0; J < LHS->getNumIndices(); ++J) {
        Id->addIndex(LHS->getIndex(J));
      }
      Id->addIndex(std::to_string((long long)I));
      if (LHS->getPermute()) {
        Id->setPermute(true);
        Id->setIndexPairs(LHS->getIndexPairs());
      }

      if (Child->isIdentifier() && !isDeclaredId(Child)) {
        // The 'Child' node is an Identifier that has not been declared
        // explicitly in Phaeton program. Hence, this Identifier must have
        // been inserted by a transformation, here the transformation is
        // performed by lifting stack expressions to the top level. Therefore,
        // the Identifier only has a single use, i.e. inside the current stack
        // expression. Since the identifier has been inserted by a
        // transformation, it also has only one single definition.
        Replacements[Child->getName()] = Id;
      } else {
        // The assignemnt of the 'Child' node is inserted before next
        // assignment in Phaeton program. Since stacks have been lifted to the
        // top level, the expression tree rooted at 'Child' node does not
        // contain any stack expressions. Hence, current transformation
        // need not be applied recursively to the new assignment.
        Assignments.insert(NextPos, {Id, Child});
      }
    }

    Assignments.erase(CurrentPos);
    CurrentPos = NextPos;
  }

  // 3. Now we need to apply the replacements.
  for (CurrentPos = Assignments.begin(); CurrentPos != Assignments.end();
       ++CurrentPos) {
    // Here we handle replacements of defs, i.e. replacements on 'LHS'.
    {
      ExpressionNode *LHS = CurrentPos->LHS;
      assert(LHS->isIdentifier());
      if (Replacements.count(LHS->getName()))
        CurrentPos->LHS = buildReplacement(LHS, /* IsForLHS = */ true);
    }

    // Here we handle replacements of uses, i.e. replacements on 'RHS'.
    {
      ExpressionNode *RHS = CurrentPos->RHS;
      setParent(nullptr);
      setChildIndex(-1);
      RHS->transform(this);
    }
  }
}

#define GEN_TRANSFORM_EXPR_NODE_IMPL(ExprName)                                 \
  void StackRemover::transform##ExprName##Expr(ExprName##Expr *E) {            \
    transformChildren(E);                                                      \
  }

GEN_TRANSFORM_EXPR_NODE_IMPL(Add)
GEN_TRANSFORM_EXPR_NODE_IMPL(Sub)
GEN_TRANSFORM_EXPR_NODE_IMPL(Mul)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarMul)
GEN_TRANSFORM_EXPR_NODE_IMPL(Div)
GEN_TRANSFORM_EXPR_NODE_IMPL(ScalarDiv)
GEN_TRANSFORM_EXPR_NODE_IMPL(Contraction)
GEN_TRANSFORM_EXPR_NODE_IMPL(Product)
GEN_TRANSFORM_EXPR_NODE_IMPL(Stack)
GEN_TRANSFORM_EXPR_NODE_IMPL(Transposition)

#undef GEN_TRANSFORM_EXPR_NODE_IMPL
