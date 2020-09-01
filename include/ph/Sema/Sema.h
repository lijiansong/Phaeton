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
  /// ElemInfo - keep track of the following information.
  struct ElemInfo {
    // Is directive for element loop present
    bool Present;
    // Position of element index, e.g. first, last
    ElementDirective::POSSpecifier Pos;
    unsigned Dim;
    // Symbols for Identifiers that take an element index
    std::set<const Symbol *> Syms;
  };

public:
  Sema();
  ~Sema();

  const TensorDataType *createType(const std::vector<int> &Dim);
  const TensorDataType *getType(const std::vector<int> &Dim);
  const TensorDataType *getType(const std::vector<int> &Dim) const;
  const TensorDataType *getType(const Expression *E) const {
    return ExprTypes.at(E);
  }
  const TensorDataType &getScalar() const { return *Scalar; }
  bool isScalar(const TensorDataType &Type) const {
    return (Type == getScalar());
  }

  const Symbol *createSymbol(Symbol::SymbolKind SK, const std::string &Name,
                             const TensorDataType &Type,
                             const Decl *D = nullptr);

  const Symbol *getSymbol(const std::string &Name) const;

  bool isTypeName(const Expression *E, const TensorDataType *&Type) const;
  static bool isIntegerList(const Expression *E, std::vector<int> &Ints);
  static bool isListOfLists(const Expression *E,
                            std::vector<std::vector<int>> &Lists);

  const TensorDataType *visitTypeExpr(const Expression *E);

#define GEN_VISIT_EXPRNODE_DECL(ExprName)                                      \
  virtual void visit##ExprName(const ExprName *E) override;

  GEN_VISIT_EXPRNODE_DECL(Decl)
  GEN_VISIT_EXPRNODE_DECL(Stmt)
  GEN_VISIT_EXPRNODE_DECL(ElementDirective)
  GEN_VISIT_EXPRNODE_DECL(BinaryExpr)
  GEN_VISIT_EXPRNODE_DECL(Identifier)
  GEN_VISIT_EXPRNODE_DECL(Integer)
  GEN_VISIT_EXPRNODE_DECL(BrackExpr)
  GEN_VISIT_EXPRNODE_DECL(ParenExpr)

#undef GEN_VISIT_EXPRNODE_DECL

  std::list<const TensorDataType *>::const_iterator types_begin() const {
    return Types.begin();
  }
  std::list<const TensorDataType *>::const_iterator types_end() const {
    return Types.end();
  }

  bool is_in_inputs(const std::string &Name) const {
    const Symbol *Sym = getSymbol(Name);
    for (auto s : Inputs) {
      if (s == Sym)
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

  bool is_in_outputs(const std::string &Name) const {
    const Symbol *Sym = getSymbol(Name);
    for (auto s : Outputs) {
      if (s == Sym)
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

  bool isNamedType(const TensorDataType *Type) const;

  const Symbol *getTypeSymbol(const TensorDataType *Type) const;

  const ElemInfo &getElemInfo() const { return ElementInfo; }

private:
  const TensorDataType *Scalar;
  std::list<const TensorDataType *> Types;

  SymbolTable Symbols;

  // vector for keeping track of symbols of variables that are
  // declared with in out specifiers
  std::vector<const Symbol *> Inputs;
  std::vector<const Symbol *> Outputs;

  // map for 'Expr' AST nodes to types
  std::map<const Expression *, const TensorDataType *> ExprTypes;

  // map user-defined types to the corresponding symbols holding the type names
  std::map<const TensorDataType *, const Symbol *> NamedTypes;

  // for element directive
  ElemInfo ElementInfo;
};

} // end namespace phaeton

#endif // PHAETON_SEMA_H
