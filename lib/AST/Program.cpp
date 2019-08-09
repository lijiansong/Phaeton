//===--- Program.cpp - Interface for Program AST node --------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements interfaces of the Program class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/Program.h"
#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"

using namespace phaeton;

void Program::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";
  Decls->dump(Indent + Str.length() + 1);
  Elem->dump(Indent + Str.length() + 1);
  Stmts->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void Program::_delete() const {
  Decls->_delete();
  delete Decls;

  Stmts->_delete();
  delete Stmts;

  if (Elem) {
    Elem->_delete();
    delete Elem;
  }
}

void Program::visit(ASTVisitor *Visitor) const { Visitor->visitProgram(this); }
