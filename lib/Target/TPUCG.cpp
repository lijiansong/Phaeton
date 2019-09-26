//==--- TPUCG.cpp ----- Interface to code generation for Google TPU --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for HLO(High-Level Optimization) IR code generation
// targeting the Google TPUs.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/TPUCG.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

using namespace phaeton;
