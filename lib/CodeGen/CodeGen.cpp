//==--- CodeGen.cpp ----- Interface to code generation ---------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the CodeGen class.
//
//===----------------------------------------------------------------------===//

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/TensorExprTree.h"
#include "ph/Sema/Type.h"
#include "ph/Support/ErrorHandling.h"

#include <cassert>
#include <functional>
#include <list>
#include <vector>

using namespace phaeton;

CodeGen::CodeGen(const Sema *S, const std::string &FuncName)
    : ThisSema(S), TmpCounter(0), TgtLangCode(""), CGFuncName(FuncName) {
  ExprNodeBuilder = new ExpressionNodeBuilder;
}

CodeGen::~CodeGen() { delete ExprNodeBuilder; }

std::string CodeGen::getTmp() {
  return "t" + std::to_string((long long)TmpCounter++);
}

void CodeGen::addFuncArg(const std::string &Name) {
  const int Position = FuncArgs.size();
  FuncArgs.push_back({Position, Name});
}

void CodeGen::assertExprTreeMap(const Expression *E) const {
  if (ExprTrees.find((E)) == ExprTrees.end())
    ph_unreachable(INTERNAL_ERROR "no expression tree for 'Expression' node");
}

void CodeGen::visitDecl(const Decl *Decl) {
  if (Decl->getASTNodeKind() == ASTNode::AST_NODE_KIND_TypeDecl) {
    return;
  }

  addDeclaredId(Decl);
}

void CodeGen::visitStmt(const Stmt *Stmt) { addAssignment(Stmt); }

void CodeGen::addDeclaredId(const Decl *Decl) {
  const Sema *S = getSema();
  const std::string &Name = Decl->getIdentifier()->getName();
  const TensorDataType &Type = S->getSymbol(Name)->getType();
  ExpressionNode *Id =
      ExprNodeBuilder->createIdentifierExpr(Name, Type.getDims());

  DeclaredIds.push_back(Id);
}

void CodeGen::addAssignment(const Stmt *Stmt) {
  const Sema *Sema = getSema();
  const std::string &Name = Stmt->getIdentifier()->getName();
  const TensorDataType &T = Sema->getSymbol(Name)->getType();
  ExpressionNode *LHS =
      ExprNodeBuilder->createIdentifierExpr(Name, T.getDims());

  const Expression *E = Stmt->getExpr();
  assertExprTreeMap(E);
  ExpressionNode *RHS = ExprTrees[E];

  Assignments.push_back({LHS, RHS});
}

const CodeGen::FuncArg &CodeGen::getFuncArg(unsigned Index) const {
  assert(Index < getNumFuncArgs() && INTERNAL_ERROR
         "index out of bounds for auto-gen function args");
  return FuncArgs.at(Index);
};

bool CodeGen::allCompare(const List &L, ComparisonKind CmpKey, int Pivot) {
  std::function<bool(int)> cmpFunc = [CmpKey, Pivot](int I) -> bool {
    switch (CmpKey) {
    case CMP_Less:
      return I < Pivot;
    case CMP_LessEqual:
      return I <= Pivot;
    case CMP_Equal:
      return I == Pivot;
    case CMP_GreaterEqual:
      return I >= Pivot;
    case CMP_Greater:
      return I > Pivot;
    default:
      ph_unreachable(INTERNAL_ERROR "invalid comparison");
    }
  };

  for (const auto &I : L) {
    if (!cmpFunc(I)) {
      return false;
    }
  }
  return true;
}

bool CodeGen::isPairList(const TupleList &TList) {
  for (const auto &L : TList) {
    if (L.size() != 2) {
      return false;
    }
  }
  return true;
}

bool CodeGen::partitionPairList(int Pivot, const TupleList &TList,
                                TupleList &Left, TupleList &Right,
                                TupleList &Mixed) {
  if (!isPairList(TList)) {
    return false;
  }

  Left.clear();
  Right.clear();
  Mixed.clear();

  for (const auto &L : TList) {
    if (allCompare(L, CMP_Less, Pivot)) {
      Left.push_back(L);
    } else if (allCompare(L, CMP_GreaterEqual, Pivot)) {
      Right.push_back(L);
    } else {
      Mixed.push_back(L);
    }
  }
  return true;
}

void CodeGen::shiftList(int ShiftAmount, List &L) {
  for (int I = 0; I < L.size(); ++I) {
    L[I] += ShiftAmount;
  }
}

void CodeGen::shiftTupleList(int ShiftAmount, TupleList &TList) {
  // for each tuple 'L' in 'TList'
  for (auto &L : TList)
    shiftList(ShiftAmount, L);
}

void CodeGen::flattenTupleList(const TupleList &TList, std::list<int> &Res) {
  Res.clear();
  for (const auto &T : TList) {
    for (int Elem : T)
      Res.push_back(Elem);
  }
  // Note that here we sort the result.
  Res.sort();
}

void CodeGen::unpackPairList(const TupleList &TList, List &Left, List &Right) {
  assert(isPairList(TList));

  Left.clear();
  Right.clear();
  for (const auto &T : TList) {
    int L = (T[0] < T[1]) ? T[0] : T[1];
    int R = (T[0] < T[1]) ? T[1] : T[0];
    Left.push_back(L);
    Right.push_back(R);
  }
}

void CodeGen::adjustForContractions(List &Index,
                                    const TupleList &Contractions) {
  assert(isPairList(Contractions));

  // FIXME: The following nested loop has a runtime(time complexity) that is
  // roughly quadratic in the size of 'Contractions', here we assume that
  // 'Index.size()' ~ 'Contractions.size()'.
  for (int I = 0; I < Index.size(); ++I) {
    int CurIndex = Index[I];
    // determine the number of contracted indices
    // that are smaller than 'CurIndex'
    int Adj = 0;
    for (const Tuple &T : Contractions) {
      Adj += (T[0] < CurIndex) + (T[1] < CurIndex);
    }

    Index[I] -= Adj;
  }
}

const std::string CodeGen::getListString(const List &L) {
  std::string Res = "[";
  for (int I = 0; I < L.size(); ++I) {
    Res += std::to_string((long long)L[I]);
    if (I != (L.size() - 1)) {
      Res += ", ";
    }
  }
  Res += "]";
  return Res;
}

const std::string CodeGen::getTupleListString(const TupleList &TList) {
  std::string Res = "[";
  for (int L = 0; L < TList.size(); ++L) {
    Res += getListString(TList[L]);
    if (L != (TList.size() - 1)) {
      Res += ", ";
    }
  }
  Res += "]";
  return Res;
}

const BinaryExpr *CodeGen::extractTensorExprOrNull(const Expression *E) {
  const BinaryExpr *TensorExpr = dynamic_cast<const BinaryExpr *>(E);
  if (!TensorExpr) {
    const ParenExpr *PE = dynamic_cast<const ParenExpr *>(E);
    if (PE) {
      return extractTensorExprOrNull(PE->getExpr());
    } else {
      return nullptr;
    }
  }

  // Note that if we get here, 'TensorExpr' should NOT be 'nullptr'.
  assert(TensorExpr);

  if (TensorExpr->getASTNodeKind() != ASTNode::AST_NODE_KIND_ProductExpr) {
    return nullptr;
  }

  return TensorExpr;
}
