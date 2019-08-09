//===--- Type.h - Classes for representing base tensor type -----*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the compiler base data type Tensor.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_SEMA_TYPE_H
#define PHAETON_SEMA_TYPE_H

#include <string>
#include <vector>

namespace phaeton {

class TensorType {
public:
  /// AddrSpaceType - Memory hierarchy specifier.
  enum AddrSpaceType {
    ADDR_SPACE_Global,
    ADDR_SPACE_Shared,
    ADDR_SPACE_Neural,
    ADDR_SPACE_Synapse,
    ADDR_SPACE_Constant,

    ADDR_SPACE_Default,
    ADDR_SPACE_AddrSpaceType_COUNT
  };

public:
  TensorType() {}
  TensorType(const std::vector<int> &Dims) : Dims(Dims) {}

  void addDim(int Dim) { Dims.push_back(Dim); }

  int getRank() const { return Dims.size(); }

  int getDim(unsigned I) const { return Dims.at(I); }

  bool operator==(const TensorType &RHS) const;
  bool operator!=(const TensorType &RHS) const { return !(*this == RHS); }

  bool equals(const std::vector<int> &Dims) const {
    return (*this == TensorType(Dims));
  }

  const std::string getDimString() const;

  const std::vector<int> &getDims() const { return Dims; }

private:
  std::vector<int> Dims;
  // TODO: add data type
  // TODO: add memory hierarchy
};

} // end namespace phaeton

#endif // PHAETON_SEMA_TYPE_H
