#include <cassert>
#include <functional>
#include <list>
#include <vector>

#include "ph/CodeGen/CodeGen.h"
#include "ph/Sema/Type.h"

CodeGen::CodeGen(const Sema *sema) : TheSema(sema), TempCounter(0), Code("") {}

std::string CodeGen::getTemp() { return "t" + std::to_string(TempCounter++); }

bool CodeGen::typeEmitted(const TensorType *type) const {
  return EmittedTypes.count(type);
}

const std::string &CodeGen::getEmittedTypeName(const TensorType *type) const {
  assert(typeEmitted(type));
  return EmittedTypes.at(type);
}

void CodeGen::addEmittedType(const TensorType *type, const std::string &name) {
  assert(!typeEmitted(type));
  EmittedTypes[type] = name;
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

bool CodeGen::isPairList(const TupleList &list) {
  for (const auto &l : list) {
    if (l.size() != 2)
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

void CodeGen::unpackPairList(const TupleList &list, List &left, List &right) {
  assert(isPairList(list));

  left.clear();
  right.clear();
  for (const auto &tuple : list) {
    int l = (tuple[0] < tuple[1]) ? tuple[0] : tuple[1];
    int r = (tuple[0] < tuple[1]) ? tuple[1] : tuple[0];

    left.push_back(l);
    right.push_back(r);
  }
}

void CodeGen::adjustForContractions(List &indices,
                                    const TupleList &contractions) {
  assert(isPairList(contractions));

  for (int i = 0; i < indices.size(); i++) {
    int index = indices[i];
    int adj = 0;
    for (const Tuple &t : contractions)
      adj += (t[0] < index) + (t[1] < index);

    indices[i] -= adj;
  }
}

const std::string CodeGen::getTupleListString(const TupleList &list) {
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

const BinaryExpr *CodeGen::extractTensorExprOrNull(const Expr *e) {
  const BinaryExpr *tensor = dynamic_cast<const BinaryExpr *>(e);
  if (!tensor) {
    const ParenExpr *paren = dynamic_cast<const ParenExpr *>(e);
    if (paren)
      return extractTensorExprOrNull(paren->getExpr());
    else
      return nullptr;
  }
  assert(tensor);

  if (tensor->getNodeType() != ASTNode::NT_ProductExpr)
    return nullptr;

  return tensor;
}
