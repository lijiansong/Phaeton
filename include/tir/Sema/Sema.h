#ifndef __SEMA_H__
#define __SEMA_H__

#include <list>
#include <map>

#include "tir/AST/AST.h"
#include "tir/Sema/Type.h"
#include "tir/Sema/Symbol.h"

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

  const TensorType *getType(const Expr *e) const { return ExprTypes.at(e); }

  const Symbol *createSymbol(SymbolKind k, const std::string &name,
                             const TensorType &type,
                             const Decl *decl = nullptr) {
    Symbol *sym = new Symbol(k, name, type, decl);
    Symbols.addSymbol(sym);
    return sym;
  }

  const Symbol *getSymbol(const std::string &name) const {
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

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
};

#endif /* !__SEMA_H__ */
