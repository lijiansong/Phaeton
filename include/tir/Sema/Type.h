#ifndef __TYPE_H__
#define __TYPE_H__

#include <string>
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

  const std::string getDimString() const {
    std::string result = "";
    for (unsigned i = 0; i < getRank(); i++) {
      result += std::to_string(getDim(i));
      if (i != (getRank() - 1))
        result += ", ";
    }
    return result;
  }
};

#endif /* !__TYPE_H__ */
