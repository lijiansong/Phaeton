//==--- CudaCG.cpp ----- Interface to code generation for NVIDIA Cuda ------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for Nvidia Cuda code generation targeting the
// NVIDIA GPUs.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/CudaCG.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

using namespace phaeton;
