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

#include <cassert>
#include <list>
#include <set>
#include <string>
#include <vector>

#define TYPE_MAP_ASSERT(expr)                                                  \
  {                                                                            \
    if (ExprTypes.find((expr)) == ExprTypes.end())                             \
      assert(0 && "internal error: type of 'Expr' node not in map");           \
  }

using namespace phaeton;

Sema::Sema() {
  scalar = new TensorType(std::vector<int>());
  Types.push_back(scalar);

  elemInfo.present = false;
}

Sema::~Sema() {
  for (auto it : Types)
    delete it;
  for (auto it : Symbols)
    delete it.second;
}

const TensorType *Sema::createType(const std::vector<int> &dims) {
  const TensorType *type = new TensorType(dims);
  Types.push_back(type);
  return type;
}

const TensorType *Sema::getType(const std::vector<int> &dims) {
  for (auto it : Types) {
    if (it->equals(dims))
      return it;
  }

  return createType(dims);
}

const TensorType *Sema::getType(const std::vector<int> &dims) const {
  for (auto it : Types) {
    if (it->equals(dims))
      return it;
  }

  return nullptr;
}

const Symbol *Sema::createSymbol(Symbol::SymbolKind k, const std::string &name,
                                 const TensorType &type, const Decl *decl) {
  Symbol *sym = new Symbol(k, name, type, decl);
  Symbols.addSymbol(sym);
  return sym;
}

const Symbol *Sema::getSymbol(const std::string &name) const {
  Symbol *sym;
  if (!Symbols.getSymbol(name, sym))
    return nullptr;

  return sym;
}

bool Sema::isTypeName(const Expr *e, const TensorType *&type) const {
  const Identifier *id = dynamic_cast<const Identifier *>(e);
  if (!id)
    return false;

  const Symbol *sym = getSymbol(id->getName());
  if (!sym || (sym->getKind() != Symbol::SYM_KIND_Type))
    return false;

  type = &sym->getType();
  return true;
}

bool Sema::isIntegerList(const Expr *e, std::vector<int> &ints) {
  const BrackExpr *be = dynamic_cast<const BrackExpr *>(e);
  if (!be)
    return false;

  const ExprList *list = be->getExprs();
  ints.clear();
  for (const auto &it : *list) {
    const Integer *integer = dynamic_cast<const Integer *>(it);
    if (!integer)
      return false;

    ints.push_back(integer->getValue());
  }

  return true;
}

bool Sema::isListOfLists(const Expr *e, std::vector<std::vector<int>> &lists) {
  const BrackExpr *be = dynamic_cast<const BrackExpr *>(e);
  if (!be)
    return false;

  const ExprList *list = be->getExprs();
  lists.clear();
  for (const Expr *const &expr : *list) {
    std::vector<int> integers;
    if (!isIntegerList(expr, integers))
      return false;

    lists.push_back(integers);
  }

  return true;
}

const TensorType *Sema::visitTypeExpr(const Expr *e) {
  const TensorType *type;
  if (isTypeName(e, type))
    return type;

  std::vector<int> dims;
  if (isIntegerList(e, dims))
    return getType(dims);

  assert(0 && "semantic error: invalid type expression");
  return nullptr;
}

void Sema::visitDecl(const Decl *d) {
  Symbol::SymbolKind k = (d->getNodeType() == ASTNode::NODETYPE_VarDecl)
                             ? Symbol::SYM_KIND_Variable
                             : Symbol::SYM_KIND_Type;
  const std::string &name = d->getIdentifier()->getName();
  const TensorType *type = visitTypeExpr(d->getTypeExpr());

  if (getSymbol(name)) {
    assert(
        0 &&
        ("semantic error: symbol \'" + name + "\' already declared").c_str());
    return;
  }

  const Symbol *sym = createSymbol(k, name, *type, d);

  if (d->getInOutSpecifier() & Decl::IO_Input)
    Inputs.push_back(sym);
  if (d->getInOutSpecifier() & Decl::IO_Output)
    Outputs.push_back(sym);

  if (k == Symbol::SYM_KIND_Type)
    NamedTypes[type] = sym;
}

void Sema::visitStmt(const Stmt *s) {
  const Identifier *id = s->getIdentifier();
  const Expr *expr = s->getExpr();

  const Symbol *sym = getSymbol(id->getName());
  if (!sym) {
    assert(0 && ("semantic error: assignment to undeclared symbol \'" +
                 id->getName() + "\'")
                    .c_str());
    return;
  }

  expr->visit(this);
  TYPE_MAP_ASSERT(expr)

  const TensorType *type = ExprTypes[expr];
  if (*type != sym->getType()) {
    assert(0 && "semantic error: assigning non-equal types");
    return;
  }
}

void Sema::visitBinaryExpr(const BinaryExpr *be) {
  const ASTNode::NodeType nt = be->getNodeType();

  if (nt == ASTNode::NODETYPE_ContractionExpr) {
    const Expr *left = be->getLeft();
    left->visit(this);
    TYPE_MAP_ASSERT(left);
    const TensorType *type0 = ExprTypes[left];

    std::vector<std::vector<int>> lists;
    if (isListOfLists(be->getRight(), lists)) {
      // assert(0 && "semantic error: right member of contraction not a list");

      if (lists.empty())
        assert(0 && "semantic error: contracting over empty index list");

      if (type0->getRank() == 0)
        assert(0 && "semantic error: cannot contract scalar");

      std::vector<int> res;
      for (int i = 0; i < type0->getRank(); i++)
        res.push_back(type0->getDim(i));

      std::set<int> index_set_to_erase;
      std::list<int> index_list_to_erase;
      for (const auto &list : lists) {
        // skip empty lists:
        if (!list.size())
          continue;

        const int dim = type0->getDim(list[0]);
        const int rank = type0->getRank();
        for (int i : list) {
          if (!(i < rank)) {
            assert(0 && "semantic error: contracted index out of range");
          }
          if (type0->getDim(i) != dim) {
            assert(0 && "semantic error: incompatible indices in contraction");
          }
          if (index_set_to_erase.count(i)) {
            assert(0 &&
                   ("semantic error: index \'" + std::to_string((long long)i) +
                    "\' appears multiple times")
                       .c_str());
          }
          index_set_to_erase.insert(i);
          index_list_to_erase.push_back(i);
        }
      }

      index_list_to_erase.sort();
      int erased = 0;
      for (int i : index_list_to_erase)
        res.erase(res.begin() + i - (erased++));

      ExprTypes[be] = getType(res);
      return;
    } else {
      const Expr *right = be->getRight();
      right->visit(this);
      TYPE_MAP_ASSERT(right);
      const TensorType *type1 = ExprTypes[right];

      const int rank0 = type0->getRank();
      const int rank1 = type1->getRank();

      // contract the last index from 'left' with the first index from 'right':
      if (rank0 == 0 || rank1 == 0) {
        assert(0 && "semantic error: cannot contract scalar");
      }
      if (type0->getDim(rank0 - 1) != type1->getDim(0)) {
        assert(0 && "semantic error: contracted dimensions do not match");
      }

      std::vector<int> res;
      for (int i = 0; i < (rank0 - 1); i++)
        res.push_back(type0->getDim(i));
      for (int i = 1; i < rank1; i++)
        res.push_back(type1->getDim(i));

      ExprTypes[be] = getType(res);
      return;
    }
    // each branch of the above if-statement should return:
    assert(0 && "internal error: should have returned");
  } else if (nt == ASTNode::NODETYPE_TranspositionExpr) {
    const Expr *left = be->getLeft();
    left->visit(this);
    TYPE_MAP_ASSERT(left);
    const TensorType *type0 = ExprTypes[left];
    const int rank = type0->getRank();

    std::vector<std::vector<int>> lists;
    if (!isListOfLists(be->getRight(), lists)) {
      assert(0 && "semantic error: right member of transposition not a list");
    }

    if (lists.empty()) {
      assert(0 && "semantic error: empty index list in transposition");
    }

    std::vector<int> dimsToTranspose = type0->getDims();
    for (const auto &list : lists) {
      if (list.size() != 2) {
        assert(0 && "semantic error: non-pair in transposition indices");
      }

      const int i0 = list[0], i1 = list[1];

      if (i0 == i1) {
        assert(0 && "semantic error: equal transposition indices");
      }

      if (!(i0 < rank) || !(i1 < rank)) {
        assert(0 && "semantic error: transposition index out of range");
      }

      // transpose the dimensions:
      const int dim0 = dimsToTranspose[i0];
      dimsToTranspose[i0] = dimsToTranspose[i1];
      dimsToTranspose[i1] = dim0;
    }

    ExprTypes[be] = getType(dimsToTranspose);
    return;
  }

  // binary expression is NOT a contraction and NOT a transposition:
  assert(nt != ASTNode::NODETYPE_ContractionExpr &&
         nt != ASTNode::NODETYPE_TranspositionExpr &&
         "internal error: should not be here");

  const Expr *left = be->getLeft();
  left->visit(this);
  TYPE_MAP_ASSERT(left);
  const TensorType &leftType = *ExprTypes[left];

  const Expr *right = be->getRight();
  right->visit(this);
  TYPE_MAP_ASSERT(right);
  const TensorType &rightType = *ExprTypes[right];

  const TensorType *resultType;
  switch (nt) {
  case ASTNode::NODETYPE_AddExpr:
  case ASTNode::NODETYPE_SubExpr: {
    if (leftType != rightType) {
      assert(0 && "semantic error: adding/subtracting non-equal types");
      return;
    }
    resultType = &leftType;
    break;
  }
  case ASTNode::NODETYPE_MulExpr: {
    if (isScalar(leftType)) {
      resultType = &rightType;
    } else {
      if (leftType != rightType) {
        assert(0 && "semantic error: multiplying non-equal types");
        return;
      }
      resultType = &leftType;
    }
    break;
  }
  case ASTNode::NODETYPE_DivExpr: {
    if (isScalar(rightType)) {
      resultType = &leftType;
    } else {
      if (leftType != rightType) {
        assert(0 && "semantic error: dividing non-equal types");
        return;
      }
      resultType = &leftType;
    }
    break;
  }
  case ASTNode::NODETYPE_ProductExpr: {
    std::vector<int> dims;

    if (isScalar(leftType) || isScalar(rightType)) {
      assert(0 &&
             "semantic error: cannot form the tensor product with a scalar");
      return;
    }

    for (int i0 = 0; i0 < leftType.getRank(); i0++)
      dims.push_back(leftType.getDim(i0));
    for (int i1 = 0; i1 < rightType.getRank(); i1++)
      dims.push_back(rightType.getDim(i1));

    resultType = getType(dims);
    break;
  }
  default:
    assert(0 && "internal error: invalid binary expression");
  }

  ExprTypes[be] = resultType;
  return;
}

void Sema::visitIdentifier(const Identifier *id) {
  const Symbol *sym = getSymbol(id->getName());
  if (!sym) {
    assert(0 && ("semantic error: use of undeclared symbol \'" + id->getName() +
                 "\'")
                    .c_str());
  }

  ExprTypes[id] = &sym->getType();
}

void Sema::visitInteger(const Integer *i) { ExprTypes[i] = scalar; }

void Sema::visitBrackExpr(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();
  if (!exprs.size())
    assert(0 && "semantic error: tensor stack cannot be empty");

  exprs[0]->visit(this);
  TYPE_MAP_ASSERT(exprs[0]);
  const TensorType *type = getType(exprs[0]);

  // skip the first expression in the list,
  // it has already been visited above:
  for (unsigned i = 1; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    e->visit(this);
    TYPE_MAP_ASSERT(e);
    if (getType(e) != type)
      assert(0 && "semantic error: type mismatch in tensor stack");
  }

  std::vector<int> dims;
  dims.push_back(exprs.size());
  for (unsigned i = 0; i < type->getRank(); i++)
    dims.push_back(type->getDim(i));

  ExprTypes[be] = getType(dims);
}

void Sema::visitParenExpr(const ParenExpr *pe) {
  const Expr *e = pe->getExpr();
  e->visit(this);
  TYPE_MAP_ASSERT(e);
  ExprTypes[pe] = getType(e);
}

bool Sema::isNamedType(const TensorType *type) const {
  return NamedTypes.count(type);
}

const Symbol *Sema::getTypeSymbol(const TensorType *type) const {
  if (!isNamedType(type))
    return nullptr;

  return NamedTypes.at(type);
}

void Sema::visitElemDirect(const ElemDirect *ed) {
  if (elemInfo.present) {
    // If we get here, this means it is an 'internal error', NOT a 'semantic
    // error'. Since the grammar allows only a single 'ElemDirect', if there are
    // more than one 'ElemDirect', the parser should have exited due to a syntax
    // error, and we should not get to the semantic analysis.
    assert(0 && "internal error: only a single element directive is allowed");
  }

  elemInfo.present = true;
  elemInfo.pos = ed->getPOSSpecifier();
  elemInfo.dim = ed->getInteger()->getValue();

  const BrackExpr *be = dynamic_cast<const BrackExpr *>(ed->getIdentifiers());
  if (!be)
    assert(0 && "semantic error: expected list of expressions in brackets");

  const ExprList &elist = *be->getExprs();
  for (unsigned i = 0; i < elist.size(); i++) {
    const Identifier *id = dynamic_cast<const Identifier *>(elist[i]);
    if (!id)
      assert(0 && "semantic error: expected identifier in element directive");

    const Symbol *s = getSymbol(id->getName());
    if (!s)
      assert(0 && "semantic error: undefined symbol");
    if (s->getKind() != Symbol::SYM_KIND_Variable)
      assert(0 && "semantic error: expected variable name");

    elemInfo.syms.insert(s);
  }
}
