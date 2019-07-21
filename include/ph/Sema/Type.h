//===--- Type.h - Classes for representing base tensor type -----*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the compiler base data type Tensor.
//
//===----------------------------------------------------------------------===//

#ifndef __TYPE_H__
#define __TYPE_H__

#include <string>
#include <vector>

class TensorType {
private:
  std::vector<int> Dims;
  // TODO: add data type
  // TODO: add memory hierarchy

public:
  enum AddrSpaceType {
    AS_Global,
    AS_Shared,
    AS_Neural,
    AS_Synapse,
    AS_Constant,

    AS_Default
  };

  TensorType() {}

  TensorType(const std::vector<int> &dims) : Dims(dims) {}

  void addDim(int dim) { Dims.push_back(dim); }

  int getRank() const { return Dims.size(); }

  int getDim(unsigned i) const { return Dims.at(i); }

  bool operator==(const TensorType &rhs) const;

  bool operator!=(const TensorType &rhs) const { return !(*this == rhs); }

  bool equals(const std::vector<int> &dims) const {
    return (*this == TensorType(dims));
  }

  const std::string getDimString() const;

  const std::vector<int> &getDims() const { return Dims; }
};

#endif // __TYPE_H__
