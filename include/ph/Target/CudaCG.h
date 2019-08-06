//==------ CudaCG.h ------ Representation of code generation for Cuda ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language NVIDIA CUDA.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_CUDA_CG_H
#define PHAETON_TARGET_CUDA_CG_H

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"

namespace phaeton {

/// CudaCG - This class is the entrance for NVIDIA Cuda code generation.
class CudaCG : public DirectCodeGen {};

} // end namespace phaeton

#endif // PHAETON_TARGET_CUDA_CG_H
