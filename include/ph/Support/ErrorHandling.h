//===--- ErrorHandling.h - Fatal error handling -----------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines an API used to indicate fatal error conditions.  Non-fatal
// errors (most of them) should be handled through Phaeton Context.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_SUPPORT_ERROR_HANDLING_H
#define PHAETON_SUPPORT_ERROR_HANDLING_H

#include "ph/Support/Colors.h"
#include "ph/Support/Compiler.h"

#include <iostream>

#define INTERNAL_ERROR FRED("internal error: ")
#define SYNTAX_ERROR FRED("syntax error: ")
#define SEMANTIC_ERROR FRED("semantic error: ")

namespace phaeton {

/// This function calls abort(), and prints the optional message to stderr.
/// Use the llvm_unreachable macro (that adds location info), instead of
/// calling this function directly.
///
/// Note that we must mark 'ph_unreachable_internal' as 'static' to make the
/// compiler happy, otherwise we will get a duplicate symbols error for this
/// function, since this header file will be included in mutiple source files,
/// using 'static' we can make this function local in each file.
PH_ATTRIBUTE_NORETURN static void
ph_unreachable_internal(const char *msg = nullptr, const char *file = nullptr,
                        unsigned line = 0) {
  if (msg)
    std::cerr << msg << "\n";
  std::cerr << "UNREACHABLE executed";
  if (file)
    std::cerr << " at " << file << ":" << line;
  std::cerr << "!\n";
  abort();
}

} // end namespace phaeton

/// Prints the message and location info to stderr in !NDEBUG builds.
/// This is intended to be used for "impossible" situations that imply
/// a bug in the compiler.
///
/// In NDEBUG mode it only prints "UNREACHABLE executed".
/// Use this instead of assert(0), so that the compiler knows this path
/// is not reachable even for NDEBUG builds.
#ifndef NDEBUG
#define ph_unreachable(msg)                                                    \
  ::phaeton::ph_unreachable_internal(msg, __FILE__, __LINE__)
#else
#define ph_unreachable(msg) ::phaeton::ph_unreachable_internal(msg)
#endif

#endif // PHAETON_SUPPORT_ERROR_HANDLING_H
