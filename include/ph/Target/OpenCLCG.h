//==------ OpenCLCG.h - Representation of code generation for OpenCL -------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language OpenCL.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_OPENCL_CG_H
#define PHAETON_TARGET_OPENCL_CG_H

#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/NaiveCG.h"

namespace phaeton {

class OpenCLCG : public NaiveCodeGen {};

} // end namespace phaeton

#endif // PHAETON_TARGET_OPENCL_CG_H
