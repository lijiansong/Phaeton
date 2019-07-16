//==------ CodeGen.h - Representation of code generation -------------------==//
//
// This file defines the base class CodeGen.
//
//===----------------------------------------------------------------------===//

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <list>
#include <map>
#include <string>
#include <vector>

#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

// TODO: add wrappers for Codegen module
class CodeGen : public ASTVisitor {
private:
  const Sema *TheSema;

  int TempCounter;

  std::string Code;

  std::map<const TensorType *, std::string> EmittedTypes;

public:
  CodeGen(const Sema *sema);

  const Sema *getSema() const { return TheSema; }
  std::string getTemp();
  const std::string &getCode() const { return Code; }

  void append(const std::string &code) { Code += code; }

  bool typeEmitted(const TensorType *type) const;
  const std::string &getEmittedTypeName(const TensorType *type) const;
  void addEmittedType(const TensorType *type, const std::string &name);

public:
  using List = std::vector<int>;
  using Tuple = std::vector<int>;
  using TupleList = std::vector<Tuple>;

  enum Comparison {
    CMP_Less,
    CMP_LessEqual,
    CMP_Equal,
    CMP_GreaterEqual,
    CMP_Greater,

    CMP_COMPARISON_COUNT
  };

  static bool allCompare(const List &list, Comparison cmp, int pivot);

  static bool isPairList(const TupleList &list);
  static bool partitionPairList(int pivot, const TupleList &list,
                                TupleList &left, TupleList &right,
                                TupleList &mixed);
  static void shiftList(int shiftAmount, List &list);
  static void shiftTupleList(int shiftAmount, TupleList &tuple);
  static void flattenTupleList(const TupleList &list, std::list<int> &result);
  static void unpackPairList(const TupleList &list, List &left, List &right);
  static void adjustForContractions(List &indices,
                                    const TupleList &contractions);

  static const std::string getTupleListString(const TupleList &list);

  static const BinaryExpr *extractTensorExprOrNull(const Expr *e);
};

#endif // __CODEGEN_H__
