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

#ifndef __COMMON_OPTIONS_PARSER_H__
#define __COMMON_OPTIONS_PARSER_H__

#include "ph/Tooling/Colors.h"
#include "ph/Tooling/ToolingUtils.h"
#include "ph/Tooling/third-party/cxxopts/cxxopts.hpp"

// TODO: Refactoring command line options parser.
using ParseResult = cxxopts::ParseResult;
using Options = cxxopts::Options;
using OptionAdder = cxxopts::OptionAdder;

#endif // __COMMON_OPTIONS_PARSER_H__
