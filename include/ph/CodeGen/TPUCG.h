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

#ifndef __TPU_CG_H__
#define __TPU_CG_H__

#include "ph/CodeGen/DirectCG.h"
#include "ph/CodeGen/GraphCG.h"

// TODO: CG for Google TPU.
class TPUCG : public DirectCodeGen {};

#endif // __TPU_CG_H__
