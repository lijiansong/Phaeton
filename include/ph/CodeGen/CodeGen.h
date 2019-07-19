//==------ CodeGen.h - Representation of code generation -------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen.
//
//===----------------------------------------------------------------------===//

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#include <list>
#include <map>
#include <string>
#include <vector>

class ExprNode;
class ExprNodeBuilder;

// TODO: add wrappers for Codegen module
class CodeGen : public ASTVisitor {
private:
  const Sema *TheSema;

  int TempCounter;

  std::string Code;

  std::map<const TensorType *, std::string> EmittedTypes;

  // map for each 'Expr' in the AST to an expression tree,
  // which rooted at an 'ExprNode'
  std::map<const Expr *, ExprNode *> ExprTrees;

public:
  struct Assignment {
    std::string variable;
    const ExprNode *en;
  };

protected:
  ExprNodeBuilder *ENBuilder;

  void EXPR_TREE_MAP_ASSERT(const Expr *expr) const;

public:
  ExprNodeBuilder *getExprNodeBuilder() { return ENBuilder; }

  void addExprNode(const Expr *expr, ExprNode *en) { ExprTrees[expr] = en; }
  ExprNode *getExprNode(const Expr *expr) const { return ExprTrees.at(expr); }

public:
  CodeGen(const Sema *sema);
  virtual ~CodeGen();

  const Sema *getSema() const { return TheSema; }
  std::string getTemp();
  const std::string &getCode() const { return Code; }

  void append(const std::string &code) { Code += code; }

  bool typeEmitted(const TensorType *type) const;
  const std::string &getEmittedTypeName(const TensorType *type) const;
  void addEmittedType(const TensorType *type, const std::string &name);

private:
  std::list<const Decl *> Declarations;
  std::list<const Stmt *> Statements;

public:
  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  const std::list<const Decl *> &getDeclarations() const {
    return Declarations;
  }
  const std::list<const Stmt *> &getStatements() const { return Statements; }

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

  static const std::string getListString(const List &list);
  static const std::string getTupleListString(const TupleList &list);

  static const BinaryExpr *extractTensorExprOrNull(const Expr *e);
};
#endif // __CODEGEN_H__
