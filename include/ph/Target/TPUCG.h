//==--- TPUCG.h ------ Representation of CodeGen for Google's TPU ----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen targeting Google's TPU. Now open
// language about TPU is TensorFlow's XLA IR and Julia, the latter maybe easier
// to implement.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_TPU_CG_H
#define PHAETON_TARGET_TPU_CG_H

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"

namespace phaeton {

/// TPUCG - This class defines the entrance for CG of Google TPU.
class TPUCG : public DirectCodeGen {};

} // end namespace phaeton

#endif // PHAETON_TARGET_TPU_CG_H
