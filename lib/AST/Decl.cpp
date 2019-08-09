//===--- Decl.cpp - Interface for Decleration AST node --------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements interfaces of the Decl class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/Decl.h"
#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"

using namespace phaeton;

void Decl::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str();

  if (getASTNodeKind() == AST_NODE_KIND_VarDecl) {
    if (getInOutSpecifier() & IO_SPEC_Input)
      std::cout << " input";
    if (getInOutSpecifier() & IO_SPEC_Output)
      std::cout << " output";
  }
  std::cout << "\n";

  Id->dump(Indent + Str.length() + 1);
  TypeExpr->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void Decl::_delete() const {
  Id->_delete();
  delete Id;

  TypeExpr->_delete();
  delete TypeExpr;
}

void Decl::visit(ASTVisitor *Visitor) const { Visitor->visitDecl(this); }
