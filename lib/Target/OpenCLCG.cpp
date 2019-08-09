//==--- OpenCLCG.cpp ----- Interface to code generation for OpenCL ---------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class implementation for OpenCL code generation.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/OpenCLCG.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

using namespace phaeton;
