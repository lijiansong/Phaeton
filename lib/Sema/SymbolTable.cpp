//===--- SymbolTable.cpp - Interfaces to symbol table ---------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements helper interfaces of symbol table to track variable
//  information.
//
//===----------------------------------------------------------------------===//

#include "ph/Sema/SymbolTable.h"

using namespace phaeton;

bool SymbolTable::addSymbol(Symbol *S) {
  if (Symbols.count(S->getName())) {
    return false;
  }

  Symbols[S->getName()] = S;
  return true;
}

bool SymbolTable::getSymbol(const std::string &Name, Symbol *&Sym) const {
  auto It = Symbols.find(Name);
  if (It == Symbols.end()) {
    return false;
  }

  Sym = It->second;
  return true;
}
