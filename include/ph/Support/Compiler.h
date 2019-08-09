//===--- Compiler.h - Compiler abstraction support --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines several macros, based on the current compiler.  This allows
// use of compiler-specific features in a way that remains portable.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_SUPPORT_COMPILER_H
#define PHAETON_SUPPORT_COMPILER_H

/// PH_ATTRIBUTE_NORETURN - Indicates that the function does not return. It is a
/// wrapper for C++11 attribute: noreturn.
#ifdef __GNUC__
#define PH_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define PH_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define PH_ATTRIBUTE_NORETURN
#endif

#endif // PHAETON_SUPPORT_COMPILER_H
