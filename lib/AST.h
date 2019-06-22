#ifndef __AST_H__
#define __AST_H__

#include <cassert>
#include <map>
#include <string>
#include <vector>

enum NodeType {
  NT_Program,

  NT_DeclList,
  NT_StmtList,
  NT_ExprList,

  NT_VarDecl,
  NT_TypeDecl,

  NT_Stmt,

  NT_TensorExpr,
  NT_DotExpr,
  NT_Identifier,
  NT_Integer,
  NT_BrackExpr,
  NT_ParenExpr,

  NT_NODETYPE_COUNT
};

class ASTVisitor;

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

  virtual void visit(ASTVisitor *v) const = 0;
};

template <typename T, NodeType nt, typename Derived>
class NodeList : public Node {
public:
  using Container = std::vector<T *>;

private:
  Container elements;

public:
  NodeList() : Node(nt) {}
  NodeList(T *);
  virtual ~NodeList() {}

  void append(T *t) { elements.push_back(t); }

  int size() const { return elements.size(); }

  T *operator[](int i) const { return elements.at(i); }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Derived *create() { return new Derived(); }

  static Derived *create(T *t) { return new Derived(t); }

  static Derived *append(Derived *l, T *t) {
    l->append(t);
    return l;
  }

  typename Container::const_iterator begin() const { return elements.begin(); }
  typename Container::const_iterator end() const { return elements.end(); }

  virtual void visit(ASTVisitor *v) const override {}
};

class Expr : public Node {
protected:
  Expr(NodeType nt) : Node(nt) {}

public:
  virtual ~Expr() {}

  virtual void visit(ASTVisitor *v) const override;
};

class Factor : public Expr {
protected:
  Factor(NodeType nt) : Expr(nt) {}

public:
  virtual ~Factor() {}

  virtual void visit(ASTVisitor *v) const override = 0;
};

class BinaryExpr : public Expr {
private:
  const Expr *LeftExpr;
  const Factor *RightFactor;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Factor *right)
      : Expr(nt), LeftExpr(left), RightFactor(right) {
    assert(nt == NT_TensorExpr || nt == NT_DotExpr);
  }

  const Expr *getLeft() const { return LeftExpr; }
  const Factor *getRight() const { return RightFactor; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left,
                            const Factor *right) {
    return new BinaryExpr(nt, left, right);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class Identifier : public Factor {
private:
  const std::string Name;

public:
  Identifier(const std::string name) : Factor(NT_Identifier), Name(name) {}

  const std::string &getName() const { return Name; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Identifier *create(const std::string &name) {
    return new Identifier(name);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class Integer : public Factor {
private:
  int Value;

public:
  Integer(int value) : Factor(NT_Integer), Value(value) {}

  int getValue() const { return Value; }

  virtual void _delete() const final {}

  virtual void dump(unsigned indent = 0) const final;

  static const Integer *create(int value) { return new Integer(value); }

  virtual void visit(ASTVisitor *v) const override;
};

class ExprList : public NodeList<const Expr, NT_ExprList, ExprList> {
public:
  ExprList() : NodeList() {}
  ExprList(const Expr *e) : NodeList(e) {}

  virtual void visit(ASTVisitor *v) const override;
};

class BrackExpr : public Factor {
private:
  const ExprList *Exprs;

public:
  BrackExpr(const ExprList *exprs) : Factor(NT_BrackExpr), Exprs(exprs) {}

  const ExprList *getExprs() const { return Exprs; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class ParenExpr : public Factor {
private:
  const Expr *NestedExpr;

public:
  ParenExpr(const Expr *expr) : Factor(NT_ParenExpr), NestedExpr(expr) {}

  const Expr *getExpr() const { return NestedExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class Stmt : public Node {
private:
  const Identifier *Id;
  const Expr *RightExpr;

public:
  Stmt(const Identifier *id, const Expr *expr)
      : Node(NT_Stmt), Id(id), RightExpr(expr) {}

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class StmtList : public NodeList<const Stmt, NT_StmtList, StmtList> {
public:
  StmtList() : NodeList() {}
  StmtList(const Stmt *s) : NodeList(s) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Decl : public Node {
private:
  const Identifier *Id;
  const Expr *TypeExpr;

public:
  Decl(NodeType nt, const Identifier *id, const Expr *expr)
      : Node(nt), Id(id), TypeExpr(expr) {
    assert(nt == NT_VarDecl || nt == NT_TypeDecl);
  }

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getTypeExpr() const { return TypeExpr; }

  virtual void _delete() const final;

  virtual void dump(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id,
                            const Expr *expr) {
    return new Decl(nt, id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};

class DeclList : public NodeList<const Decl, NT_DeclList, DeclList> {
public:
  DeclList() : NodeList() {}
  DeclList(const Decl *d) : NodeList(d) {}

  virtual void visit(ASTVisitor *v) const override;
};

class Program : public Node {
private:
  const DeclList *Decls;
  const StmtList *Stmts;

public:
  Program(const DeclList *decls, const StmtList *stmts)
      : Node(NT_Program), Decls(decls), Stmts(stmts) {}

  const DeclList *getDecls() const { return Decls; }
  const StmtList *getStmts() const { return Stmts; }

  virtual void dump(unsigned indent = 0) const final;

  virtual void _delete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts) {
    return new Program(decls, stmts);
  }

  static void destroy(const Program *p) {
    p->_delete();
    delete p;
  }

  virtual void visit(ASTVisitor *v) const override;
};

class ASTVisitor {

public:
  virtual void visitProgram(const Program *) = 0;
  virtual void visitDeclList(const DeclList *) = 0;
  virtual void visitStmtList(const StmtList *) = 0;
  virtual void visitExprList(const ExprList *) = 0;
  virtual void visitDecl(const Decl *) = 0;
  virtual void visitStmt(const Stmt *) = 0;
  virtual void visitExpr(const Expr *) = 0;
  virtual void visitFactor(const Factor *) = 0;
  virtual void visitBinaryExpr(const BinaryExpr *) = 0;
  virtual void visitIdentifier(const Identifier *) = 0;
  virtual void visitInteger(const Integer *) = 0;
  virtual void visitBrackExpr(const BrackExpr *) = 0;
  virtual void visitParenExpr(const ParenExpr *) = 0;
};

#endif /* __AST_H__ */
