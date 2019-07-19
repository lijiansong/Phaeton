//===--- Parser.cpp - Phaeton Language Parser -----------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements the Parser interfaces.
//
//===----------------------------------------------------------------------===//

#include "ph/Parse/Parser.h"

int Parser::parse() { return yyparse(lexer.getScanner(), ast); }
