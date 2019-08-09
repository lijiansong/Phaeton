//===--- Sema.cpp - AST Builder and Semantic Analysis Implementation ------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the actions class which performs semantic analysis and
// builds an AST out of a parse stream.
//
//===----------------------------------------------------------------------===//

#include "ph/Sema/Sema.h"
#include "ph/Support/ErrorHandling.h"

#include <cassert>
#include <list>
#include <set>
#include <string>
#include <vector>

#define ASSERT_TYPE_MAP(Expr)                                                  \
  {                                                                            \
    if (ExprTypes.find((Expr)) == ExprTypes.end())                             \
      ph_unreachable(INTERNAL_ERROR "type of 'Expr' node not in map");         \
  }

using namespace phaeton;

Sema::Sema() {
  Scalar = new TensorType(std::vector<int>());
  Types.push_back(Scalar);

  ElementInfo.Present = false;
}

Sema::~Sema() {
  for (auto It : Types)
    delete It;
  for (auto It : Symbols)
    delete It.second;
}

const TensorType *Sema::createType(const std::vector<int> &Dims) {
  const TensorType *Type = new TensorType(Dims);
  Types.push_back(Type);
  return Type;
}

const TensorType *Sema::getType(const std::vector<int> &Dims) {
  for (auto It : Types) {
    if (It->equals(Dims))
      return It;
  }

  return createType(Dims);
}

const TensorType *Sema::getType(const std::vector<int> &Dims) const {
  for (auto It : Types) {
    if (It->equals(Dims))
      return It;
  }

  return nullptr;
}

const Symbol *Sema::createSymbol(Symbol::SymbolKind SK, const std::string &Name,
                                 const TensorType &Type, const Decl *Decl) {
  Symbol *Sym = new Symbol(SK, Name, Type, Decl);
  Symbols.addSymbol(Sym);
  return Sym;
}

const Symbol *Sema::getSymbol(const std::string &Name) const {
  Symbol *Sym;
  if (!Symbols.getSymbol(Name, Sym)) {
    return nullptr;
  }

  return Sym;
}

bool Sema::isTypeName(const Expr *E, const TensorType *&Type) const {
  const Identifier *Id = dynamic_cast<const Identifier *>(E);
  if (!Id) {
    return false;
  }

  const Symbol *Sym = getSymbol(Id->getName());
  if (!Sym || (Sym->getSymbolKind() != Symbol::SYM_KIND_Type))
    return false;

  Type = &Sym->getType();
  return true;
}

bool Sema::isIntegerList(const Expr *E, std::vector<int> &Ints) {
  const BrackExpr *BE = dynamic_cast<const BrackExpr *>(E);
  if (!BE) {
    return false;
  }

  const ExprList *List = BE->getExprs();
  Ints.clear();
  for (const auto &It : *List) {
    const Integer *Int = dynamic_cast<const Integer *>(It);
    if (!Int) {
      return false;
    }
    Ints.push_back(Int->getValue());
  }

  return true;
}

bool Sema::isListOfLists(const Expr *E, std::vector<std::vector<int>> &Lists) {
  const BrackExpr *BE = dynamic_cast<const BrackExpr *>(E);
  if (!BE) {
    return false;
  }

  const ExprList *List = BE->getExprs();
  Lists.clear();
  for (const Expr *const &Expr : *List) {
    std::vector<int> Integers;
    if (!isIntegerList(Expr, Integers)) {
      return false;
    }

    Lists.push_back(Integers);
  }

  return true;
}

const TensorType *Sema::visitTypeExpr(const Expr *E) {
  const TensorType *Type;
  if (isTypeName(E, Type)) {
    return Type;
  }

  std::vector<int> Dims;
  if (isIntegerList(E, Dims)) {
    return getType(Dims);
  }

  ph_unreachable(SEMANTIC_ERROR "invalid type expression");
  return nullptr;
}

void Sema::visitDecl(const Decl *D) {
  Symbol::SymbolKind Kind =
      (D->getASTNodeKind() == ASTNode::AST_NODE_KIND_VarDecl)
          ? Symbol::SYM_KIND_Variable
          : Symbol::SYM_KIND_Type;
  const std::string &Name = D->getIdentifier()->getName();
  const TensorType *Type = visitTypeExpr(D->getTypeExpr());

  if (getSymbol(Name)) {
    ph_unreachable(("symbol \'" + Name + "\' already declared").c_str());
    return;
  }

  const Symbol *Sym = createSymbol(Kind, Name, *Type, D);

  if (D->getInOutSpecifier() & Decl::IO_SPEC_Input)
    Inputs.push_back(Sym);
  if (D->getInOutSpecifier() & Decl::IO_SPEC_Output)
    Outputs.push_back(Sym);

  if (Kind == Symbol::SYM_KIND_Type)
    NamedTypes[Type] = Sym;
}

void Sema::visitStmt(const Stmt *S) {
  const Identifier *Id = S->getIdentifier();
  const Expr *Expr = S->getExpr();

  const Symbol *Sym = getSymbol(Id->getName());
  if (!Sym) {
    ph_unreachable(("semantic error: assignment to undeclared symbol \'" +
                    Id->getName() + "\'")
                       .c_str());
    return;
  }

  Expr->visit(this);
  ASSERT_TYPE_MAP(Expr)

  const TensorType *Type = ExprTypes[Expr];
  if (*Type != Sym->getType()) {
    ph_unreachable("semantic error: assigning non-equal types");
    return;
  }
}

// TODO: Refer clang to refactor this method.
void Sema::visitBinaryExpr(const BinaryExpr *BE) {
  const ASTNode::ASTNodeKind NK = BE->getASTNodeKind();

  if (NK == ASTNode::AST_NODE_KIND_ContractionExpr) {
    const Expr *Left = BE->getLeft();
    Left->visit(this);
    ASSERT_TYPE_MAP(Left);
    const TensorType *Type0 = ExprTypes[Left];

    std::vector<std::vector<int>> Lists;
    if (isListOfLists(BE->getRight(), Lists)) {
      // ph_unreachable(SEMANTIC_ERROR"right member of contraction not a list");

      if (Lists.empty()) {
        ph_unreachable(SEMANTIC_ERROR "contracting over empty index list");
      }

      if (Type0->getRank() == 0) {
        ph_unreachable(SEMANTIC_ERROR "cannot contract scalar");
      }

      std::vector<int> Res;
      for (int i = 0; i < Type0->getRank(); i++)
        Res.push_back(Type0->getDim(i));

      std::set<int> IndexSetToErase;
      std::list<int> IndexListToErase;
      for (const auto &List : Lists) {
        // skip empty Lists
        if (!List.size())
          continue;

        const int Dim = Type0->getDim(List[0]);
        const int Rank = Type0->getRank();
        for (int i : List) {
          if (!(i < Rank)) {
            ph_unreachable(SEMANTIC_ERROR "contracted index out of range");
          }
          if (Type0->getDim(i) != Dim) {
            ph_unreachable(SEMANTIC_ERROR
                           "incompatible indices in contraction");
          }
          if (IndexSetToErase.count(i)) {
            ph_unreachable(("semantic error: index \'" +
                            std::to_string((long long)i) +
                            "\' appears multiple times")
                               .c_str());
          }
          IndexSetToErase.insert(i);
          IndexListToErase.push_back(i);
        }
      }

      IndexListToErase.sort();
      int Erased = 0;
      for (int i : IndexListToErase)
        Res.erase(Res.begin() + i - (Erased++));

      ExprTypes[BE] = getType(Res);
      return;
    } else {
      const Expr *Right = BE->getRight();
      Right->visit(this);
      ASSERT_TYPE_MAP(Right);
      const TensorType *Type1 = ExprTypes[Right];

      const int Rank0 = Type0->getRank();
      const int Rank1 = Type1->getRank();

      // Note: Contract the last index from 'Left' with the first index from
      // 'Right'.
      if (Rank0 == 0 || Rank1 == 0) {
        ph_unreachable(SEMANTIC_ERROR "cannot contract scalar");
      }
      if (Type0->getDim(Rank0 - 1) != Type1->getDim(0)) {
        ph_unreachable(SEMANTIC_ERROR "contracted dimensions do not match");
      }

      std::vector<int> Res;
      for (int i = 0; i < (Rank0 - 1); i++)
        Res.push_back(Type0->getDim(i));
      for (int i = 1; i < Rank1; i++)
        Res.push_back(Type1->getDim(i));

      ExprTypes[BE] = getType(Res);
      return;
    }
    // Note that each branch of the above if-statement should return.
    // If we get here, this must be an internal ERROR!
    ph_unreachable(INTERNAL_ERROR "should have returned");
  } else if (NK == ASTNode::AST_NODE_KIND_TranspositionExpr) {
    const Expr *Left = BE->getLeft();
    Left->visit(this);
    ASSERT_TYPE_MAP(Left);
    const TensorType *Type0 = ExprTypes[Left];
    const int Rank = Type0->getRank();

    std::vector<std::vector<int>> Lists;
    if (!isListOfLists(BE->getRight(), Lists)) {
      ph_unreachable(SEMANTIC_ERROR "right member of transposition not a list");
    }

    if (Lists.empty()) {
      ph_unreachable(SEMANTIC_ERROR "empty index list in transposition");
    }

    std::vector<int> DimsToTranspose = Type0->getDims();
    for (const auto &List : Lists) {
      if (List.size() != 2) {
        ph_unreachable(SEMANTIC_ERROR "not a pair in transposition indices");
      }

      const int I0 = List[0], I1 = List[1];

      if (I0 == I1) {
        ph_unreachable(SEMANTIC_ERROR "equal transposition indices");
      }

      if (!(I0 < Rank) || !(I1 < Rank)) {
        ph_unreachable(SEMANTIC_ERROR "transposition index out of range");
      }

      // Note: Here we transpose the dimensions.
      const int Dim0 = DimsToTranspose[I0];
      DimsToTranspose[I0] = DimsToTranspose[I1];
      DimsToTranspose[I1] = Dim0;
    }

    ExprTypes[BE] = getType(DimsToTranspose);
    return;
  }

  // Note that binary expression is neither a contraction nor a transposition,
  // if we get here, that means an internal ERROR!
  assert(NK != ASTNode::AST_NODE_KIND_ContractionExpr &&
         NK != ASTNode::AST_NODE_KIND_TranspositionExpr &&
         "internal error: should not be here");

  const Expr *Left = BE->getLeft();
  Left->visit(this);
  ASSERT_TYPE_MAP(Left);
  const TensorType &LeftType = *ExprTypes[Left];

  const Expr *Right = BE->getRight();
  Right->visit(this);
  ASSERT_TYPE_MAP(Right);
  const TensorType &RightType = *ExprTypes[Right];

  const TensorType *ResultType;
  switch (NK) {
  case ASTNode::AST_NODE_KIND_AddExpr:
  case ASTNode::AST_NODE_KIND_SubExpr: {
    if (LeftType != RightType) {
      ph_unreachable(SEMANTIC_ERROR "add or sub non-equal types");
      return;
    }
    ResultType = &LeftType;
    break;
  }
  case ASTNode::AST_NODE_KIND_MulExpr: {
    if (isScalar(LeftType)) {
      ResultType = &RightType;
    } else {
      if (LeftType != RightType) {
        ph_unreachable(SEMANTIC_ERROR "mul non-equal types");
        return;
      }
      ResultType = &LeftType;
    }
    break;
  }
  case ASTNode::AST_NODE_KIND_DivExpr: {
    if (isScalar(RightType)) {
      ResultType = &LeftType;
    } else {
      if (LeftType != RightType) {
        ph_unreachable(SEMANTIC_ERROR "div non-equal types");
        return;
      }
      ResultType = &LeftType;
    }
    break;
  }
  case ASTNode::AST_NODE_KIND_ProductExpr: {
    std::vector<int> Dims;

    if (isScalar(LeftType) || isScalar(RightType)) {
      ph_unreachable(SEMANTIC_ERROR
                     "cannot form the tensor product with a scalar");
      return;
    }

    for (int I0 = 0; I0 < LeftType.getRank(); I0++)
      Dims.push_back(LeftType.getDim(I0));
    for (int I1 = 0; I1 < RightType.getRank(); I1++)
      Dims.push_back(RightType.getDim(I1));

    ResultType = getType(Dims);
    break;
  }
  default:
    ph_unreachable(INTERNAL_ERROR "invalid binary expression");
  }

  ExprTypes[BE] = ResultType;
  return;
}

void Sema::visitIdentifier(const Identifier *Id) {
  const Symbol *Sym = getSymbol(Id->getName());
  if (!Sym) {
    ph_unreachable(
        ("semantic error: use of undeclared symbol \'" + Id->getName() + "\'")
            .c_str());
  }

  ExprTypes[Id] = &Sym->getType();
}

void Sema::visitInteger(const Integer *I) { ExprTypes[I] = Scalar; }

void Sema::visitBrackExpr(const BrackExpr *BE) {
  const ExprList &List = *BE->getExprs();
  if (!List.size())
    ph_unreachable(SEMANTIC_ERROR "tensor stack cannot be empty");

  List[0]->visit(this);
  ASSERT_TYPE_MAP(List[0]);
  const TensorType *Type = getType(List[0]);

  // Note that here we skip the first expression in the list,
  // since it has already been visited above.
  for (unsigned i = 1; i < List.size(); ++i) {
    const Expr *E = List[i];
    E->visit(this);
    ASSERT_TYPE_MAP(E);
    if (getType(E) != Type)
      ph_unreachable(SEMANTIC_ERROR "type mismatch in tensor stack");
  }

  std::vector<int> Dims;
  Dims.push_back(List.size());
  for (unsigned i = 0; i < Type->getRank(); ++i)
    Dims.push_back(Type->getDim(i));

  ExprTypes[BE] = getType(Dims);
}

void Sema::visitParenExpr(const ParenExpr *PE) {
  const Expr *E = PE->getExpr();
  E->visit(this);
  ASSERT_TYPE_MAP(E);
  ExprTypes[PE] = getType(E);
}

bool Sema::isNamedType(const TensorType *Type) const {
  return NamedTypes.count(Type);
}

const Symbol *Sema::getTypeSymbol(const TensorType *Type) const {
  if (!isNamedType(Type)) {
    return nullptr;
  }

  return NamedTypes.at(Type);
}

void Sema::visitElementDirective(const ElementDirective *ED) {
  if (ElementInfo.Present) {
    // We should NOT get here!
    // If we get here, this means it is an 'internal error', NOT a 'semantic
    // error'. Since the grammar allows only a single 'ElementDirective', if
    // there are more than one 'ElementDirective', the parser should have exited
    // due to a syntax error, and we should not get to the semantic analysis.
    ph_unreachable(INTERNAL_ERROR
                   "only one single element directive is allowed");
  }

  ElementInfo.Present = true;
  ElementInfo.Pos = ED->getPOSSpecifier();
  ElementInfo.Dim = ED->getInteger()->getValue();

  const BrackExpr *BE = dynamic_cast<const BrackExpr *>(ED->getIdentifiers());
  if (!BE) {
    // Here 'BE' is expected to be a list of expressions in brackets.
    ph_unreachable(SEMANTIC_ERROR "expected list of expressions in brackets");
  }

  const ExprList &List = *BE->getExprs();
  for (unsigned i = 0; i < List.size(); i++) {
    const Identifier *Id = dynamic_cast<const Identifier *>(List[i]);
    if (!Id) {
      ph_unreachable(SEMANTIC_ERROR "expected identifier in element directive");
    }

    const Symbol *Sym = getSymbol(Id->getName());
    if (!Sym) {
      ph_unreachable(SEMANTIC_ERROR "undefined symbol");
    }
    if (Sym->getSymbolKind() != Symbol::SYM_KIND_Variable) {
      ph_unreachable(SEMANTIC_ERROR "expected variable name");
    }

    ElementInfo.Syms.insert(Sym);
  }
}

#undef ASSERT_TYPE_MAP
