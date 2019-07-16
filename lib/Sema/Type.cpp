#include "ph/Sema/Type.h"

bool TensorType::operator==(const TensorType &rhs) const {
  if (getRank() != rhs.getRank())
    return false;

  for (unsigned i = 0; i < getRank(); i++) {
    if (getDim(i) != rhs.getDim(i))
      return false;
  }

  return true;
}

const std::string TensorType::getDimString() const {
  std::string result = "";
  for (unsigned i = 0; i < getRank(); i++) {
    result += std::to_string(getDim(i));
    if (i != (getRank() - 1))
      result += ", ";
  }
  return result;
}
