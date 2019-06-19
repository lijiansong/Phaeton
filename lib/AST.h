#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>
#include <cassert>
#include <map>

enum NodeType {
  NT_Program,

  NT_DeclList,
  NT_StmtList,
  NT_ExprList,

  /* declarations: */
  NT_VarDecl,
  NT_TypeDecl,

  /* statement: */
  NT_Stmt,

  /* expressions: */
  NT_TensorExpr,
  NT_DotExpr,
  NT_Identifier, 
  NT_Integer,
  NT_BrackExpr,
  NT_ParenExpr,

  NT_DECLTYPE_COUNT
};

class Node {
private:
  NodeType NdType;

protected:
  Node(NodeType nt) : NdType(nt) {}
  virtual ~Node() {}

public:
  NodeType getNodeType() const { return NdType; };

  virtual void _delete() const = 0;

  virtual void dump(unsigned indent = 0) const = 0;
};

template <typename T, NodeType nt>
class NodeList : public Node {
private:
  std::vector<T *> elements;

public:
  NodeList() : Node(nt) {}
  NodeList(T *);
  virtual ~NodeList() {}

  void append(T *t) { elements.push_back(t); }
 
  int size() const { return elements.size(); } 
  
  T * operator[](int i) const { return elements.at(i); }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const NodeList *create() {
    return new NodeList();
  }

  static const NodeList *create(T *t) {
    return new NodeList(t);
  }

  static NodeList *append(NodeList *l, T *t) {
    l->append(t);
    return l;
  }
};

class Expr : public Node {
protected:
  Expr(NodeType nt) : Node(nt) {}

public:
  virtual ~Expr() {}
};


class Factor : public Expr {
protected:
  Factor(NodeType nt) : Expr(nt) {}

public:
  virtual ~Factor() {}
};

class BinaryExpr : public Expr {
private:
  const Expr *LeftExpr;
  const Factor *RightFactor;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Factor *right)
    : Expr(NT_TensorExpr), LeftExpr(left), RightFactor(right) {
    assert(nt == NT_TensorExpr || nt == NT_DotExpr);
  }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left, const Factor *right) {
    return new BinaryExpr(nt, left, right);
  }
};


class Identifier : public Factor {
private:
  const std::string Name;

public:
  Identifier(const std::string name)
    : Factor(NT_Identifier), Name(name) {}

  const std::string &getName() const { return Name; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Identifier *create(const std::string &name) {
    return new Identifier(name);
  }
};

class Integer : public Factor {
private:
  int Value;

public:
  Integer(int value)
    : Factor(NT_Integer), Value(value) {}

  int getValue() const { return Value; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Integer *create(int value) {
    return new Integer(value);
  }
};


using ExprList = NodeList<const Expr, NT_ExprList>;

class BrackExpr : public Factor {
private:
  const ExprList *Exprs;

public:
  BrackExpr(const ExprList *exprs)
    : Factor(NT_BrackExpr), Exprs(exprs) {}

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }
};


class ParenExpr : public Factor {
private:
  const Expr *NestedExpr;

public:
  ParenExpr(const Expr *expr)
    : Factor(NT_ParenExpr), NestedExpr(expr) {}

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }
};


class Stmt : public Node {
private:
  const Identifier *Id;
  const Expr *RightExpr;

public:
  Stmt(const Identifier *id, const Expr *expr)
    : Node(NT_Stmt), Id(id), RightExpr(expr) {}

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }
};


using StmtList = NodeList<const Stmt, NT_StmtList>;

class Decl : public Node {
private:
  const Identifier *Id;
  const Expr *TypeExpr;

public:
  Decl(NodeType nt, const Identifier *id, const Expr *expr) 
    : Node(nt), Id(id), TypeExpr(expr) {
    assert(nt == NT_VarDecl || nt == NT_TypeDecl);
  }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id, const Expr *expr) {
    return new Decl(nt, id, expr);
  }
};

using DeclList = NodeList<const Decl, NT_DeclList>;


class Program : public Node {
private:
  const DeclList *Decls;
  const StmtList *Stmts;

public:
  Program(const DeclList *decls, const StmtList *stmts)
    : Node(NT_Program), Decls(decls), Stmts(stmts) {}
  
  virtual void dump(unsigned indent = 0) const final;

  virtual void _delete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts) {
    return new Program(decls, stmts);
  }

  static void destroy(const Program *p) {
    p->_delete();
    delete p;
  }
};

#define NODE_PTR  void *

NODE_PTR createTensorExpr(const NODE_PTR left, const NODE_PTR right);
NODE_PTR createDotExpr(const NODE_PTR left, const NODE_PTR right);
NODE_PTR createIdentifier(const char *name);
NODE_PTR createInteger(int value);
NODE_PTR createBrackExpr(const NODE_PTR list);
NODE_PTR createParenExpr(const NODE_PTR expr);
NODE_PTR createStmt(const NODE_PTR id, const NODE_PTR expr);
NODE_PTR createVarDecl(const NODE_PTR id, const NODE_PTR expr);
NODE_PTR createTypeDecl(const NODE_PTR id, const NODE_PTR expr);
NODE_PTR createProgram(const NODE_PTR decls, const NODE_PTR stmts);
NODE_PTR createDeclList(const NODE_PTR d);
NODE_PTR appendDeclList(NODE_PTR list, const NODE_PTR d);
NODE_PTR createStmtList(const NODE_PTR s);
NODE_PTR appendStmtList(NODE_PTR list, const NODE_PTR s);
NODE_PTR createExprList();
NODE_PTR appendExprList(NODE_PTR list, const NODE_PTR e);

void dumpNode(const NODE_PTR n);
void dumpAST(const NODE_PTR p);
void destroyAST(const NODE_PTR p);

#endif /* __AST_H__ */
