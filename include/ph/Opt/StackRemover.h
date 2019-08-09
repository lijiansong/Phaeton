//==--- StackRemover.h ----- Representation of stack expr remover ----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines class StackRemover. It removes tensor stack
// expressions and replaces and them with indexed identifiers.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_STACK_EXPR_REMOVER_H
#define PHAETON_OPT_STACK_EXPR_REMOVER_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/TensorExprTree.h"

#include <list>
#include <map>
#include <string>

namespace phaeton {

/// StackRemover - This pass Implement the removal of stack expressions,
/// where there used to be an assignment of a stack expression, the members of
/// the stack are now assigned to components of the left-hand side.
///
/// Note: this optimization pass have to be associate with Transposition
/// operations.
class StackRemover : public ExprTreeTransformer {
public:
  StackRemover(CodeGen *Gen)
      : CG(Gen), ENBuilder(CG->getENBuilder()),
        Assignments(CG->getAssignments()) {}

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

  void transformChildren(ExprNode *Node);

  void transformAssignments();

protected:
  ExprNode *buildReplacement(ExprNode *OriginalNode, bool IsForLHS);

  void setParent(ExprNode *Node) { Parent = Node; }
  ExprNode *getParent() const { return Parent; }

  void setChildIndex(int Index) { ChildIndex = Index; }
  int getChildIndex() const { return ChildIndex; }

private:
  CodeGen *CG;
  ExprNodeBuilder *ENBuilder;
  CodeGen::AssignmentsListTy &Assignments;
  ExprNode *Parent;
  int ChildIndex;
  CodeGen::AssignmentsListTy::iterator CurrentPos;
  std::map<std::string, const IdentifierExpr *> Replacements;

  // The following three methods are helper methods that help to implement
  // functionality from the code generator 'CG', and get the state of
  // code generator.
  std::string getTemp() { return CG->getTemp(); }
  bool isDeclaredId(const ExprNode *Node) const;
  ExprNodeBuilder *getENBuilder() { return CG->getENBuilder(); }
};

} // end namespace phaeton

#endif // PHAETON_OPT_STACK_EXPR_REMOVER_H
