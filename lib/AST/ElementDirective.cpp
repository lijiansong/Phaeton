//===--- ElemDirect.cpp - Interface for elements directive ----------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements interfaces of the elements directive class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"
#include "ph/AST/Stmt.h"

using namespace phaeton;

void ElemDirect::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str();

  switch (getPOSSpecifier()) {
  case POS_First:
    std::cout << " first";
    break;
  case POS_Last:
    std::cout << " last";
    break;
  default:
    assert(0 && "internal error: invalid position specifier");
  }
  std::cout << "\n";

  I->dump(indent + str.length() + 1);
  Identifiers->dump(indent + str.length() + 1);
  FORMAT_AST_INDENT(indent + 1);
  std::cout << ")\n";
}

void ElemDirect::_delete() const {
  I->_delete();
  delete I;

  Identifiers->_delete();
  delete Identifiers;
}

void ElemDirect::visit(ASTVisitor *v) const { v->visitElemDirect(this); }
