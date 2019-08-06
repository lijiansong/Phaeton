//===--- ASTNode.cpp - Union class for AST nodes --------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements the ASTNode class.
//
//===----------------------------------------------------------------------===//

#include "ph/AST/ASTNode.h"
#include "ph/AST/ASTUtils.h"
#include "ph/AST/ASTVisitor.h"

using namespace phaeton;

std::map<ASTNode::NodeType, std::string> ASTNode::NodeLabel = {
    {NODETYPE_Program, "Program"},

    {NODETYPE_DeclList, "DeclList"},
    {NODETYPE_StmtList, "StmtList"},
    {NODETYPE_ExprList, "ExprList"},

    {NODETYPE_VarDecl, "VarDecl"},
    {NODETYPE_TypeDecl, "TypeDecl"},

    {NODETYPE_ElemDirect, "ElemDirect"},

    {NODETYPE_Stmt, "Stmt"},

    {NODETYPE_TranspositionExpr, "TranspositionExpr"},
    {NODETYPE_ContractionExpr, "ContractionExpr"},
    {NODETYPE_AddExpr, "AddExpr"},
    {NODETYPE_SubExpr, "SubExpr"},
    {NODETYPE_MulExpr, "MulExpr"},
    {NODETYPE_DivExpr, "DivExpr"},
    {NODETYPE_ProductExpr, "ProductExpr"},
    {NODETYPE_Identifier, "Identifier"},
    {NODETYPE_Integer, "Integer"},
    {NODETYPE_BrackExpr, "BrackExpr"},
    {NODETYPE_ParenExpr, "ParenExpr"},
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

  FORMAT_AST_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  for (int i = 0; i < size(); i++)
    elements[i]->dump(indent + str.length() + 1);

  FORMAT_AST_INDENT(indent + 1)
  std::cout << ")\n";
}

template <typename T, ASTNode::NodeType nt, typename Derived>
void ASTNodeList<T, nt, Derived>::_delete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->_delete();
    delete elements[i];
  }
}

template class phaeton::ASTNodeList<const Decl, ASTNode::NODETYPE_DeclList, DeclList>;

void DeclList::visit(ASTVisitor *v) const { v->visitDeclList(this); }

template class phaeton::ASTNodeList<const Stmt, ASTNode::NODETYPE_StmtList, StmtList>;

void StmtList::visit(ASTVisitor *v) const { v->visitStmtList(this); }

template class phaeton::ASTNodeList<const Expr, ASTNode::NODETYPE_ExprList, ExprList>;

void ExprList::visit(ASTVisitor *v) const { v->visitExprList(this); }
