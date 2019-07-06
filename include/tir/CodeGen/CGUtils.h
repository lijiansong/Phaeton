//===--- CGUtils.h ------ CodeGen Helper Functions --------------*- C++ -*-===//
//
/// \file
/// \brief This file provides some common utility functions for code generation.
///
//===----------------------------------------------------------------------===//

#ifndef __CG_UTILS_H__
#define __CG_UTILS_H__

#include <sstream>
#include <string>

#include "tir/AST/AST.h"

template <typename Derived> class Comparable {
public:
  virtual bool operator==(const Derived &rhs) const = 0;
  virtual bool operator!=(const Derived &rhs) const { return !(*this == rhs); }
  virtual bool operator<(const Derived &rhs) const = 0;
  virtual bool operator<=(const Derived &rhs) const {
    return (*this < rhs) || (*this == rhs);
  }
  virtual bool operator>=(const Derived &rhs) const { return !(*this < rhs); }
  virtual bool operator>(const Derived &rhs) const {
    return (*this >= rhs) && (*this != rhs);
  }
};

template <typename Derived>
class GraphComponentID : public Comparable<Derived> {
private:
  const std::string Label;

  const ASTNode *AST; /* AST node */

public:
  GraphComponentID(const std::string label = "", const ASTNode *ast = nullptr)
      : Label(label), AST(ast) {}

  const std::string &getLabel() const { return Label; }
  const ASTNode *getAST() const { return AST; }

  virtual const std::string str() const = 0;
};

class StringID : public GraphComponentID<StringID> {
private:
  const std::string ID;

public:
  StringID(const std::string &id, const std::string label = "",
           const ASTNode *ast = nullptr)
      : GraphComponentID<StringID>(label, ast), ID(id) {}

  StringID(const StringID &rhs)
      : GraphComponentID<StringID>(rhs.getLabel(), rhs.getAST()),
        ID(rhs.str()) {}

  const std::string str() const final { return ID; }
  bool operator==(const StringID &id) const final { return ID == id.str(); }
  bool operator<(const StringID &id) const final { return ID < id.str(); }
};

class AddressID : public GraphComponentID<AddressID> {
private:
  const void *const ID;

public:
  AddressID(const void *const id, const std::string label = "",
            const ASTNode *ast = nullptr)
      : GraphComponentID<AddressID>(label, ast), ID(id) {}

  const void *const get() const { return ID; }

  AddressID(const AddressID &rhs, const std::string label = "")
      : GraphComponentID<AddressID>(rhs.getLabel(), rhs.getAST()),
        ID(rhs.get()) {}

  const std::string str() const {
    std::stringstream ss;
    ss << std::hex << ID;
    return ss.str();
  }

  bool operator==(const AddressID &id) const final { return ID == id.get(); }
  bool operator<(const AddressID &id) const final { return ID < id.get(); }
};

#endif // __CG_UTILS_H__
