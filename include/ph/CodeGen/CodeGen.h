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

class ExpressionNode;
class ExpressionNodeBuilder;

class CodeGen : public ASTVisitor {
public:
  // Note: A program consists of delcared identifiers and assignments.
  // A program has input and output specifier, both of which
  // are generated as function arguments.
  struct Assignment {
    ExpressionNode *LHS;
    ExpressionNode *RHS;
  };
  struct FuncArg {
    int Position;
    std::string Name;
  };
  using DeclaredIdListTy = std::list<ExpressionNode *>;
  using AssignmentsListTy = std::list<Assignment>;
  using FuncArgsListTy = std::vector<FuncArg>;

public:
  // FIXME: remove CG function name latter.
  CodeGen(const Sema *S, const std::string &FuncName);
  virtual ~CodeGen();

  // Helper methods for accessing and manipulating this object's state.
  const Sema *getSema() const { return ThisSema; }
  std::string getTmp();
  const std::string &getTgtLangCode() const { return TgtLangCode; }
  void appendCode(const std::string &Code) { TgtLangCode += Code; }
  const std::string &getCGFuncName() const { return CGFuncName; }

  // Helper methods for accessing and manipulating state
  // that is built up during CodeGen.
  ExpressionNodeBuilder *getExprNodeBuilder() { return ExprNodeBuilder; }

  virtual void addDeclaredId(const Decl *D);
  virtual void addAssignment(const Stmt *S);
  virtual void addFuncArg(const std::string &Name);

  DeclaredIdListTy &getDeclaredIds() { return DeclaredIds; }
  AssignmentsListTy &getAssignments() { return Assignments; }
  FuncArgsListTy &getFuncArgs() { return FuncArgs; }

  const DeclaredIdListTy &getDeclaredIds() const { return DeclaredIds; }
  const AssignmentsListTy &getAssignments() const { return Assignments; }
  const FuncArgsListTy &getFuncArgs() const { return FuncArgs; };

  unsigned getNumFuncArgs() const { return FuncArgs.size(); };
  const FuncArg &getFuncArg(unsigned I) const;

  // Decl and Stmt visitor for CodeGen.
  virtual void visitDecl(const Decl *D) override;
  virtual void visitStmt(const Stmt *S) override;

  // Helper methods for CodeGen.
  using List = std::vector<int>;
  using Tuple = std::vector<int>;
  using TupleList = std::vector<Tuple>;
  enum ComparisonKind {
    CMP_Less,
    CMP_LessEqual,
    CMP_Equal,
    CMP_GreaterEqual,
    CMP_Greater,

    CMP_ComparisonKind_COUNT
  };
  static bool allCompare(const List &L, ComparisonKind Cmp, int Pivot);
  static bool isPairList(const TupleList &TList);
  static bool partitionPairList(int Pivot, const TupleList &TList,
                                TupleList &Left, TupleList &Right,
                                TupleList &Mixed);
  static void shiftList(int ShiftAmount, List &L);
  static void shiftTupleList(int ShiftAmount, TupleList &TList);
  static void flattenTupleList(const TupleList &TList, std::list<int> &Result);
  static void unpackPairList(const TupleList &TList, List &Left, List &Right);
  static void adjustForContractions(List &Indices,
                                    const TupleList &Contractions);
  static const std::string getListString(const List &L);
  static const std::string getTupleListString(const TupleList &TList);
  static const BinaryExpr *extractTensorExprOrNull(const Expression *E);
  using DimsTy = std::vector<int>;
  using TmpVecTy = std::vector<std::string>;

  std::string getTmpWithDims(const DimsTy &Dims) {
    TmpVecTy &Tmps = FreeTmps[Dims];

    if (Tmps.size() == 0) {
      return getTmp();
    } else {
      auto Result = Tmps.back();
      Tmps.pop_back();
      return Result;
    }
  }

  void freeTmpWithDims(const std::string &Name, const DimsTy &Dims) {
    TmpVecTy &Tmps = FreeTmps[Dims];
    Tmps.push_back(Name);
  }

protected:
  // States that are built up during code generation.

  // ExprNodeBuilder is used for creating new 'ExpressionNode' objects, hence we
  // need to keep track of these information.
  ExpressionNodeBuilder *ExprNodeBuilder;

  // Some components of the result target language program.
  DeclaredIdListTy DeclaredIds;
  AssignmentsListTy Assignments;
  FuncArgsListTy FuncArgs;

  // Note: map for Expr in AST, each 'Expr' in the AST is mapped to an
  // 'ExpressionNode'.
  std::map<const Expression *, ExpressionNode *> ExprTrees;
  void assertExprTreeMap(const Expression *E) const;

  void addExpressionNode(const Expression *E, ExpressionNode *ENode) {
    ExprTrees[E] = ENode;
  }
  ExpressionNode *getExpressionNode(const Expression *E) const {
    return ExprTrees.at(E);
  }

private:
  std::map<DimsTy, TmpVecTy> FreeTmps;
  // Member variables that tracks the state of this object.
  const Sema *ThisSema;
  int TmpCounter;
  std::string TgtLangCode;
  const std::string CGFuncName;
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_H
