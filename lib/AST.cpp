#include <string>
#include <iostream>

#include "AST.h"

#define EMIT_INDENT(indent) { std::cout.width((indent)); \
                              std::cout << std::left << ""; \
                              std::cout.unsetf(std::ios::adjustfield); }

static std::map<NodeType, std::string> NodeLabel = {
  { NT_Program, "Program" },

  { NT_DeclList, "DeclList" },
  { NT_StmtList, "StmtList" },
  { NT_ExprList, "ExprList" },

  { NT_VarDecl, "VarDecl" },
  { NT_TypeDecl, "TypeDecl" },

  { NT_Stmt, "Stmt" },

  { NT_TensorExpr, "TensorExpr" },
  { NT_DotExpr, "DotExpr" },
  { NT_Identifier, "Identifier" },
  { NT_Integer, "Integer" },
  { NT_BrackExpr, "BrackExpr" },
  { NT_BrackExpr, "ParenExpr" },
};

void Identifier::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << " \"" << getName() << "\")\n";
}

void Integer::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << " \"" << getValue() << "\")\n";
}

void BinaryExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  LeftExpr->dump(indent + str.length() + 1);
  RightFactor->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::_delete() const {
  LeftExpr->_delete();
  delete LeftExpr;

  RightFactor->_delete();
  delete RightFactor;
}

template<typename T, NodeType nt>
NodeList<T, nt>::NodeList(T *t)
  : NodeList() {
  append(t);
}

template<typename T, NodeType nt>
void NodeList<T, nt>::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
 
  for (int i = 0; i < size(); i++)
    elements[i]->dump(indent + str.length() + 1); 

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

template<typename T, NodeType nt>
void NodeList<T, nt>::_delete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->_delete();
    delete elements[i];
  }
}

template class NodeList<const Decl, NT_DeclList>;
template class NodeList<const Stmt, NT_StmtList>;
template class NodeList<const Expr, NT_ExprList>;

void Decl::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Id->dump(indent + str.length() + 1);
  TypeExpr->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Decl::_delete() const {
  Id->_delete();
  delete Id;

  TypeExpr->_delete();
  delete TypeExpr;
}

void Program::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Decls->dump(indent + str.length() + 1);
  Stmts->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Program::_delete() const {
  Decls->_delete();
  delete Decls;

  Stmts->_delete();
  delete Stmts;
}

void BrackExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Exprs->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BrackExpr::_delete() const {
  Exprs->_delete();
  delete Exprs;
}

void ParenExpr::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  NestedExpr->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ParenExpr::_delete() const {
  NestedExpr->_delete();
  delete NestedExpr;
}

void Stmt::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Id->dump(indent + str.length() + 1);
  RightExpr->dump(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Stmt::_delete() const {
  Id->_delete();
  delete Id;

  RightExpr->_delete();
  delete RightExpr;
}

static NODE_PTR createBinaryExpr(NodeType nt, const NODE_PTR left, const NODE_PTR right) {
  const Expr *left_cast = dynamic_cast<const Expr *>((const Node *)left);
  assert(left_cast);

  const Factor *right_cast = dynamic_cast<const Factor *>((const Node *)right);
  assert(right_cast);

  return (NODE_PTR) BinaryExpr::create(nt, left_cast, right_cast);
}

NODE_PTR createTensorExpr(const NODE_PTR left, const NODE_PTR right) {
  return createBinaryExpr(NT_TensorExpr, left, right);
}

NODE_PTR createDotExpr(const NODE_PTR left, const NODE_PTR right) {
  return createBinaryExpr(NT_DotExpr, left, right);
}

NODE_PTR createIdentifier(const char *name) {
 return (NODE_PTR) Identifier::create(std::string(name)); 
}

NODE_PTR createInteger(int value) {
 return (NODE_PTR) Integer::create(value); 
}

NODE_PTR createBrackExpr(const NODE_PTR list) {
  const ExprList *list_cast = dynamic_cast<const ExprList *>((const Node *)list);
  assert(list_cast);

  return (NODE_PTR) BrackExpr::create(list_cast);
}

NODE_PTR createParenExpr(const NODE_PTR expr) {
  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) ParenExpr::create(expr_cast);
}

NODE_PTR createStmt(const NODE_PTR id, const NODE_PTR expr) {
  const Identifier *id_cast = dynamic_cast<const Identifier *>((const Node *)id);
  assert(id_cast);

  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) Stmt::create(id_cast, expr_cast);
}

static NODE_PTR createDecl(NodeType nt, const NODE_PTR id, const NODE_PTR expr) {
  const Identifier *id_cast = dynamic_cast<const Identifier *>((const Node *)id);
  assert(id_cast);

  const Expr *expr_cast = dynamic_cast<const Expr *>((const Node *)expr);
  assert(expr_cast);

  return (NODE_PTR) Decl::create(nt, id_cast, expr_cast);
}

NODE_PTR createVarDecl(const NODE_PTR id, const NODE_PTR expr) {
  return createDecl(NT_VarDecl, id, expr);
}

NODE_PTR createTypeDecl(const NODE_PTR id, const NODE_PTR expr) {
  return createDecl(NT_TypeDecl, id, expr);
}

NODE_PTR createProgram(const NODE_PTR decls, const NODE_PTR stmts) {
  const DeclList *decls_cast = dynamic_cast<const DeclList *>((const Node *)decls);
  assert(decls_cast);

  const StmtList *stmts_cast = dynamic_cast<const StmtList *>((const Node *)stmts);
  assert(stmts_cast);

  return (NODE_PTR) Program::create(decls_cast, stmts_cast);
}

NODE_PTR createDeclList(const NODE_PTR d) {
  const Decl *d_cast = dynamic_cast<const Decl *>((const Node *)d);
  assert(d_cast);

  return (NODE_PTR) DeclList::create(d_cast);
}

NODE_PTR appendDeclList(NODE_PTR list, const NODE_PTR d) {
  DeclList *list_cast = dynamic_cast<DeclList *>((Node *)list);
  assert(list_cast);

  const Decl *d_cast = dynamic_cast<const Decl *>((const Node *)d);
  assert(d_cast);

  return (NODE_PTR) DeclList::append(list_cast, d_cast);
}

NODE_PTR createStmtList(const NODE_PTR s) {
  const Stmt *s_cast = dynamic_cast<const Stmt *>((const Node *)s);
  assert(s_cast);

  return (NODE_PTR) StmtList::create(s_cast);
}

NODE_PTR appendStmtList(NODE_PTR list, const NODE_PTR s) {
  StmtList *list_cast = dynamic_cast<StmtList *>((Node *)list);
  assert(list_cast);

  const Stmt *s_cast = dynamic_cast<const Stmt *>((const Node *)s);
  assert(s_cast);

  return (NODE_PTR) StmtList::append(list_cast, s_cast);
}

NODE_PTR createExprList() {
  return (NODE_PTR) ExprList::create();
}

NODE_PTR appendExprList(NODE_PTR list, const NODE_PTR e) {
  ExprList *list_cast = dynamic_cast<ExprList *>((Node *)list);
  assert(list_cast);

  const Expr *e_cast = dynamic_cast<const Expr *>((const Node *)e);
  assert(e_cast);

  return (NODE_PTR) ExprList::append(list_cast, e_cast);
}

void dumpNode(const NODE_PTR n) {
  ((const Node *)n)->dump();
}

void dumpAST(const NODE_PTR p) {
  const Program *p_cast = dynamic_cast<const Program *>((const Node *)p);
  assert(p_cast);
  
  p_cast->dump();
}

void destroyAST(const NODE_PTR p) {
  const Program *p_cast = dynamic_cast<const Program *>((const Node *)p);
  assert(p_cast);

  Program::destroy(p_cast);
}
