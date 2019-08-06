//===--- OptUtils.h ------ Optimizer Helper Functions -----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief This file provides some common utility functions for optimizers.
///
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_OPT_UTILS_H
#define PHAETON_OPT_OPT_UTILS_H

#include <iostream>
#include <sstream>
#include <string>

// TODO: add wrappers for expression tree indent formater
#define FORMAT_OPT_INDENT(indent)                                              \
  {                                                                            \
    std::cout.width((indent));                                                 \
    std::cout << std::left << "";                                              \
    std::cout.unsetf(std::ios::adjustfield);                                   \
  }

#endif // PHAETON_OPT_OPT_UTILS_H
