//===--- Device.h - Abstraction for different accelerators ------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Device interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_VIEWACC_H
#define PHAETON_VIEWACC_H

#pragma once

namespace phaeton {

/// DeviceType - Target device specifier.
enum DeviceType {
  DEVICE_TYPE_CPU,
  DEVICE_TYPE_GPU,
  DEVICE_TYPE_CAMBRICON_MLU,
  DEVICE_TYPE_HUAWEI_NPU,
  DEVICE_TYPE_GOOGLE_TPU,

  DEVICE_TYPE_DEFAULT,
  DEVICE_TYPE_COUNT
};

class OnChipSPMs {};

class OffChipDDRs {};

class ComputationCores {};

} // end namespace phaeton

#endif // PHAETON_AST_STMT_H
