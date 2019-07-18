//===--- OptUtils.h ------ Optimizer Helper Functions -----------*- C++ -*-===//
//
/// \file
/// \brief This file provides some common utility functions for optimizers.
///
//===----------------------------------------------------------------------===//

#ifndef __OPT_UTILS_H__
#define __OPT_UTILS_H__

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

#endif // __OPT_UTILS_H__
