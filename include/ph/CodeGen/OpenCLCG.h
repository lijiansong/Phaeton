//==------ OpenCLCG.h - Representation of code generation for OpenCL -------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language OpenCL.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_OPENCL_CG_H
#define PHAETON_CODEGEN_OPENCL_CG_H

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"

namespace phaeton {

class OpenCLCG : public DirectCodeGen {};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_OPENCL_CG_H
