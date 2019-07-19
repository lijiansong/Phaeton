//===--- Stmt.cpp - Interface for statements AST node --------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements interfaces of the Stmt class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/Stmt.h"
#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"

void Stmt::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Id->dump(indent + str.length() + 1);
  RightExpr->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Stmt::_delete() const {
  Id->_delete();
  delete Id;

  RightExpr->_delete();
  delete RightExpr;
}

void Stmt::visit(ASTVisitor *v) const { v->visitStmt(this); }
