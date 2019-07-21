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

#ifndef __TOOLING_UTILS_H__
#define __TOOLING_UTILS_H__

#include <algorithm>
#include <cctype>
#include <string>

void toUpperCase(std::string &str) {
  std::transform(
      str.begin(), str.end(), str.begin(),
      [](unsigned char c) -> unsigned char { return std::toupper(c); });
}

#endif // __TOOLING_UTILS_H__
