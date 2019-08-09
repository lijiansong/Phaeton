//===- CommonOptionsParser.h - common options for ph tools ----*- C++ -*-=====//
//
//                     The Pheaton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements the CommonOptionsParser class used to parse common
//  command-line options for phaeton tools, so that they can be run as separate
//  command-line applications with a consistent common interface for handling
//  compilation database and input files.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TOOLING_COMMON_OPTIONS_PARSER_H
#define PHAETON_TOOLING_COMMON_OPTIONS_PARSER_H

#include "ph/Support/Colors.h"
#include "ph/Tooling/ToolingUtils.h"
#include "ph/Tooling/third-party/cxxopts/cxxopts.hpp"

namespace phaeton {

// TODO: Refactoring command line options parser.
using ParseResult = cxxopts::ParseResult;
using Options = cxxopts::Options;
using OptionAdder = cxxopts::OptionAdder;

} // end namespace phaeton

#endif // PHAETON_TOOLING_COMMON_OPTIONS_PARSER_H
