#include <cassert>
#include <string>
#include <vector>

#include "ph/AST/AST.h"
#include "ph/CodeGen/DirectCG.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#define TEMP_MAP_ASSERT(expr)                                                  \
  {                                                                            \
    if (ExprTemps.find((expr)) == ExprTemps.end())                             \
      assert(0 && "internal error: no temporary for 'Expr' node");             \
  }

DirectCodeGen::DirectCodeGen(const Sema *sema) : CodeGen(sema) {}

void DirectCodeGen::visitStmt(const Stmt *s) {
  CodeGen::visitStmt(s);

  const Expr *expr = s->getExpr();
  expr->visit(this);
  EXPR_TREE_MAP_ASSERT(expr);
}

void DirectCodeGen::visitContraction(const Expr *e, const TupleList &indices) {
  if (indices.empty()) {
    e->visit(this);
    EXPR_TREE_MAP_ASSERT(e);
    return;
  }

  const BinaryExpr *tensor = extractTensorExprOrNull(e);
  if (!tensor)
    assert(0 && "internal error: cannot handle general contractions yet");

  if (!isPairList(indices))
    assert(0 && "internal error: only pairs of indices can be contracted");

  const Expr *tensorL = tensor->getLeft();
  const Expr *tensorR = tensor->getRight();
  const TensorType *typeL = getSema()->getType(tensorL);
  int rankL = typeL->getRank();

  TupleList contrL, contrR, contrMixed;
  // Classify these index pairs into three categories:
  // - contractions of the left sub-expression, i.e. 'contrL'
  // - contractions of the right sub-expression, i.e. 'contrR'
  // - having one index from each sub-expression, i.e. 'contrMixed'
  partitionPairList(rankL, indices, contrL, contrR, contrMixed);

  visitContraction(tensorL, contrL);
  EXPR_TREE_MAP_ASSERT(tensorL);

  int rankContractedL = rankL - 2 * contrL.size();

  TupleList shiftedR = contrR;
  shiftTupleList(-rankL, shiftedR);
  visitContraction(tensorR, shiftedR);
  EXPR_TREE_MAP_ASSERT(tensorR);

  if (contrMixed.empty()) {
    addExprNode(e, ENBuilder->createProductExpr(getExprNode(tensorL),
                                                getExprNode(tensorR)));
    return;
  }

  List indL, indR;
  unpackPairList(contrMixed, indL, indR);
  adjustForContractions(indL, contrL);
  adjustForContractions(indR, contrL);
  adjustForContractions(indR, contrR);
  shiftList(-rankContractedL, indR);

  addExprNode(e, ENBuilder->createContractionExpr(getExprNode(tensorL), indL,
                                                  getExprNode(tensorR), indR));
}

void DirectCodeGen::visitBinaryExpr(const BinaryExpr *be) {
  const ASTNode::NodeType nt = be->getNodeType();

  if (nt == ASTNode::NT_ContractionExpr) {
    TupleList contractionsList;
    if (Sema::isListOfLists(be->getRight(), contractionsList)) {
      const BinaryExpr *tensor = extractTensorExprOrNull(be->getLeft());
      if (!tensor)
        assert(0 && "internal error: cannot handle general contractions yet");

      if (contractionsList.empty())
        assert(0 && "internal error: cannot have an empty list here");

      visitContraction(tensor, contractionsList);
      EXPR_TREE_MAP_ASSERT(tensor);
      addExprNode(be, getExprNode(tensor));
    } else {
      const Expr *left = be->getLeft();
      left->visit(this);
      EXPR_TREE_MAP_ASSERT(left);

      const Expr *right = be->getRight();
      right->visit(this);
      EXPR_TREE_MAP_ASSERT(right);

      const int leftRank = getSema()->getType(left)->getRank();
      addExprNode(
          be, ENBuilder->createContractionExpr(
                  getExprNode(left), {leftRank - 1}, getExprNode(right), {0}));
    }
    return;
  }

  assert(nt != ASTNode::NT_ContractionExpr &&
         "internal error: should not be here");

  const Expr *left = be->getLeft();
  left->visit(this);
  EXPR_TREE_MAP_ASSERT(left);

  const Expr *right = be->getRight();
  right->visit(this);
  EXPR_TREE_MAP_ASSERT(right);

  ExprNode *result, *lhs = getExprNode(left), *rhs = getExprNode(right);
  switch (nt) {
  case ASTNode::NT_AddExpr:
    result = ENBuilder->createAddExpr(lhs, rhs);
    break;
  case ASTNode::NT_SubExpr:
    result = ENBuilder->createSubExpr(lhs, rhs);
    break;
  case ASTNode::NT_MulExpr:
    result = ENBuilder->createMulExpr(lhs, rhs);
    break;
  case ASTNode::NT_DivExpr:
    result = ENBuilder->createDivExpr(lhs, rhs);
    break;
  case ASTNode::NT_ProductExpr:
    result = ENBuilder->createProductExpr(lhs, rhs);
    break;
  default:
    assert(0 && "internal error: invalid binary expression");
  }

  addExprNode(be, result);
}

void DirectCodeGen::visitIdentifier(const Identifier *id) {
  const Sema &sema = *getSema();
  const std::string &name = id->getName();
  const TensorType &type = sema.getSymbol(name)->getType();

  addExprNode(id, ENBuilder->createIdentifierExpr(name, type.getDims()));
}

void DirectCodeGen::visitBrackExpr(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();
  assert(exprs.size() &&
         "internal error: tensor stack should not be empty here");

  std::vector<ExprNode *> members;
  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    e->visit(this);
    EXPR_TREE_MAP_ASSERT(e);

    members.push_back(getExprNode(e));
  }

  addExprNode(be, ENBuilder->createStackExpr(members));
}

void DirectCodeGen::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
  EXPR_TREE_MAP_ASSERT(e);

  addExprNode(pe, getExprNode(e));
}
