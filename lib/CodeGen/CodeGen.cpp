#include <cassert>
#include <functional>
#include <vector>

#include "tir/CodeGen/CodeGen.h"
#include "tir/Sema/Type.h"

#define TEMP_MAP_ASSERT(expr)                                                  \
  {                                                                            \
    if (ExprTemps.find((expr)) == ExprTemps.end())                             \
      assert(0 && "internal error: no temporary for 'Expr' node");             \
  }

bool CodeGen::isPairList(const TupleList &list) {
  for (const auto &l : list) {
    if (l.size() != 2)
      return false;
  }
  return true;
}

void CodeGen::shiftList(int shiftAmount, List &list) {
  for (int i = 0; i < list.size(); i++)
    list[i] += shiftAmount;
}

void CodeGen::shiftTupleList(int shiftAmount, TupleList &tuples) {
  for (auto &t : tuples)
    shiftList(shiftAmount, t);
}

void CodeGen::flattenTupleList(const TupleList &list, std::list<int> &result) {
  result.clear();
  for (const auto &tuple : list) {
    for (int elem : tuple)
      result.push_back(elem);
  }
  result.sort();
}

bool CodeGen::allCompare(const List &list, Comparison cmp, int pivot) {
  std::function<bool(int)> compare = [cmp, pivot](int i) {
    switch (cmp) {
    case CMP_Less:
      return i < pivot;
    case CMP_LessEqual:
      return i <= pivot;
    case CMP_Equal:
      return i == pivot;
    case CMP_GreaterEqual:
      return i >= pivot;
    case CMP_Greater:
      return i > pivot;
    default:
      assert(0 && "internal error: invalid comparison");
    }
  };

  for (const auto &i : list) {
    if (!compare(i))
      return false;
  }
  return true;
}

bool CodeGen::partitionPairList(int pivot, const TupleList &list,
                                TupleList &left, TupleList &right,
                                TupleList &mixed) {
  if (!isPairList(list))
    return false;

  left.clear();
  right.clear();
  mixed.clear();

  for (const auto &l : list) {
    if (allCompare(l, CMP_Less, pivot)) {
      left.push_back(l);
    } else if (allCompare(l, CMP_GreaterEqual, pivot)) {
      right.push_back(l);
    } else {
      mixed.push_back(l);
    }
  }

  return true;
}

void NumpyCodeGen::visitProgram(const Program *d) {
  append("# ----- Autogen kernel by TensorIR -----\n");
  append("import numpy as np\n\n");

  ASTVisitor::visitProgram(d);
}

void NumpyCodeGen::visitDecl(const Decl *d) {
  if (d->getNodeType() == NT_TypeDecl)
    return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = TheSema->getSymbol(name);
  append(sym->getName() + " = ");

  const TensorType &type = sym->getType();
  const std::string init = "np.random.rand(" + type.getDimString() + ")\n";
  append(init);
}

void NumpyCodeGen::visitStmt(const Stmt *s) {
  const std::string &name = s->getIdentifier()->getName();
  const Symbol *sym = TheSema->getSymbol(name);

  const Expr *expr = s->getExpr();
  expr->visit(this);
  TEMP_MAP_ASSERT(expr)

  const std::string &temp = getTempForExpr(expr);
  append(sym->getName() + " = " + temp + "\n");
}

static void orderLeftRight(CodeGen::TupleList &list) {
  for (auto &tuple : list) {
    assert(tuple.size() == 2);
    if (tuple[0] > tuple[1]) {
      int tmp = tuple[0];
      tuple[0] = tuple[1];
      tuple[1] = tmp;
    }
  }
}

static int countLess(const std::list<int> &contracted, int bound) {
  int counter = 0;
  for (int i : contracted)
    counter += (i < bound);
  return counter;
}

static void adjustIndices(CodeGen::List &indices,
                          const std::list<int> &contracted) {
  for (int i = 0; i < indices.size(); i++) {
    int adj = countLess(contracted, indices[i]);
    indices[i] -= adj;
  }
}

static void adjustTuples(CodeGen::TupleList &tuples,
                         const std::list<int> &contracted) {
  for (auto &t : tuples)
    adjustIndices(t, contracted);
}

static void zipPairs(const CodeGen::TupleList &pairs,
                     CodeGen::TupleList &result) {
  CodeGen::Tuple first, second;

  for (const auto pair : pairs) {
    assert(pair.size() == 2);
    first.push_back(pair[0]);
    second.push_back(pair[1]);
  }

  result.clear();
  result.push_back(first);
  result.push_back(second);
}

static const std::string getTupleListString(const CodeGen::TupleList &list) {
  std::string result = "[";
  for (int l = 0; l < list.size(); l++) {
    result += "[";
    auto &tuple = list[l];
    for (int i = 0; i < tuple.size(); i++) {
      result += std::to_string(tuple[i]);
      if (i != (tuple.size() - 1))
        result += ", ";
    }
    result += "]";
    if (l != (list.size() - 1))
      result += ", ";
  }
  result += "]";
  return result;
}

static const std::string getTensorDotString(const std::string &r,
                                            const std::string &t0,
                                            const std::string &t1,
                                            const std::string axes = "") {
  if (!axes.length())
    return r + " = np.tensordot(" + t0 + ", " + t1 + ", axes=0)\n";
  else
    return r + " = np.tensordot(" + t0 + ", " + t1 + ", axes=" + axes + ")\n";
}

const std::string NumpyCodeGen::visitContraction(const Expr *e,
                                                 const TupleList &indices) {
  if (indices.empty()) {
    e->visit(this);
    TEMP_MAP_ASSERT(e);
    return getTempForExpr(e);
  }

  const BinaryExpr *tensor = dynamic_cast<const BinaryExpr *>(e);
  if (!tensor || (tensor->getNodeType() != NT_TensorExpr))
    assert(0 && "internal error: cannot handle general contractions yet");

  if (!isPairList(indices))
    assert(0 && "internal error: only pairs of indices can be contracted");

  const Expr *tensorL = tensor->getLeft();
  const TensorType *typeL = TheSema->getType(tensorL);

  const Expr *tensorR = tensor->getRight();
  const TensorType *typeR = TheSema->getType(tensorR);

  int pivot = typeL->getRank();
  TupleList contrL, contrR, contrMixed;
  partitionPairList(pivot, indices, contrL, contrR, contrMixed);

  TupleList shiftedR = contrR;
  shiftTupleList(-pivot, shiftedR);

  const std::string tempL = visitContraction(tensorL, contrL);
  const std::string tempR = visitContraction(tensorR, shiftedR);

  const std::string result = getTemp();

  if (contrMixed.empty()) {
    append(getTensorDotString(result, tempL, tempR));
    return result;
  }

  std::list<int> flatL, flatR;
  flattenTupleList(contrL, flatL);
  flattenTupleList(contrR, flatR);
  flatL.merge(flatR);

  TupleList zipped;
  adjustTuples(contrMixed, flatL);
  orderLeftRight(contrMixed);
  zipPairs(contrMixed, zipped);
  shiftList(-(pivot - (2 * contrL.size())), zipped[1]);

  append(getTensorDotString(result, tempL, tempR, getTupleListString(zipped)));
  return result;
}

void NumpyCodeGen::visitBinaryExpr(const BinaryExpr *be) {
  switch (be->getNodeType()) {
  case NT_TensorExpr: {
    const Expr *left = be->getLeft();
    left->visit(this);
    TEMP_MAP_ASSERT(left);
    const std::string &t0 = getTempForExpr(left);

    const Expr *right = be->getRight();
    right->visit(this);
    TEMP_MAP_ASSERT(right);
    const std::string &t1 = getTempForExpr(right);

    const std::string &temp = addTempForExpr(be);
    append(getTensorDotString(temp, t0, t1));
    return;
  }
  case NT_DotExpr: {
    const BinaryExpr *tensor = dynamic_cast<const BinaryExpr *>(be->getLeft());
    if (!tensor || (tensor->getNodeType() != NT_TensorExpr))
      assert(0 && "internal error: cannot handle general contractions yet");

    TupleList contractionsList;
    if (!TheSema->isListOfLists(be->getRight(), contractionsList))
      assert(0 && "internal error: cannot have a non-list here");

    if (contractionsList.empty())
      assert(0 && "internal error: cannot have an empty list here");

    const std::string temp = visitContraction(tensor, contractionsList);
    addNameForExpr(be, temp);
    return;
  }
  default:
    assert(0 && "internal error: invalid binary expression");
  }
}

void NumpyCodeGen::visitIdentifier(const Identifier *id) {
  addNameForExpr(id, id->getName());
}

void NumpyCodeGen::visitInteger(const Integer *i) {
  const std::string &temp = addTempForExpr(i);
  append(temp + " = " + std::to_string(i->getValue()));
}

void NumpyCodeGen::visitBrackExpr(const BrackExpr *be) {
  assert(0 && "codegen error: cannot generate code for list in expression");
}
