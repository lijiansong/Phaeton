//===--- ViewAcc.h - Abstraction for DNN accelerators -----------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the ViewAcc interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_VIEWACC_H
#define PHAETON_VIEWACC_H

#pragma once

namespace phaeton {

/// MemHierarchyType - Memory hierarchy specifier.
enum MemHierarchyType {
  ADDR_SPACE_Global,
  ADDR_SPACE_Shared,
  ADDR_SPACE_Neural,
  ADDR_SPACE_Synapse,
  ADDR_SPACE_Constant,

  ADDR_SPACE_Default,
  ADDR_SPACE_AddrSpaceType_COUNT
};

class OnChipSPMs {};

class OffChipDDRs {};

class ComputationCores {};

} // end namespace phaeton

#endif // PHAETON_AST_STMT_H
