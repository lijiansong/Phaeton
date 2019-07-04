#include <cassert>
#include <iostream>
#include <string>

#include "tir/AST/AST.h"

#undef FORMAT_INDENT
#define FORMAT_INDENT(indent)                                                  \
  {                                                                            \
    std::cout.width((indent));                                                 \
    std::cout << std::left << "";                                              \
    std::cout.unsetf(std::ios::adjustfield);                                   \
  }

static std::map<NodeType, std::string> NodeLabel = {
    {NT_Program, "Program"},

    {NT_DeclList, "DeclList"},
    {NT_StmtList, "StmtList"},
    {NT_ExprList, "ExprList"},

    {NT_VarDecl, "VarDecl"},
    {NT_TypeDecl, "TypeDecl"},

    {NT_Stmt, "Stmt"},

    {NT_TensorExpr, "TensorExpr"},
    {NT_DotExpr, "DotExpr"},
    {NT_Identifier, "Identifier"},
    {NT_Integer, "Integer"},
    {NT_BrackExpr, "BrackExpr"},
    {NT_BrackExpr, "ParenExpr"},
};

void Expr::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Expr' should not be visited");
}

void Factor::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Factor' should not be visited");
}

void Identifier::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << " \"" << getName() << "\")\n";
}

void Identifier::visit(ASTVisitor *v) const { v->visitIdentifier(this); }

void Integer::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << " \"" << getValue() << "\")\n";
}

void Integer::visit(ASTVisitor *v) const { v->visitInteger(this); }

void BinaryExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";
  LeftExpr->dump(indent + str.length() + 1);
  RightExpr->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::_delete() const {
  LeftExpr->_delete();
  delete LeftExpr;

  RightExpr->_delete();
  delete RightExpr;
}

void BinaryExpr::visit(ASTVisitor *v) const { v->visitBinaryExpr(this); }

template <typename T, NodeType nt, typename Derived>
NodeList<T, nt, Derived>::NodeList(T *t) : NodeList() {
  append(t);
}

template <typename T, NodeType nt, typename Derived>
void NodeList<T, nt, Derived>::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";

  for (int i = 0; i < size(); i++)
    elements[i]->dump(indent + str.length() + 1);

  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

template <typename T, NodeType nt, typename Derived>
void NodeList<T, nt, Derived>::_delete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->_delete();
    delete elements[i];
  }
}

template class NodeList<Decl, NT_DeclList, DeclList>;

void DeclList::visit(ASTVisitor *v) const { v->visitDeclList(this); }

template class NodeList<Stmt, NT_StmtList, StmtList>;

void StmtList::visit(ASTVisitor *v) const { v->visitStmtList(this); }

template class NodeList<Expr, NT_ExprList, ExprList>;

void ExprList::visit(ASTVisitor *v) const { v->visitExprList(this); }

void Decl::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str;
  if (getNodeType() == NT_VarDecl) {
    if (getInOutSpecifier() & IO_Input)
      std::cout << " in";
    if (getInOutSpecifier() & IO_Output)
      std::cout << " out";
  }
  std::cout << '\n';
  Id->dump(indent + str.length() + 1);
  TypeExpr->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Decl::_delete() const {
  Id->_delete();
  delete Id;

  TypeExpr->_delete();
  delete TypeExpr;
}

void Decl::visit(ASTVisitor *v) const { v->visitDecl(this); }

void Program::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";
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

void BrackExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Exprs->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BrackExpr::_delete() const {
  Exprs->_delete();
  delete Exprs;
}

void BrackExpr::visit(ASTVisitor *v) const { v->visitBrackExpr(this); }

void ParenExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";
  NestedExpr->dump(indent + str.length() + 1);
  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ParenExpr::_delete() const {
  NestedExpr->_delete();
  delete NestedExpr;
}

void ParenExpr::visit(ASTVisitor *v) const { v->visitParenExpr(this); }

void Stmt::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  FORMAT_INDENT(indent)
  std::cout << "(" << str << "\n";
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

template <typename T, NodeType nt, typename Derived>
void ASTVisitor::visitNodeList(const NodeList<T, nt, Derived> *list) {
  for (const auto &i : *list)
    i->visit(this);
}

#define DEF_VISIT_LIST(Derived)                                                \
  void ASTVisitor::visit##Derived(const Derived *list) { visitNodeList(list); }

DEF_VISIT_LIST(DeclList)
DEF_VISIT_LIST(StmtList)
DEF_VISIT_LIST(ExprList)

void ASTVisitor::visitProgram(const Program *p) {
  p->getDecls()->visit(this);
  p->getStmts()->visit(this);
}

void ASTVisitor::visitExpr(const Expr *e) { e->visit(this); }

void ASTVisitor::visitFactor(const Factor *f) { f->visit(this); }

void ASTVisitor::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);
}
