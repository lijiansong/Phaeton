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
#include "ph/Support/ErrorHandling.h"

using namespace phaeton;

void Expression::visit(ASTVisitor *Visitor) const {
  ph_unreachable(INTERNAL_ERROR
                 "base class 'Expression' should not be visited");
}

void Factor::visit(ASTVisitor *Visitor) const {
  ph_unreachable(INTERNAL_ERROR "base class 'Factor' should not be visited");
}

void Identifier::visit(ASTVisitor *Visitor) const {
  Visitor->visitIdentifier(this);
}

void Identifier::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << " \"" << getName() << "\")\n";
}

void Integer::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << " \"" << getValue() << "\")\n";
}

void Integer::visit(ASTVisitor *Visitor) const { Visitor->visitInteger(this); }

void BinaryExpr::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";
  LeftExpr->dump(Indent + Str.length() + 1);
  RightExpr->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::_delete() const {
  LeftExpr->_delete();
  delete LeftExpr;

  RightExpr->_delete();
  delete RightExpr;
}

void BinaryExpr::visit(ASTVisitor *Visitor) const {
  Visitor->visitBinaryExpr(this);
}

void BrackExpr::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";
  Exprs->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void BrackExpr::_delete() const {
  Exprs->_delete();
  delete Exprs;
}

void BrackExpr::visit(ASTVisitor *Visitor) const {
  Visitor->visitBrackExpr(this);
}

void ParenExpr::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";
  NestedExpr->dump(Indent + Str.length() + 1);
  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

void ParenExpr::_delete() const {
  NestedExpr->_delete();
  delete NestedExpr;
}

void ParenExpr::visit(ASTVisitor *Visitor) const {
  Visitor->visitParenExpr(this);
}
