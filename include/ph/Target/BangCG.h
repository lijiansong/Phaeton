//==------ BangCG.h ------ Representation of code generation for Bang ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language Cambricon Bang.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_BANG_CG_H
#define PHAETON_TARGET_BANG_CG_H

#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/NaiveCG.h"

#include <string>

namespace phaeton {

/// BangCG - this class is the entrance of code generation for Cambricon Bang
/// with SIMD intrinsics.
class BangCG : public NaiveCodeGen {};

} // end namespace phaeton

#endif // PHAETON_TARGET_BANG_CG_H
