//==--- CCECG.h ------ Representation of CodeGen for Huawei Davinci CCE ----==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for target language Huawei
// Davinci's CCE.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_CCE_CG_H
#define PHAETON_TARGET_CCE_CG_H

#include "ph/CodeGen/GraphCG.h"
#include "ph/CodeGen/NaiveCG.h"

namespace phaeton {

/// CCECG - This class defines entrance for the CG of Huawei CCE.
class CCECG : public NaiveCodeGen {};

} // end namespace phaeton

#endif // PHAETON_TARGET_CCE_CG_H
