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

bool SymbolTable::addSymbol(Symbol *sym) {
  if (Symbols.count(sym->getName()))
    return false;

  Symbols[sym->getName()] = sym;
  return true;
}

bool SymbolTable::getSymbol(const std::string &name, Symbol *&sym) const {
  auto it = Symbols.find(name);
  if (it == Symbols.end())
    return false;

  sym = it->second;
  return true;
}
