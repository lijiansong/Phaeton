//===--- SymbolTable.h - Classes for representing symbol table --*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines symbol table to track variable information.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_SEMA_SYMBOL_TABLE_H
#define PHAETON_SEMA_SYMBOL_TABLE_H

#include "ph/AST/AST.h"
#include "ph/Sema/Type.h"

#include <map>
#include <string>

namespace phaeton {

/// Symbol - This class keep track of each symbol in phaeton.
class Symbol {
public:
  enum SymbolKind {
    SYM_KIND_Variable,
    SYM_KIND_Type,

    SYM_KIND_SYMBOLKIND_COUNT
  };

public:
  Symbol(SymbolKind SK, const std::string &Name, const TensorType &Type,
         const Decl *Decl = nullptr)
      : SK(SK), Name(Name), Type(Type), DeclNode(Decl) {}

  SymbolKind getSymbolKind() const { return SK; }
  const std::string &getName() const { return Name; }
  const TensorType &getType() const { return Type; }
  const Decl *getDecl() const { return DeclNode; }

  void setDecl(const Decl *Decl) { DeclNode = Decl; }

private:
  const SymbolKind SK;
  const std::string Name;
  const TensorType &Type;
  const Decl *DeclNode;
};

/// SymbolTable - This class along with Symbol to implement semantic analysis
/// for type checking.
class SymbolTable {
public:
  using SymbolMap = std::map<const std::string, Symbol *>;

public:
  SymbolTable() {}

  bool addSymbol(Symbol *Sym);

  bool getSymbol(const std::string &Name, Symbol *&Sym) const;

  SymbolMap::iterator begin() { return Symbols.begin(); }
  SymbolMap::iterator end() { return Symbols.end(); }

  SymbolMap::const_iterator begin() const { return Symbols.begin(); }
  SymbolMap::const_iterator end() const { return Symbols.end(); }

private:
  SymbolMap Symbols;
};

} // end namespace phaeton

#endif // PHAETON_SEMA_SYMBOL_TABLE_H
