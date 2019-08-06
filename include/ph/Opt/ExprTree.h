//==------ ExprTree.h - Representation of expression tree ------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of expression tree. Expression tree records
// the states of each tensor expression. We can do some transformation over
// it to get high performace code.
//
//===----------------------------------------------------------------------===//
#ifndef PHAETON_OPT_EXPR_TREE_H
#define PHAETON_OPT_EXPR_TREE_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Sema/SymbolTable.h"

#include <cassert>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace phaeton {

class ExprTreeVisitor;
class ExprTreeTransformer;

class ExprNode {
public:
  enum ExprKind {
    EXPR_KIND_Add,
    EXPR_KIND_Sub,
    EXPR_KIND_Mul,
    EXPR_KIND_ScalarMul,
    EXPR_KIND_Div,
    EXPR_KIND_ScalarDiv,
    EXPR_KIND_Contraction,
    EXPR_KIND_Product,
    EXPR_KIND_Stack,
    EXPR_KIND_Transposition,
    EXPR_KIND_Identifier,

    EXPR_KIND_EXPRKIND_COUNT
  };

  using ExprDimensions = std::vector<int>;

public:
  ExprNode(ExprKind ek, int numChildren,
           const ExprDimensions &dims = ExprDimensions());

  virtual ~ExprNode() {}

  int getNumChildren() const { return NumChildren; }

  const ExprNode *getChild(int i) const {
    assert(i < getNumChildren());
    return Children[i];
  }

  ExprNode *getChild(int i) {
    assert(i < getNumChildren());
    return Children[i];
  }

  enum ExprKind getExprKind() const { return ExKind; }

  void setChild(int i, ExprNode *en) {
    assert(i < getNumChildren());
    Children[i] = en;
  }
  const ExprDimensions &getDims() const { return Dims; }
  virtual void _delete() const;
  virtual void dump(unsigned indent = 0) const;
  virtual void visit(ExprTreeVisitor *v) const = 0;
  virtual void transform(ExprTreeTransformer *t) = 0;
  virtual bool isIdentifier() const { return false; }
  virtual std::string getName() const { return ""; }
  virtual const std::string getIndex(unsigned i) const { return ""; };
  virtual unsigned getNumIndices() const { return 0; }
  virtual bool isContractionExpr() const { return false; }
  virtual bool isStackExpr() const { return false; }
  virtual bool isTranspositionExpr() const { return false; }

protected:
  void setDims(const ExprDimensions &dims) { Dims = dims; }
  static std::map<ExprKind, std::string> ExprLabel;

private:
  const ExprKind ExKind;
  const int NumChildren;
  std::vector<ExprNode *> Children;
  ExprDimensions Dims;
};

#define GEN_EXPR_NODE_CLASS_DECL(ExprName)                                     \
  class ExprName##Expr : public ExprNode {                                     \
  public:                                                                      \
    ExprName##Expr(ExprNode *lhs, ExprNode *rhs)                               \
        : ExprNode(EXPR_KIND_##ExprName, 2) {                                  \
      setChild(0, lhs);                                                        \
      setChild(1, rhs);                                                        \
      /* by type checking, the following must hold:                            \
       * 'lhs->getDims() == rhs->getDims()'                                    \
       */                                                                      \
      setDims(lhs->getDims());                                                 \
    }                                                                          \
                                                                               \
    virtual void visit(ExprTreeVisitor *v) const;                              \
    virtual void transform(ExprTreeTransformer *t);                            \
                                                                               \
    static ExprName##Expr *create(ExprNode *lhs, ExprNode *rhs) {              \
      return new ExprName##Expr(lhs, rhs);                                     \
    }                                                                          \
  };

GEN_EXPR_NODE_CLASS_DECL(Add)
GEN_EXPR_NODE_CLASS_DECL(Sub)
GEN_EXPR_NODE_CLASS_DECL(Mul)
GEN_EXPR_NODE_CLASS_DECL(Div)

#undef GEN_EXPR_NODE_CLASS_DECL

class ScalarMulExpr : public ExprNode {
public:
  ScalarMulExpr(ExprNode *lhs, ExprNode *rhs);

  virtual void visit(ExprTreeVisitor *v) const;
  virtual void transform(ExprTreeTransformer *t);

  static ScalarMulExpr *create(ExprNode *lhs, ExprNode *rhs) {
    return new ScalarMulExpr(lhs, rhs);
  }
};

class ScalarDivExpr : public ExprNode {
public:
  ScalarDivExpr(ExprNode *lhs, ExprNode *rhs);

  virtual void visit(ExprTreeVisitor *v) const;
  virtual void transform(ExprTreeTransformer *t);

  static ScalarDivExpr *create(ExprNode *lhs, ExprNode *rhs) {
    return new ScalarDivExpr(lhs, rhs);
  }
};

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

private:
  const CodeGen::List LeftIndices;
  const CodeGen::List RightIndices;
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

class TranspositionExpr : public ExprNode {
public:
  TranspositionExpr(ExprNode *en, const CodeGen::TupleList &indexPairs);

  virtual bool isTranspositionExpr() const override { return true; }

  const CodeGen::TupleList &getIndexPairs() const { return IndexPairs; }

  virtual void dump(unsigned indent = 0) const override;

  virtual void visit(ExprTreeVisitor *v) const override;
  virtual void transform(ExprTreeTransformer *t) override;

  static TranspositionExpr *create(ExprNode *en,
                                   const CodeGen::TupleList &indexPairs) {
    return new TranspositionExpr(en, indexPairs);
  }

private:
  const CodeGen::TupleList IndexPairs;
};

class IdentifierExpr : public ExprNode {
public:
  IdentifierExpr(const std::string &name, const ExprDimensions &dims)
      : ExprNode(EXPR_KIND_Identifier, 0, dims), Name(name), Permute(false),
        ElementIndexPosition(-1) {}

  void addIndex(const std::string &idx) { Indices.push_back(idx); }
  virtual const std::string getIndex(unsigned i) const override;
  virtual unsigned getNumIndices() const override { return Indices.size(); }

  virtual bool isIdentifier() const override { return true; }
  virtual std::string getName() const override { return Name; }

  virtual void dump(unsigned indent = 0) const override;

  virtual void visit(ExprTreeVisitor *v) const override;
  virtual void transform(ExprTreeTransformer *t) override;

  static IdentifierExpr *create(const std::string &name,
                                const ExprDimensions &dims) {
    return new IdentifierExpr(name, dims);
  }

  bool permute() const { return Permute; }
  void setPermute(bool p) { Permute = p; }
  const CodeGen::TupleList &getIndexPairs() const { return IndexPairs; }
  void setIndexPairs(const CodeGen::TupleList &pairs) { IndexPairs = pairs; }
  bool isElementIndexPositionSet() const { return ElementIndexPosition >= 0; }
  int getElementIndexPosition() const { return ElementIndexPosition; }
  void setElementIndexPosition(int pos) { ElementIndexPosition = pos; }

private:
  const std::string Name;
  std::vector<std::string> Indices;
  bool Permute;
  CodeGen::TupleList IndexPairs;
  int ElementIndexPosition;
};

} // end namespace phaeton

#endif // PHAETON_OPT_EXPR_TREE_H
