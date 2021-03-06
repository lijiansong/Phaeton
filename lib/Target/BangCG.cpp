//==--- BangCG.cpp ----- Interface to code generation for Cambricon Bang ---==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for Cambricon Bang code generation targeting the
// Cambricon MLUs.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/BangCG.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

using namespace phaeton;
