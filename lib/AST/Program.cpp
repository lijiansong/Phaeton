#include "tir/AST/Program.h"
#include "tir/AST/ASTUtils.h"
#include "tir/AST/ASTVisitor.h"

void Program::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Decls->dump(indent + str.length() + 1);
  Stmts->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Program::_delete() const {
  Decls->_delete();
  delete Decls;

  Stmts->_delete();
  delete Stmts;
}

void Program::visit(ASTVisitor *v) const { v->visitProgram(this); }
