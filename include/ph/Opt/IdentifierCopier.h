//==--- IdentifierCopier.h - Representation of Identifier expr copier ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the class IdentifierCopier.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_IDENTIFIER_COPIER_H
#define PHAETON_OPT_IDENTIFIER_COPIER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/TensorExprTree.h"

#include <functional>
#include <list>
#include <string>

namespace phaeton {

/// IdentifierCopier - This pass can help to lift identifiers out of expression
/// trees if they appear on both the 'lhs' and the 'rhs' in conflicting ways,
/// i.e. they come with different index structures.
///
/// Note: 'ExprLifter' CANNOT simply be used for the same purpose, because
/// 'IdentifierCopier' must have to gain more state about the current expression
/// trees and identifiers.
/// TODO: Make this pass more general.
class IdentifierCopier : public ExprTreeTransformer {
public:
  IdentifierCopier(CodeGen *Gen) : CG(Gen), Assignments(CG->getAssignments()) {}

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

  void transformNode(ExpressionNode *Node);
  void transformChildren(ExpressionNode *Node);
  void liftNode(ExpressionNode *Node);

  void transformAssignments();

private:
  CodeGen *CG;
  CodeGen::AssignmentsListTy &Assignments;

  const IdentifierExpr *CurrentLHS;
  ExpressionNode *Parent;
  int ChildIndex;
  bool Incompatible;
  std::string ReplaceName;

  CodeGen::AssignmentsListTy::iterator CurrentPos;

  // Helper methods that helps to implement functionality from the code
  // generator 'CG', and provide some information.
  std::string getTmp() { return CG->getTmp(); }
  ExpressionNodeBuilder *getExprNodeBuilder() {
    return CG->getExprNodeBuilder();
  }
};

} // end namespace phaeton

#endif // PHAETON_OPT_IDENTIFIER_COPIER_H
