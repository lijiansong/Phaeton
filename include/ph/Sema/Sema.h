//===--- Sema.h - Semantic Analysis & AST Building --------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the Sema class, which performs semantic analysis and
// builds ASTs.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_SEMA_H
#define PHAETON_SEMA_H

#include "ph/AST/AST.h"
#include "ph/Sema/SymbolTable.h"
#include "ph/Sema/Type.h"

#include <list>
#include <map>
#include <set>
#include <vector>

namespace phaeton {

class Sema : public ASTVisitor {
public:
  struct ElemInfo {
    // Is directive for element loop present
    bool present;

    // Position of element index, e.g. first, last
    ElemDirect::POSSpecifier pos;

    unsigned dim;

    // Symbols for Identifiers that take an element index
    std::set<const Symbol *> syms;
  };

public:
  Sema();
  ~Sema();

  const TensorType *createType(const std::vector<int> &dims);
  const TensorType *getType(const std::vector<int> &dims);
  const TensorType *getType(const std::vector<int> &dims) const;
  const TensorType *getType(const Expr *e) const { return ExprTypes.at(e); }
  const TensorType &getScalar() const { return *scalar; }
  bool isScalar(const TensorType &t) const { return (t == getScalar()); }

  const Symbol *createSymbol(Symbol::SymbolKind k, const std::string &name,
                             const TensorType &type,
                             const Decl *decl = nullptr);

  const Symbol *getSymbol(const std::string &name) const;

  bool isTypeName(const Expr *e, const TensorType *&type) const;
  static bool isIntegerList(const Expr *e, std::vector<int> &ints);
  static bool isListOfLists(const Expr *e,
                            std::vector<std::vector<int>> &lists);

  const TensorType *visitTypeExpr(const Expr *e);

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitElemDirect(const ElemDirect *ed) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  std::list<const TensorType *>::const_iterator types_begin() const {
    return Types.begin();
  }
  std::list<const TensorType *>::const_iterator types_end() const {
    return Types.end();
  }

  bool is_in_inputs(const std::string &name) const {
    const Symbol *sym = getSymbol(name);
    for (auto s : Inputs) {
      if (s == sym)
        return true;
    }
    return false;
  }

  std::vector<const Symbol *>::const_iterator inputs_begin() const {
    return Inputs.begin();
  }

  std::vector<const Symbol *>::const_iterator inputs_end() const {
    return Inputs.end();
  }

  int inputs_size() const { return Inputs.size(); }

  bool is_in_outputs(const std::string &name) const {
    const Symbol *sym = getSymbol(name);
    for (auto s : Outputs) {
      if (s == sym)
        return true;
    }
    return false;
  }

  std::vector<const Symbol *>::const_iterator outputs_begin() const {
    return Outputs.begin();
  }

  std::vector<const Symbol *>::const_iterator outputs_end() const {
    return Outputs.end();
  }

  int outputs_size() const { return Outputs.size(); }

  bool isNamedType(const TensorType *type) const;

  const Symbol *getTypeSymbol(const TensorType *type) const;

  const ElemInfo &getElemInfo() const { return elemInfo; }

private:
  const TensorType *scalar;
  std::list<const TensorType *> Types;

  SymbolTable Symbols;

  // map for 'Expr' AST nodes to types
  std::map<const Expr *, const TensorType *> ExprTypes;

  // vector for symbols of variables that are declared with in out specifiers
  std::vector<const Symbol *> Inputs;
  std::vector<const Symbol *> Outputs;

  // map user-defined types to the corresponding symbols holding the type names
  std::map<const TensorType *, const Symbol *> NamedTypes;

  // for elem directive
  ElemInfo elemInfo;
};

} // end namespace phaeton

#endif // PHAETON_SEMA_H
