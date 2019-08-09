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

using namespace phaeton;

void Stmt::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";
  Id->dump(Indent + Str.length() + 1);
  RightExpr->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void Stmt::_delete() const {
  Id->_delete();
  delete Id;

  RightExpr->_delete();
  delete RightExpr;
}

void Stmt::visit(ASTVisitor *Visitor) const { Visitor->visitStmt(this); }
