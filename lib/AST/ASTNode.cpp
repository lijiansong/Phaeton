#include "tir/AST/ASTNode.h"
#include "tir/AST/Decl.h"
#include "tir/AST/Expr.h"
#include "tir/AST/Stmt.h"
#include "tir/AST/ASTVisitor.h"
#include "tir/AST/ASTUtils.h"

std::map<ASTNode::NodeType, std::string> ASTNode::NodeLabel = {
    {ASTNode::NT_Program, "Program"},

    {ASTNode::NT_DeclList, "DeclList"},
    {ASTNode::NT_StmtList, "StmtList"},
    {ASTNode::NT_ExprList, "ExprList"},

    {ASTNode::NT_VarDecl, "VarDecl"},
    {ASTNode::NT_TypeDecl, "TypeDecl"},

    {ASTNode::NT_Stmt, "Stmt"},

    {ASTNode::NT_ContractionExpr, "ContractionExpr"},
    {ASTNode::NT_AddExpr, "AddExpr"},
    {ASTNode::NT_SubExpr, "SubExpr"},
    {ASTNode::NT_MulExpr, "MulExpr"},
    {ASTNode::NT_DivExpr, "DivExpr"},
    {ASTNode::NT_ProductExpr, "ProductExpr"},
    {ASTNode::NT_Identifier, "Identifier"},
    {ASTNode::NT_Integer, "Integer"},
    {ASTNode::NT_BrackExpr, "BrackExpr"},
    {ASTNode::NT_ParenExpr, "ParenExpr"},
};

template <typename T, ASTNode::NodeType nt, typename Derived>
ASTNodeList<T, nt, Derived>::ASTNodeList(T *t) : ASTNodeList() {
  append(t);
}

template <typename T, ASTNode::NodeType nt, typename Derived>
void ASTNodeList<T, nt, Derived>::dump(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  FORMAT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  for (int i = 0; i < size(); i++)
    elements[i]->dump(indent + str.length() + 1);

  FORMAT_INDENT(indent + 1)
  std::cout << ")\n";
}

template <typename T, ASTNode::NodeType nt, typename Derived>
void ASTNodeList<T, nt, Derived>::_delete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->_delete();
    delete elements[i];
  }
}

template class ASTNodeList<Decl, ASTNode::NT_DeclList, DeclList>;

void DeclList::visit(ASTVisitor *v) const { v->visitDeclList(this); }

template class ASTNodeList<Stmt, ASTNode::NT_StmtList, StmtList>;

void StmtList::visit(ASTVisitor *v) const { v->visitStmtList(this); }

template class ASTNodeList<Expr, ASTNode::NT_ExprList, ExprList>;

void ExprList::visit(ASTVisitor *v) const { v->visitExprList(this); }
