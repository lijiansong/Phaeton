#include <cassert>
#include <string>
#include <vector>

#include "tir/AST/AST.h"
#include "tir/CodeGen/DirectCG.h"
#include "tir/Sema/Sema.h"
#include "tir/Sema/Type.h"

#define TEMP_MAP_ASSERT(expr)                                                  \
  {                                                                            \
    if (ExprTemps.find((expr)) == ExprTemps.end())                             \
      assert(0 && "internal error: no temporary for 'Expr' node");             \
  }

const std::string DirectCodeGen::addTempForExpr(const Expr *e) {
  const std::string t = getTemp();
  ExprTemps[e] = t;
  return t;
}

const std::string DirectCodeGen::addNameForExpr(const Expr *e,
                                                const std::string &name) {
  ExprTemps[e] = name;
  return name;
}

const std::string DirectCodeGen::getTempForExpr(const Expr *e) const {
  return ExprTemps.at(e);
}

DirectCodeGen::DirectCodeGen(const Sema *sema) : CodeGen(sema) {}

void DirectCodeGen::visitProgram(const Program *p) {
  visitProgramPrologue(p);
  ASTVisitor::visitProgram(p);
  visitProgramEpilogue(p);
}

void DirectCodeGen::visitDecl(const Decl *d) {
  visitDeclPrologue(d);
  visitDeclEpilogue(d);
}

void DirectCodeGen::visitStmt(const Stmt *s) {
  visitStmtPrologue(s);

  const Expr *expr = s->getExpr();
  expr->visit(this);
  TEMP_MAP_ASSERT(expr)

  visitStmtEpilogue(s);
}

const std::string DirectCodeGen::visitContraction(const Expr *e,
                                                  const TupleList &indices) {
  visitContractionPrologue(e, indices);

  if (indices.empty()) {
    e->visit(this);
    TEMP_MAP_ASSERT(e);
    return getTempForExpr(e);
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
  partitionPairList(rankL, indices, contrL, contrR, contrMixed);

  const std::string tempL = visitContraction(tensorL, contrL);

  int rankContractedL = rankL - 2 * contrL.size();

  TupleList shiftedR = contrR;
  shiftTupleList(-rankL, shiftedR);
  const std::string tempR = visitContraction(tensorR, shiftedR);

  if (contrMixed.empty())
    return visitContractionEpilogue(e, tempL, tempR, TupleList());

  List indL, indR;
  unpackPairList(contrMixed, indL, indR);
  adjustForContractions(indL, contrL);
  adjustForContractions(indR, contrL);
  adjustForContractions(indR, contrR);
  shiftList(-rankContractedL, indR);

  TupleList indicesLR;
  indicesLR.push_back(indL);
  indicesLR.push_back(indR);
  return visitContractionEpilogue(e, tempL, tempR, indicesLR);
}

void DirectCodeGen::visitBinaryExpr(const BinaryExpr *be) {
  const ASTNode::NodeType nt = be->getNodeType();

  visitBinaryExprPrologue(be);

  if (nt == ASTNode::NT_ContractionExpr) {
    visitContractionExprPrologue(be);

    std::string result;
    TupleList contractionsList;
    if (Sema::isListOfLists(be->getRight(), contractionsList)) {
      const BinaryExpr *tensor = extractTensorExprOrNull(be->getLeft());
      if (!tensor)
        assert(0 && "internal error: cannot handle general contractions yet");

      if (contractionsList.empty())
        assert(0 && "internal error: cannot have an empty list here");

      result = visitContraction(tensor, contractionsList);
    } else {
      const Expr *left = be->getLeft();
      left->visit(this);
      TEMP_MAP_ASSERT(left);

      const Expr *right = be->getRight();
      right->visit(this);
      TEMP_MAP_ASSERT(right);

      const int leftRank = getSema()->getType(left)->getRank();
      TupleList indicesLR;
      indicesLR.push_back({leftRank - 1});
      indicesLR.push_back({0});
      result = visitContractionEpilogue(be, getTempForExpr(left),
                                        getTempForExpr(right), indicesLR);
    }
    addNameForExpr(be, result);
    visitContractionExprEpilogue(be);

    visitBinaryExprEpilogue(be);
    return;
  }

  assert(nt != ASTNode::NT_ContractionExpr &&
         "internal error: should not be here");

  switch (nt) {
  case ASTNode::NT_AddExpr:
    visitAddExprPrologue(be);
    break;
  case ASTNode::NT_SubExpr:
    visitSubExprPrologue(be);
    break;
  case ASTNode::NT_MulExpr:
    visitMulExprPrologue(be);
    break;
  case ASTNode::NT_DivExpr:
    visitDivExprPrologue(be);
    break;
  case ASTNode::NT_ProductExpr:
    visitProductExprPrologue(be);
    break;
  default:
    assert(0 && "internal error: invalid binary expression");
  }

  const Expr *left = be->getLeft();
  left->visit(this);
  TEMP_MAP_ASSERT(left);

  const Expr *right = be->getRight();
  right->visit(this);
  TEMP_MAP_ASSERT(right);

  addTempForExpr(be);

  switch (nt) {
  case ASTNode::NT_AddExpr:
    visitAddExprEpilogue(be);
    break;
  case ASTNode::NT_SubExpr:
    visitSubExprEpilogue(be);
    break;
  case ASTNode::NT_MulExpr:
    visitMulExprEpilogue(be);
    break;
  case ASTNode::NT_DivExpr:
    visitDivExprEpilogue(be);
    break;
  case ASTNode::NT_ProductExpr:
    visitProductExprEpilogue(be);
    break;
  default:
    assert(0 && "internal error: invalid binary expression");
  }

  visitBinaryExprEpilogue(be);
  return;
}

void DirectCodeGen::visitIdentifier(const Identifier *id) {
  visitIdentifierPrologue(id);
  addNameForExpr(id, id->getName());
  visitIdentifierEpilogue(id);
}

void DirectCodeGen::visitInteger(const Integer *i) {
  visitIntegerPrologue(i);
  addTempForExpr(i);
  visitIntegerEpilogue(i);
}

void DirectCodeGen::visitBrackExpr(const BrackExpr *be) {
  visitBrackExprPrologue(be);

  const ExprList &exprs = *be->getExprs();
  assert(exprs.size() &&
         "internal error: tensor stack should not be empty here");

  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    e->visit(this);
    TEMP_MAP_ASSERT(e);
  }

  addTempForExpr(be);
  visitBrackExprEpilogue(be);
}

void DirectCodeGen::visitParenExpr(const ParenExpr *pe) {
  visitParenExprPrologue(pe);
  const Expr *e = pe->getExpr();
  e->visit(this);
  TEMP_MAP_ASSERT(e);
  addNameForExpr(pe, getTempForExpr(e));
  visitParenExprEpilogue(pe);
}
