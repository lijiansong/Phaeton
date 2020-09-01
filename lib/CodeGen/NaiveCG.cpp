//==---- NaiveCG.cpp ----- Interface to naive code generation --------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the NaiveCodeGen class.
//
//===----------------------------------------------------------------------===//

#include "ph/CodeGen/NaiveCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"
#include "ph/Support/ErrorHandling.h"

#include <cassert>
#include <string>
#include <vector>

using namespace phaeton;

NaiveCodeGen::NaiveCodeGen(const Sema *S, const std::string &FuncName)
    : CodeGen(S, FuncName) {}

void NaiveCodeGen::visitBinaryExpr(const BinaryExpr *BE) {
  const Sema &S = *getSema();
  const ASTNode::ASTNodeKind NK = BE->getASTNodeKind();

  // Handle contraction expression.
  if (NK == ASTNode::AST_NODE_KIND_ContractionExpr) {
    TupleList ContractionsList;
    if (Sema::isListOfLists(BE->getRight(), ContractionsList)) {
      const BinaryExpr *TensorExpr = extractTensorExprOrNull(BE->getLeft());
      if (!TensorExpr)
        ph_unreachable(INTERNAL_ERROR "cannot handle general contractions yet");

      if (ContractionsList.empty())
        ph_unreachable(INTERNAL_ERROR "cannot have an empty list here");

      visitContraction(TensorExpr, ContractionsList);
      assertExprTreeMap(TensorExpr);
      addExpressionNode(BE, getExpressionNode(TensorExpr));
    } else {
      const Expression *Left = BE->getLeft();
      Left->visit(this);
      assertExprTreeMap(Left);

      const Expression *Right = BE->getRight();
      Right->visit(this);
      assertExprTreeMap(Right);

      const int LeftRank = S.getType(Left)->getRank();
      addExpressionNode(BE, ExprNodeBuilder->createContractionExpr(
                                getExpressionNode(Left), {LeftRank - 1},
                                getExpressionNode(Right), {0}));
    }
    return;
  } else if (NK == ASTNode::AST_NODE_KIND_TranspositionExpr) {
    // Handle transposition expression.
    const Expression *Left = BE->getLeft();
    Left->visit(this);
    assertExprTreeMap(Left);

    TupleList IndexPairs;
    if (!Sema::isListOfLists(BE->getRight(), IndexPairs))
      ph_unreachable(INTERNAL_ERROR "right of transposition is not a list");

    if (IndexPairs.empty())
      ph_unreachable(INTERNAL_ERROR "cannot have an empty list here");

    addExpressionNode(BE, ExprNodeBuilder->createTranspositionExpr(
                              getExpressionNode(Left), IndexPairs));
    return;
  }

  assert(NK != ASTNode::AST_NODE_KIND_ContractionExpr &&
         NK != ASTNode::AST_NODE_KIND_TranspositionExpr &&
         INTERNAL_ERROR "should not be here");

  const Expression *Left = BE->getLeft();
  Left->visit(this);
  assertExprTreeMap(Left);

  const Expression *Right = BE->getRight();
  Right->visit(this);
  assertExprTreeMap(Right);

  // Handle element-wise and product expression.
  ExpressionNode *Result;
  ExpressionNode *LHS = getExpressionNode(Left);
  ExpressionNode *RHS = getExpressionNode(Right);
  switch (NK) {
  case ASTNode::AST_NODE_KIND_AddExpr:
    Result = ExprNodeBuilder->createAddExpr(LHS, RHS);
    break;
  case ASTNode::AST_NODE_KIND_SubExpr:
    Result = ExprNodeBuilder->createSubExpr(LHS, RHS);
    break;
  case ASTNode::AST_NODE_KIND_MulExpr:
    if (S.isScalar(*S.getType(Left)))
      Result = ExprNodeBuilder->createScalarMulExpr(LHS, RHS);
    else
      Result = ExprNodeBuilder->createMulExpr(LHS, RHS);
    break;
  case ASTNode::AST_NODE_KIND_DivExpr:
    if (S.isScalar(*S.getType(Right)))
      Result = ExprNodeBuilder->createScalarDivExpr(LHS, RHS);
    else
      Result = ExprNodeBuilder->createDivExpr(LHS, RHS);
    break;
  case ASTNode::AST_NODE_KIND_ProductExpr:
    Result = ExprNodeBuilder->createProductExpr(LHS, RHS);
    break;
  default:
    ph_unreachable(INTERNAL_ERROR "invalid binary expression");
  }

  addExpressionNode(BE, Result);
}

void NaiveCodeGen::visitStmt(const Stmt *S) {
  const Expression *E = S->getExpr();
  E->visit(this);
  assertExprTreeMap(E);
  CodeGen::visitStmt(S);
}

void NaiveCodeGen::visitBrackExpr(const BrackExpr *BracketExpr) {
  const ExprList &Exprs = *BracketExpr->getExprs();
  assert(Exprs.size() &&
         "internal error: tensor stack should not be empty here");

  std::vector<ExpressionNode *> Members;
  for (unsigned i = 0; i < Exprs.size(); i++) {
    const Expression *E = Exprs[i];
    E->visit(this);
    assertExprTreeMap(E);

    Members.push_back(getExpressionNode(E));
  }

  addExpressionNode(BracketExpr, ExprNodeBuilder->createStackExpr(Members));
}

void NaiveCodeGen::visitParenExpr(const ParenExpr *PE) {
  const Expression *Node = PE->getExpr();
  Node->visit(this);
  assertExprTreeMap(Node);

  addExpressionNode(PE, getExpressionNode(Node));
}

void NaiveCodeGen::visitIdentifier(const Identifier *Id) {
  const Sema &S = *getSema();
  const std::string &Name = Id->getName();
  const TensorDataType &Type = S.getSymbol(Name)->getType();

  addExpressionNode(
      Id, ExprNodeBuilder->createIdentifierExpr(Name, Type.getDims()));
}

void NaiveCodeGen::visitContraction(const Expression *E,
                                    const TupleList &Index) {
  if (Index.empty()) {
    E->visit(this);
    assertExprTreeMap(E);
    return;
  }

  const BinaryExpr *TensorExpr = extractTensorExprOrNull(E);
  if (!TensorExpr)
    ph_unreachable(INTERNAL_ERROR "cannot handle general contractions yet");

  if (!isPairList(Index))
    ph_unreachable(INTERNAL_ERROR "only pairs of indices can be contracted");

  const Expression *TensorLeft = TensorExpr->getLeft();
  const Expression *TensorRight = TensorExpr->getRight();
  const TensorDataType *TypeLeft = getSema()->getType(TensorLeft);
  int RankLeft = TypeLeft->getRank();

  TupleList ContrLeft, ContrRight, ContrMixed;
  // Classify the index pairs into the following three categories:
  // - 'ContrLeft', contractions of the left sub-expression;
  // - 'ContrRight', contractions of the right sub-expression;
  // - 'ContrMixed', means having one index from each sub-expression
  partitionPairList(RankLeft, Index, ContrLeft, ContrRight, ContrMixed);

  visitContraction(TensorLeft, ContrLeft);
  assertExprTreeMap(TensorLeft);

  // Note: here we determine the rank of the result left sub-expression after
  // contraction has been performed over the set of index pairs 'contrLeft'.
  int RankContractedLeft = RankLeft - 2 * ContrLeft.size();

  // Note that the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression.
  TupleList ShiftedRight = ContrRight;
  shiftTupleList(-RankLeft, ShiftedRight);
  visitContraction(TensorRight, ShiftedRight);
  assertExprTreeMap(TensorRight);

  if (ContrMixed.empty()) {
    addExpressionNode(
        E, ExprNodeBuilder->createProductExpr(getExpressionNode(TensorLeft),
                                              getExpressionNode(TensorRight)));
    return;
  }

  List IndexLeft, IndexRight;
  unpackPairList(ContrMixed, IndexLeft, IndexRight);
  // Note that only contractions in 'ContrLeft' affect the adjustments
  // of the left indices in 'IndexLeft'.
  adjustForContractions(IndexLeft, ContrLeft);
  // Note that adjustment of right indices in 'IndexRight' are affected by
  // the contractions in both 'ContrLeft' and 'ContrRight'.
  adjustForContractions(IndexRight, ContrLeft);
  adjustForContractions(IndexRight, ContrRight);
  // Note that indices to be contracted over in the right sub-expression
  // must be relative to the first index of the right sub-expresion.
  shiftList(-RankContractedLeft, IndexRight);

  addExpressionNode(E, ExprNodeBuilder->createContractionExpr(
                           getExpressionNode(TensorLeft), IndexLeft,
                           getExpressionNode(TensorRight), IndexRight));
}
