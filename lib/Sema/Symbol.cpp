#include "tir/Sema/Symbol.h"

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
