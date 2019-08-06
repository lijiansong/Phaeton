//==------ CodeGen.h - Representation of code generation -------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_H
#define PHAETON_CODEGEN_H

#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#include <list>
#include <map>
#include <string>
#include <vector>

namespace phaeton {

class ExprNode;
class ExprNodeBuilder;

class CodeGen : public ASTVisitor {
public:
  // Note: A program consists of delcared identifiers and assignments.
  // A program has input and output specifier, both of which
  // are generated as function arguments.
  struct Assignment {
    ExprNode *lhs;
    ExprNode *rhs;
  };
  struct FunctionArgument {
    int position;
    std::string name;
  };
  using DeclaredIdListTy = std::list<ExprNode *>;
  using AssignmentsListTy = std::list<Assignment>;
  using FunctionArgumentsListTy = std::vector<FunctionArgument>;

public:
  CodeGen(const Sema *sema, const std::string &functionName);
  virtual ~CodeGen();

  // Helper methods for accessing and manipulating this object's state.
  const Sema *getSema() const { return TheSema; }
  std::string getTemp();
  const std::string &getCode() const { return Code; }
  void append(const std::string &code) { Code += code; }
  const std::string &getFunctionName() const { return FunctionName; }

  // Helper methods for accessing and manipulating state
  // that is built up during CodeGen.
  ExprNodeBuilder *getENBuilder() { return ENBuilder; }

  virtual void addDeclaredId(const Decl *d);
  virtual void addAssignment(const Stmt *s);
  virtual void addFunctionArgument(const std::string &name);

  DeclaredIdListTy &getDeclaredIds() { return DeclaredIds; }
  AssignmentsListTy &getAssignments() { return Assignments; }
  FunctionArgumentsListTy &getFunctionArguments() { return FunctionArguments; }

  const DeclaredIdListTy &getDeclaredIds() const { return DeclaredIds; }
  const AssignmentsListTy &getAssignments() const { return Assignments; }
  const FunctionArgumentsListTy &getFunctionArguments() const {
    return FunctionArguments;
  };

  unsigned getNumFunctionArguments() const { return FunctionArguments.size(); };
  const FunctionArgument &getFunctionArgument(unsigned i) const;

  // Decl and Stmt visitor for CodeGen.
  virtual void visitDecl(const Decl *decl) override;
  virtual void visitStmt(const Stmt *stmt) override;

  // Helper methods for CodeGen.
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
  static const std::string getListString(const List &list);
  static const std::string getTupleListString(const TupleList &list);
  static const BinaryExpr *extractTensorExprOrNull(const Expr *e);
  using DimsTy = std::vector<int>;
  using TempVecTy = std::vector<std::string>;

  std::string getTempWithDims(const DimsTy &dims) {
    TempVecTy &temps = FreeTemps[dims];

    if (temps.size() == 0) {
      return getTemp();
    } else {
      auto result = temps.back();
      temps.pop_back();
      return result;
    }
  }

  void freeTempWithDims(const std::string &name, const DimsTy &dims) {
    TempVecTy &temps = FreeTemps[dims];
    temps.push_back(name);
  }

protected:
  // States that are built up during code generation.

  // ENBuilder is used for creating new 'ExprNode' objects, so we
  // need to record some information.
  ExprNodeBuilder *ENBuilder;

  // Some components of the result target language program.
  DeclaredIdListTy DeclaredIds;
  AssignmentsListTy Assignments;
  FunctionArgumentsListTy FunctionArguments;

  // Note: map for Expr in AST, each 'Expr' in the AST be mapped to an
  // 'ExprNode'.
  std::map<const Expr *, ExprNode *> ExprTrees;
  void EXPR_TREE_MAP_ASSERT(const Expr *expr) const;

  void addExprNode(const Expr *expr, ExprNode *en) { ExprTrees[expr] = en; }
  ExprNode *getExprNode(const Expr *expr) const { return ExprTrees.at(expr); }

private:
  std::map<DimsTy, TempVecTy> FreeTemps;
  // Member variables that tracks the state of this object.
  const Sema *TheSema;
  int TempCounter;
  std::string Code;
  const std::string FunctionName;
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_H
