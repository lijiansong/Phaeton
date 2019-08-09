//==--- CCECG.cpp ----- Interface to code generation for Huawei CCE --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for Cambricon Bang code generation targeting the
// Huawei Ascend accelerators.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/CCECG.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

using namespace phaeton;
