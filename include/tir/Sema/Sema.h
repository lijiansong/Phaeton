#ifndef __SEMA_H__
#define __SEMA_H__

#include <list>
#include <map>

#include "tir/AST/AST.h"
#include "tir/Sema/Symbol.h"
#include "tir/Sema/Type.h"

class Sema : public ASTVisitor {
private:
  const TensorType *scalar;
  std::list<const TensorType *> Types;

  SymbolTable Symbols;

  // map 'Expr' nodes in the AST to types:
  std::map<const Expr *, const TensorType *> ExprTypes;

public:
  Sema() {
    scalar = new TensorType(std::vector<int>());
    Types.push_back(scalar);
  }

  ~Sema() {
    for (auto it : Types)
      delete it;

    for (auto it : Symbols)
      delete it.second;
  }

  const TensorType *createType(const std::vector<int> &dims) {
    const TensorType *type = new TensorType(dims);
    Types.push_back(type);
    return type;
  }

  const TensorType *getType(const std::vector<int> &dims) {
    for (auto it : Types) {
      if (it->equals(dims))
        return it;
    }

    return createType(dims);
  }

  Symbol *createSymbol(SymbolKind k, const std::string &name,
                       const TensorType &type, const Decl *decl = nullptr) {
    Symbol *sym = new Symbol(k, name, type, decl);
    Symbols.addSymbol(sym);
    return sym;
  }

  Symbol *getSymbol(const std::string &name) const {
    Symbol *sym;
    if (!Symbols.getSymbol(name, sym))
      return nullptr;

    return sym;
  }

  bool isTypeName(const Expr *e, const TensorType *&type) const;
  bool isIntegerList(const Expr *e, std::vector<int> &ints) const;
  bool isListOfLists(const Expr *e, std::vector<std::vector<int>> &lists) const;

  const TensorType *visitTypeExpr(const Expr *e);

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitExpr(const Expr *e) override;
  virtual void visitFactor(const Factor *f) override;
  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  template <typename T, NodeType nt, typename Derived>
  void visitNodeList(const NodeList<T, nt, Derived> *list);

#define VISIT_LIST(Derived)                                                    \
  virtual void visit##Derived(const Derived *list) override {                  \
    visitNodeList(list);                                                       \
  }
  VISIT_LIST(DeclList)
  VISIT_LIST(StmtList)
  VISIT_LIST(ExprList)

  virtual void visitProgram(const Program *prog) override;
};

#endif /* !__SEMA_H__ */
