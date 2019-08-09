//===--- ElementDirective.cpp - Interface for elements directive ----------===//
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
#include "ph/Support/ErrorHandling.h"

using namespace phaeton;

void ElementDirective::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str();

  switch (getPOSSpecifier()) {
  case POS_First:
    std::cout << " first";
    break;
  case POS_Last:
    std::cout << " last";
    break;
  default:
    ph_unreachable(INTERNAL_ERROR "invalid position specifier");
  }
  std::cout << '\n';

  Int->dump(Indent + Str.length() + 1);
  Identifiers->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1);
  std::cout << ")\n";
}

void ElementDirective::_delete() const {
  Int->_delete();
  delete Int;

  Identifiers->_delete();
  delete Identifiers;
}

void ElementDirective::visit(ASTVisitor *Visitor) const {
  Visitor->visitElementDirective(this);
}
