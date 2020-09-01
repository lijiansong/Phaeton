//===--- Type.cpp - Interfaces to base tensor type ------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements the interfaces of compiler base data type Tensor.
//
//===----------------------------------------------------------------------===//

#include "ph/Sema/Type.h"

using namespace phaeton;

bool TensorDataType::operator==(const TensorDataType &RHS) const {
  if (getRank() != RHS.getRank()) {
    return false;
  }

  for (unsigned i = 0; i < getRank(); ++i) {
    if (getDim(i) != RHS.getDim(i)) {
      return false;
    }
  }

  return true;
}

const std::string TensorDataType::getDimString() const {
  std::string Res = "";
  for (unsigned i = 0; i < getRank(); i++) {
    Res += std::to_string((long long)getDim(i));
    if (i != (getRank() - 1))
      Res += ", ";
  }
  return Res;
}
