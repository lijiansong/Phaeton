//===--- ASTUtils.h --------- AST Helper Functions --------------*- C++ -*-===//
//
/// \file
/// \brief This file provides some common utility functions for processing
/// AST operations.
///
//===----------------------------------------------------------------------===//

#ifndef __ASTUTILS_H__
#define __ASTUTILS_H__

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

// TODO: add log wrapper
#define FORMAT_INDENT(indent)                                                  \
  {                                                                            \
    std::cout.width((indent));                                                 \
    std::cout << std::left << "";                                              \
    std::cout.unsetf(std::ios::adjustfield);                                   \
  }

// global var, TODO: class wrapper

#endif // __ASTUTILS_H__
