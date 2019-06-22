#ifndef __TYPE_H__
#define __TYPE_H__

#include <vector>

enum AddressSpace { AS_Default };

class TensorType {
private:
  std::vector<int> Dims;

public:
  TensorType() {}
  TensorType(const std::vector<int> &dims) : Dims(dims) {}

  void addDim(int dim) { Dims.push_back(dim); }

  int getRank() const { return Dims.size(); }

  int getDim(unsigned i) const { return Dims.at(i); }

  bool operator==(const TensorType &rhs) const {
    if (getRank() != rhs.getRank())
      return false;

    for (unsigned i = 0; i < getRank(); i++) {
      if (getDim(i) != rhs.getDim(i))
        return false;
    }

    return true;
  }

  bool operator!=(const TensorType &rhs) const { return !(*this == rhs); }

  bool equals(const std::vector<int> &dims) const {
    return (*this == TensorType(dims));
  }
};

#endif /* !__TYPE_H__ */
