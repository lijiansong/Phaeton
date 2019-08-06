//===--- ToolingUtils.h - Tooling Helper Functions --------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief This file provides some common utility functions for processing
/// command line options.
///
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TOOLING_UTILS_H
#define PHAETON_TOOLING_UTILS_H

#include <algorithm>
#include <cctype>
#include <string>

namespace phaeton {

void toUpperCase(std::string &str) {
  std::transform(
      str.begin(), str.end(), str.begin(),
      [](unsigned char c) -> unsigned char { return std::toupper(c); });
}

} // end namespace phaeton

#endif // PHAETON_TOOLING_UTILS_H
