//==--- StackExprRemover.cpp ---- Interface to StackExprRemover ------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the StackExprRemover class.
//
//===----------------------------------------------------------------------===//

#include "ph/Opt/StackExprRemover.h"
#include "ph/Opt/ExprTreeLifter.h"

#include <functional>

using namespace phaeton;

ExprNode *StackExprRemover::buildReplacement(ExprNode *original, bool forLHS) {
  assert(original->isIdentifier());
  const std::string &name = original->getName();
  assert(Replacements.count(name));
  const IdentifierExpr *id = Replacements[name];

  IdentifierExpr *replacement =
      ENBuilder->createIdentifierExpr(id->getName(), id->getDims());

  for (unsigned j = 0; j < id->getNumIndices(); j++) {
    replacement->addIndex(id->getIndex(j));
  }
  for (unsigned j = 0; j < original->getNumIndices(); j++) {
    replacement->addIndex(original->getIndex(j));
  }

  if (!id->permute()) {
    return replacement;
  } else if (forLHS) {
    replacement->setPermute(true);
    replacement->setIndexPairs(id->getIndexPairs());
    return replacement;
  } else {
    // on the 'rhs', transpositions are not represented within an
    // 'IdentifierExpr' directly; instead, a 'TranspositionExpr'
    // must be used.
    return ENBuilder->createTranspositionExpr(replacement,
                                              replacement->getIndexPairs());
  }
  // all branches of the above if-else-statement return.
  assert(0 && "internal error: should not be here");
}

bool StackExprRemover::isDeclaredId(const ExprNode *en) const {
  if (!en->isIdentifier())
    return false;

  for (const auto *id : CG->getDeclaredIds()) {
    if (en->getName() == id->getName())
      return true;
  }

  return false;
}

void StackExprRemover::transformAssignments() {
  // first, lift all stack expressions up to the top level.
  const auto &nodeLiftPredicate = [](const ExprNode *en, const ExprNode *root) {
    return en->isStackExpr();
  };
  ExprTreeLifter lifter(CG, nodeLiftPredicate);
  lifter.transformAssignments();

  // second, find all stack expressions and remove them.
  curPos = Assignments.begin();
  while (curPos != Assignments.end()) {
    IdentifierExpr *lhs = static_cast<IdentifierExpr *>(curPos->lhs);
    ExprNode *rhs = curPos->rhs;

    if (!rhs->isStackExpr()) {
      ++curPos;
      continue;
    }

    const auto nextPos = std::next(curPos);
    const std::string &lhsName = lhs->getName();

    for (unsigned i = 0; i < rhs->getNumChildren(); i++) {
      ExprNode *child = rhs->getChild(i);

      // build the 'lhs' for assigning the i-th child.
      IdentifierExpr *id =
          ENBuilder->createIdentifierExpr(lhsName, lhs->getDims());
      for (unsigned j = 0; j < lhs->getNumIndices(); j++) {
        id->addIndex(lhs->getIndex(j));
      }
      id->addIndex(std::to_string((long long)i));
      if (lhs->permute()) {
        id->setPermute(true);
        id->setIndexPairs(lhs->getIndexPairs());
      }

      if (child->isIdentifier() && !isDeclaredId(child)) {
        // The 'child' node is an identifier that has not been declared
        // explicitly in the CFD program. Hence, this identifier must have
        // been inserted by a transformation, i.e. by lifting stack expressions
        // to the top level. Therefore, the identifier only has a single use:
        // inside the current stack expression. Since the identifier has been
        // inserted by a transformation, it also has only a single definition.
        Replacements[child->getName()] = id;
      } else {
        // The assignemnt of the 'child' node is inserted before the next
        // assignment in the CFD program. Since stacks have been lifted to the
        // top level, the expression tree rooted at the 'child' node does not
        // contain any stack expressions. Therefore, the current transformation
        // need not be applied recursively to the new assignment.
        Assignments.insert(nextPos, {id, child});
      }
    }

    Assignments.erase(curPos);
    curPos = nextPos;
  }

  // third, apply replacements.
  for (curPos = Assignments.begin(); curPos != Assignments.end(); curPos++) {
    // handle replacements of definitions, i.e. replacements on the 'lhs'
    {
      ExprNode *lhs = curPos->lhs;
      assert(lhs->isIdentifier());

      if (Replacements.count(lhs->getName()))
        curPos->lhs = buildReplacement(lhs, /* forLHS = */ true);
    }

    // handle replacements of uses, i.e. replacements on the 'rhs'
    {
      ExprNode *rhs = curPos->rhs;
      setParent(nullptr);
      setChildIndex(-1);

      rhs->transform(this);
    }
  }
}

void StackExprRemover::transformChildren(ExprNode *en) {
  ExprNode *parent = getParent();
  unsigned childIndex = getChildIndex();

  setParent(en);
  for (int i = 0; i < en->getNumChildren(); i++) {
    setChildIndex(i);
    en->getChild(i)->transform(this);
  }

  setChildIndex(childIndex);
  setParent(parent);
}

#define GEN_TRANSFORM_EXPR_NODE_IMPL(Kind)                                     \
  void StackExprRemover::transform##Kind##Expr(Kind##Expr *en) {               \
    transformChildren(en);                                                     \
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

void StackExprRemover::transformIdentifierExpr(IdentifierExpr *en) {
  if (Replacements.count(en->getName()) == 0)
    return;

  ExprNode *replacement = buildReplacement(en, /* forLHS =*/false);

  ExprNode *parent = getParent();
  if (parent == nullptr)
    curPos->rhs = replacement;
  else
    parent->setChild(getChildIndex(), replacement);
}
