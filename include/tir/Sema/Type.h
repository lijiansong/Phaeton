//===--- Type.h - Classes for representing base tensor type -----*- C++ -*-===//
//
//  This file defines the compile base data type Tensor.
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

public:
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
};

#endif // __TYPE_H__
