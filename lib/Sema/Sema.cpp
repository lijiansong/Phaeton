#include <cassert>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "tir/Sema/Sema.h"
#include "tir/Sema/Symbol.h"
#include "tir/Sema/Type.h"

#define TYPE_MAP_ASSERT(expr)                                                  \
  {                                                                            \
    if (ExprTypes.find((expr)) == ExprTypes.end())                             \
      assert(0 && "internal error: type of 'Expr' node not in map");           \
  }

bool Sema::isTypeName(const Expr *e, const TensorType *&type) const {
  const Identifier *id = dynamic_cast<const Identifier *>(e);
  if (!id)
    return false;

  Symbol *sym = getSymbol(id->getName());
  if (!sym || (sym->getKind() != SK_Type))
    return false;

  type = &sym->getType();
  return true;
}

bool Sema::isIntegerList(const Expr *e, std::vector<int> &ints) const {
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

bool Sema::isListOfLists(const Expr *e,
                         std::vector<std::vector<int>> &lists) const {
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
  SymbolKind k = (d->getNodeType() == NT_VarDecl) ? SK_Variable : SK_Type;
  const std::string &name = d->getIdentifier()->getName();
  const TensorType *type = visitTypeExpr(d->getTypeExpr());

  if (getSymbol(name)) {
    assert(
        0 &&
        ("semantic error: symbol \'" + name + "\' already declared").c_str());
    return;
  }

  createSymbol(k, name, *type, d);
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

void Sema::visitExpr(const Expr *e) {
  e->visit(this);
  TYPE_MAP_ASSERT(e)
}

void Sema::visitFactor(const Factor *f) {
  f->visit(this);
  TYPE_MAP_ASSERT(f);
}

void Sema::visitBinaryExpr(const BinaryExpr *be) {
  switch (be->getNodeType()) {
  case NT_TensorExpr: {
    const Expr *left = be->getLeft();
    left->visit(this);
    TYPE_MAP_ASSERT(left);
    const TensorType *type0 = ExprTypes[left];

    const Expr *right = be->getRight();
    right->visit(this);
    TYPE_MAP_ASSERT(right);
    const TensorType *type1 = ExprTypes[right];

    std::vector<int> dims;
    for (int i0 = 0; i0 < type0->getRank(); i0++)
      dims.push_back(type0->getDim(i0));
    for (int i1 = 0; i1 < type1->getRank(); i1++)
      dims.push_back(type1->getDim(i1));

    ExprTypes[be] = getType(dims);
    return;
  }
  case NT_DotExpr: {
    const Expr *left = be->getLeft();
    left->visit(this);
    TYPE_MAP_ASSERT(left);
    const TensorType *type0 = ExprTypes[left];

    std::vector<std::vector<int>> lists;
    if (!isListOfLists(be->getRight(), lists))
      assert(0 && "semantic error: right member of contraction not a list");

    std::vector<int> res;
    for (int i = 0; i < type0->getRank(); i++)
      res.push_back(type0->getDim(i));

    std::set<int> index_set_to_erase;
    std::list<int> index_list_to_erase;
    for (const auto &list : lists) {
      // skip empty lists
      if (!list.size())
        continue;

      int dim = type0->getDim(list[0] - 1);
      for (int index : list) {
        int i = index - 1;
        if (type0->getDim(i) != dim) {
          assert(0 && "semantic error: incompatible indices in contraction");
        }
        if (index_set_to_erase.count(i)) {
          assert(0 && ("semantic error: index \'" + std::to_string(index) +
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
  }

  default:
    assert(0 && "internal error: invalid binary expression");
  }
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
  assert(0 && "semantic error: list cannot appear in typed expression");
}

void Sema::visitParenExpr(const ParenExpr *pe) { pe->getExpr()->visit(this); }

template <typename T, NodeType nt, typename Derived>
void Sema::visitNodeList(const NodeList<T, nt, Derived> *list) {
  for (const auto &i : *list)
    i->visit(this);
}

void Sema::visitProgram(const Program *p) {
  p->getDecls()->visit(this);
  p->getStmts()->visit(this);
}
