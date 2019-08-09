//===--- ASTUtils.h --------- AST Helper Functions --------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief This file provides some common utility functions for processing
/// AST operations.
///
//===----------------------------------------------------------------------===//

#ifndef PHAETON_AST_ASTUTILS_H
#define PHAETON_AST_ASTUTILS_H

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

// TODO: add log wrapper.
// Current AST style refer to Swift AST, using a list pattern.
#define FORMAT_AST_INDENT(INDENT)                                              \
  {                                                                            \
    std::cout.width((INDENT));                                                 \
    std::cout << std::left << "";                                              \
    std::cout.unsetf(std::ios::adjustfield);                                   \
  }

// global var, TODO: class wrapper

#endif // PHAETON_AST_ASTUTILS_H
