//==------ ExprTree.h - Representation of expression tree ------------------==//
//
// This file defines interfaces of expression tree. Expression tree records
// the states of each tensor expression. We can do some transformation over
// it to get high performace code.
//
//===----------------------------------------------------------------------===//

#ifndef __EXPR_TREE_H__
#define __EXPR_TREE_H__

#include <cassert>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "ph/CodeGen/CodeGen.h"
#include "ph/Sema/SymbolTable.h"

class ExprTreeVisitor;
class ExprTreeTransformer;

class ExprNode {
public:
  enum ExprKind {
    EK_Add,
    EK_Sub,
    EK_Mul,
    EK_Div,
    EK_Contraction,
    EK_Product,
    EK_Stack,
    EK_Identifier,

    EK_EXPRKIND_COUNT
  };

  // FIXME: Now we only record static tensor.
  using ExprDimensions = std::vector<int>;

private:
  const ExprKind ExKind;

  const int NumChildren;

  std::vector<ExprNode *> Children;

  ExprDimensions Dims;

public:
  int getNumChildren() const { return NumChildren; }

  const ExprNode *getChild(int i) const {
    assert(i < getNumChildren());
    return Children[i];
  }

  ExprNode *getChild(int i) {
    assert(i < getNumChildren());
    return Children[i];
  }

public:
  ExprNode(ExprKind ek, int numChildren,
           const ExprDimensions &dims = ExprDimensions());
  virtual ~ExprNode() {}

  enum ExprKind getExprKind() const { return ExKind; }

  void setChild(int i, ExprNode *en) {
    assert(i < getNumChildren());
    Children[i] = en;
  }

protected:
  void setDims(const ExprDimensions &dims) { Dims = dims; }

  static std::map<ExprKind, std::string> ExprLabel;

public:
  const ExprDimensions &getDims() const { return Dims; }

  virtual void _delete() const;

  virtual void dump(unsigned indent = 0) const;

  virtual void visit(ExprTreeVisitor *v) const = 0;
  virtual void transform(ExprTreeTransformer *t) = 0;

  virtual bool isIdentifier() const { return false; }
  virtual std::string getName() const { return ""; }
  virtual bool isContractionExpr() const { return false; }
  virtual bool isStackExpr() const { return false; }
};

#define DECL_EXPR_NODE(Kind)                                                   \
  class Kind##Expr : public ExprNode {                                         \
  public:                                                                      \
    Kind##Expr(ExprNode *lhs, ExprNode *rhs) : ExprNode(EK_##Kind, 2) {        \
      setChild(0, lhs);                                                        \
      setChild(1, rhs);                                                        \
      /* for type checking of element-wise operation , the following must      \
       * hold: 'lhs->getDims() == rhs->getDims()'                              \
       */                                                                      \
      setDims(lhs->getDims());                                                 \
    }                                                                          \
                                                                               \
    virtual void visit(ExprTreeVisitor *v) const;                              \
    virtual void transform(ExprTreeTransformer *t);                            \
                                                                               \
    static Kind##Expr *create(ExprNode *lhs, ExprNode *rhs) {                  \
      return new Kind##Expr(lhs, rhs);                                         \
    }                                                                          \
  };

DECL_EXPR_NODE(Add)
DECL_EXPR_NODE(Sub)
DECL_EXPR_NODE(Mul)
DECL_EXPR_NODE(Div)

#undef DECL_EXPR_NODE

class ProductExpr : public ExprNode {
public:
  ProductExpr(ExprNode *lhs, ExprNode *rhs);

  virtual void visit(ExprTreeVisitor *v) const;
  virtual void transform(ExprTreeTransformer *t);

  static ProductExpr *create(ExprNode *lhs, ExprNode *rhs) {
    return new ProductExpr(lhs, rhs);
  }
};

class ContractionExpr : public ExprNode {
private:
  const CodeGen::List LeftIndices;
  const CodeGen::List RightIndices;

public:
  ContractionExpr(ExprNode *lhs, const CodeGen::List &leftIndices,
                  ExprNode *rhs, const CodeGen::List &rightIndices);

  virtual bool isContractionExpr() const override { return true; }

  const CodeGen::List &getLeftIndices() const { return LeftIndices; }
  const CodeGen::List &getRightIndices() const { return RightIndices; }

  virtual void dump(unsigned indent = 0) const override;

  virtual void visit(ExprTreeVisitor *v) const override;
  virtual void transform(ExprTreeTransformer *t) override;

  static ContractionExpr *create(ExprNode *lhs,
                                 const CodeGen::List &leftIndices,
                                 ExprNode *rhs,
                                 const CodeGen::List &rightIndices) {
    return new ContractionExpr(lhs, leftIndices, rhs, rightIndices);
  }
};

class StackExpr : public ExprNode {
public:
  StackExpr(const std::vector<ExprNode *> &members);

  virtual bool isStackExpr() const override { return true; }

  virtual void visit(ExprTreeVisitor *v) const override;
  virtual void transform(ExprTreeTransformer *t) override;

  static StackExpr *create(const std::vector<ExprNode *> &members) {
    return new StackExpr(members);
  }
};

class IdentifierExpr : public ExprNode {
private:
  const std::string Name;

public:
  IdentifierExpr(const std::string &name, const ExprDimensions &dims)
      : ExprNode(EK_Identifier, 0, dims), Name(name) {}

  virtual bool isIdentifier() const override { return true; }
  virtual std::string getName() const override { return Name; }

  virtual void dump(unsigned indent = 0) const override;

  virtual void visit(ExprTreeVisitor *v) const override;
  virtual void transform(ExprTreeTransformer *t) override;

  static IdentifierExpr *create(const std::string &name,
                                const ExprDimensions &dims) {
    return new IdentifierExpr(name, dims);
  }
};

#endif // __EXPR_TREE_H__
