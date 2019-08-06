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

void Program::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Decls->dump(indent + str.length() + 1);
  Elem->dump(indent + str.length() + 1);
  Stmts->dump(indent + str.length() + 1);
  FORMAT_AST_INDENT(indent + 1)
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

void Program::visit(ASTVisitor *v) const { v->visitProgram(this); }
