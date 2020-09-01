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

std::map<ASTNode::ASTNodeKind, std::string> ASTNode::NodeLabel = {
    {AST_NODE_KIND_Program, "Program"},

    {AST_NODE_KIND_DeclList, "DeclList"},
    {AST_NODE_KIND_StmtList, "StmtList"},
    {AST_NODE_KIND_ExprList, "ExprList"},

    {AST_NODE_KIND_VarDecl, "VarDecl"},
    {AST_NODE_KIND_TypeDecl, "TypeDecl"},

    {AST_NODE_KIND_ElementDirective, "ElementDirective"},

    {AST_NODE_KIND_Stmt, "Stmt"},

    {AST_NODE_KIND_TranspositionExpr, "TranspositionExpr"},
    {AST_NODE_KIND_ContractionExpr, "ContractionExpr"},
    {AST_NODE_KIND_AddExpr, "AddExpr"},
    {AST_NODE_KIND_SubExpr, "SubExpr"},
    {AST_NODE_KIND_MulExpr, "MulExpr"},
    {AST_NODE_KIND_DivExpr, "DivExpr"},
    {AST_NODE_KIND_ProductExpr, "ProductExpr"},
    {AST_NODE_KIND_Identifier, "Identifier"},
    {AST_NODE_KIND_Integer, "Integer"},
    {AST_NODE_KIND_BrackExpr, "BrackExpr"},
    {AST_NODE_KIND_ParenExpr, "ParenExpr"},
};

template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
ASTNodeList<T, NK, Derived>::ASTNodeList(T *Tpl) : ASTNodeList() {
  append(Tpl);
}

template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
void ASTNodeList<T, NK, Derived>::dump(unsigned Indent) const {
  std::string Str = NodeLabel[getASTNodeKind()];

  std::stringstream StrStream;
  StrStream << " <" << std::hex << this << ">";

  FORMAT_AST_INDENT(Indent)
  std::cout << "(" << Str << StrStream.str() << "\n";

  for (int i = 0; i < size(); i++)
    Elements[i]->dump(Indent + Str.length() + 1);

  FORMAT_AST_INDENT(Indent + 1)
  std::cout << ")\n";
}

template <typename T, ASTNode::ASTNodeKind NK, typename Derived>
void ASTNodeList<T, NK, Derived>::_delete() const {
  for (int i = 0; i < size(); i++) {
    Elements[i]->_delete();
    delete Elements[i];
  }
}

template class phaeton::ASTNodeList<const Decl, ASTNode::AST_NODE_KIND_DeclList,
                                    DeclList>;

void DeclList::visit(ASTVisitor *Visitor) const {
  Visitor->visitDeclList(this);
}

template class phaeton::ASTNodeList<const Stmt, ASTNode::AST_NODE_KIND_StmtList,
                                    StmtList>;

void StmtList::visit(ASTVisitor *Visitor) const {
  Visitor->visitStmtList(this);
}

template class phaeton::ASTNodeList<const Expression,
                                    ASTNode::AST_NODE_KIND_ExprList, ExprList>;

void ExprList::visit(ASTVisitor *Visitor) const {
  Visitor->visitExprList(this);
}
