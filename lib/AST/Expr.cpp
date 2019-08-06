//===--- Expr.cpp - Union class for expressions ---------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements interface of expressions.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/Expr.h"
#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"

using namespace phaeton;

void Expr::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Expr' should not be visited");
}

void Factor::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Factor' should not be visited");
}

void Identifier::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getName() << "\")\n";
}

void Identifier::visit(ASTVisitor *v) const { v->visitIdentifier(this); }

void Integer::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getValue() << "\")\n";
}

void Integer::visit(ASTVisitor *v) const { v->visitInteger(this); }

void BinaryExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  LeftExpr->dump(indent + str.length() + 1);
  RightExpr->dump(indent + str.length() + 1);
  FORMAT_AST_INDENT(indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::_delete() const {
  LeftExpr->_delete();
  delete LeftExpr;

  RightExpr->_delete();
  delete RightExpr;
}

void BinaryExpr::visit(ASTVisitor *v) const { v->visitBinaryExpr(this); }

void BrackExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Exprs->dump(indent + str.length() + 1);
  FORMAT_AST_INDENT(indent + 1)
  std::cout << ")\n";
}

void BrackExpr::_delete() const {
  Exprs->_delete();
  delete Exprs;
}

void BrackExpr::visit(ASTVisitor *v) const { v->visitBrackExpr(this); }

void ParenExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  NestedExpr->dump(indent + str.length() + 1);
  FORMAT_AST_INDENT(indent + 1)
  std::cout << ")\n";
}

void ParenExpr::_delete() const {
  NestedExpr->_delete();
  delete NestedExpr;
}

void ParenExpr::visit(ASTVisitor *v) const { v->visitParenExpr(this); }
