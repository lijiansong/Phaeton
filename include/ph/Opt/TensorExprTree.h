//==------ TensorExprTree.h - Representation of tensor expr tree -----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines interfaces of tensor expression tree. Tensor expression
// tree records the states of each tensor expression. We can do some
// transformation over it to get high performace code.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_OPT_TENSOR_EXPR_TREE_H
#define PHAETON_OPT_TENSOR_EXPR_TREE_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Sema/SymbolTable.h"

#include <cassert>
#include <string>
#include <vector>

namespace phaeton {

class ExprTreeVisitor;
class ExprTreeTransformer;

/// ExprNode - This class keeps track of state of each expression node.
class ExprNode {
public:
  /// ExprDims - Helper alias type traits to record dimensions of each
  /// expression.
  using ExprDims = std::vector<int>;
  /// ExprKind - Represent different expressions.
  enum ExprKind {
    EXPR_KIND_Add,
    EXPR_KIND_Sub,
    EXPR_KIND_Mul,
    EXPR_KIND_ScalarMul,
    EXPR_KIND_Div,
    EXPR_KIND_ScalarDiv,
    EXPR_KIND_Identifier,
    EXPR_KIND_Product,
    EXPR_KIND_Stack,
    EXPR_KIND_Contraction,
    EXPR_KIND_Transposition,

    EXPR_KIND_EXPRKIND_COUNT
  };

public:
  ExprNode(ExprKind Kind, int NumChildren, const ExprDims &ED = ExprDims());

  virtual ~ExprNode() {}

  int getNumChildren() const { return NumChildren; }

  const ExprNode *getChild(int I) const {
    assert(I < getNumChildren());
    return Children[I];
  }

  ExprNode *getChild(int I) {
    assert(I < getNumChildren());
    return Children[I];
  }

  enum ExprKind getExprKind() const { return ExKind; }

  void setChild(int I, ExprNode *Node) {
    assert(I < getNumChildren());
    Children[I] = Node;
  }

  const ExprDims &getDims() const { return Dims; }
  virtual void _delete() const;
  virtual void dump(unsigned Indent = 0) const;
  virtual void visit(ExprTreeVisitor *Visitor) const = 0;
  virtual void transform(ExprTreeTransformer *Transformer) = 0;
  virtual bool isIdentifier() const { return false; }
  virtual std::string getName() const { return ""; }
  virtual const std::string getIndex(unsigned I) const { return ""; };
  virtual unsigned getNumIndices() const { return 0; }
  virtual bool isContractionExpr() const { return false; }
  virtual bool isStackExpr() const { return false; }
  virtual bool isTranspositionExpr() const { return false; }

protected:
  void setDims(const ExprDims &ED) { Dims = ED; }
  static std::map<ExprKind, std::string> ExprLabel;

private:
  const ExprKind ExKind;
  const int NumChildren;
  std::vector<ExprNode *> Children;
  ExprDims Dims;
};

#define GEN_EXPR_NODE_CLASS_DECL(ExprName)                                     \
  class ExprName##Expr : public ExprNode {                                     \
  public:                                                                      \
    ExprName##Expr(ExprNode *LHS, ExprNode *RHS)                               \
        : ExprNode(EXPR_KIND_##ExprName, 2) {                                  \
      setChild(0, LHS);                                                        \
      setChild(1, RHS);                                                        \
      /* Type checking for element-wise expr, the following must hold:         \
       * LHS->getDims() == RHS->getDims()                                      \
       */                                                                      \
      setDims(LHS->getDims());                                                 \
    }                                                                          \
                                                                               \
    virtual void visit(ExprTreeVisitor *Visitor) const;                        \
    virtual void transform(ExprTreeTransformer *Transformer);                  \
                                                                               \
    static ExprName##Expr *create(ExprNode *LHS, ExprNode *RHS) {              \
      return new ExprName##Expr(LHS, RHS);                                     \
    }                                                                          \
  };

GEN_EXPR_NODE_CLASS_DECL(Add)
GEN_EXPR_NODE_CLASS_DECL(Sub)
GEN_EXPR_NODE_CLASS_DECL(Mul)
GEN_EXPR_NODE_CLASS_DECL(Div)

#undef GEN_EXPR_NODE_CLASS_DECL

/// ScalarMulExpr - This class represents scalar multiplication expression node.
class ScalarMulExpr : public ExprNode {
public:
  ScalarMulExpr(ExprNode *LHS, ExprNode *RHS);

  virtual void visit(ExprTreeVisitor *Visitor) const;
  virtual void transform(ExprTreeTransformer *Transformer);

  static ScalarMulExpr *create(ExprNode *LHS, ExprNode *RHS) {
    return new ScalarMulExpr(LHS, RHS);
  }
};

/// ScalarDivExpr - This class represents for scalar division expression.
class ScalarDivExpr : public ExprNode {
public:
  ScalarDivExpr(ExprNode *LHS, ExprNode *RHS);

  virtual void visit(ExprTreeVisitor *Visitor) const;
  virtual void transform(ExprTreeTransformer *Transformer);

  static ScalarDivExpr *create(ExprNode *LHS, ExprNode *RHS) {
    return new ScalarDivExpr(LHS, RHS);
  }
};

/// IdentifierExpr - This class represents for identifier expression.
class IdentifierExpr : public ExprNode {
public:
  IdentifierExpr(const std::string &Name, const ExprDims &ED)
      : ExprNode(EXPR_KIND_Identifier, /*NumChildren=*/0, ED), Name(Name),
        Permute(false), ElementIndexPosition(-1) {}

  void addIndex(const std::string &Index) { Indices.push_back(Index); }
  virtual const std::string getIndex(unsigned I) const override;
  virtual unsigned getNumIndices() const override { return Indices.size(); }

  virtual bool isIdentifier() const override { return true; }
  virtual std::string getName() const override { return Name; }

  virtual void dump(unsigned Indent = 0) const override;

  virtual void visit(ExprTreeVisitor *Visitor) const override;
  virtual void transform(ExprTreeTransformer *Transformer) override;

  static IdentifierExpr *create(const std::string &Name, const ExprDims &ED) {
    return new IdentifierExpr(Name, ED);
  }

  bool getPermute() const { return Permute; }
  void setPermute(bool IsPermute) { Permute = IsPermute; }
  const CodeGen::TupleList &getIndexPairs() const { return IndexPairs; }
  void setIndexPairs(const CodeGen::TupleList &Pairs) { IndexPairs = Pairs; }
  bool isElementIndexPositionSet() const { return ElementIndexPosition >= 0; }
  int getElementIndexPosition() const { return ElementIndexPosition; }
  void setElementIndexPosition(int Pos) { ElementIndexPosition = Pos; }

private:
  const std::string Name;
  std::vector<std::string> Indices;
  bool Permute;
  CodeGen::TupleList IndexPairs;
  int ElementIndexPosition;
};

/// ProductExpr - This class represents for tensor product expression.
class ProductExpr : public ExprNode {
public:
  ProductExpr(ExprNode *LHS, ExprNode *RHS);

  virtual void visit(ExprTreeVisitor *Visitor) const;
  virtual void transform(ExprTreeTransformer *Transformer);

  static ProductExpr *create(ExprNode *LHS, ExprNode *RHS) {
    return new ProductExpr(LHS, RHS);
  }
};

/// ContractionExpr - This class represents for tensor contraction expression.
class ContractionExpr : public ExprNode {
public:
  ContractionExpr(ExprNode *LHS, const CodeGen::List &LeftIndex, ExprNode *RHS,
                  const CodeGen::List &RightIndex);

  virtual bool isContractionExpr() const override { return true; }

  const CodeGen::List &getLeftIndices() const { return LeftIndices; }
  const CodeGen::List &getRightIndices() const { return RightIndices; }

  virtual void dump(unsigned Indent = 0) const override;

  virtual void visit(ExprTreeVisitor *Visitor) const override;
  virtual void transform(ExprTreeTransformer *Transformer) override;

  static ContractionExpr *create(ExprNode *LHS, const CodeGen::List &LeftIndex,
                                 ExprNode *RHS,
                                 const CodeGen::List &RightIndex) {
    return new ContractionExpr(LHS, LeftIndex, RHS, RightIndex);
  }

private:
  const CodeGen::List LeftIndices;
  const CodeGen::List RightIndices;
};

/// StackExpr - This class represents for tensor stack expression.
class StackExpr : public ExprNode {
public:
  StackExpr(const std::vector<ExprNode *> &Members);

  virtual bool isStackExpr() const override { return true; }

  virtual void visit(ExprTreeVisitor *Visitor) const override;
  virtual void transform(ExprTreeTransformer *Transformer) override;

  static StackExpr *create(const std::vector<ExprNode *> &Members) {
    return new StackExpr(Members);
  }
};

/// TranspositionExpr - This class represents for tensor transposition
/// expression.
class TranspositionExpr : public ExprNode {
public:
  TranspositionExpr(ExprNode *Node, const CodeGen::TupleList &IndexPairs);

  virtual bool isTranspositionExpr() const override { return true; }

  const CodeGen::TupleList &getIndexPairs() const { return IndexPairs; }

  virtual void dump(unsigned Indent = 0) const override;

  virtual void visit(ExprTreeVisitor *Visitor) const override;
  virtual void transform(ExprTreeTransformer *Transformer) override;

  static TranspositionExpr *create(ExprNode *Node,
                                   const CodeGen::TupleList &IndexPairs) {
    return new TranspositionExpr(Node, IndexPairs);
  }

private:
  const CodeGen::TupleList IndexPairs;
};

} // end namespace phaeton

#endif // PHAETON_OPT_TENSOR_EXPR_TREE_H
