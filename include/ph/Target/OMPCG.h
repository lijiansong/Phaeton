//==------ OMPCG.h ------- Representation of code generation for OpenMP ----==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines the base class CodeGen for CPU C with OpenMP pragmas.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TARGET_OMPCG_H
#define PHAETON_TARGET_OMPCG_H

#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Type.h"

#include <list>
#include <string>
#include <vector>

namespace phaeton {

/// OMPCG - This class defines the entrance for OpenMP code generator. It
/// maintains some boolean variables to control some optimization and
/// transformation pass.
///
/// TODO: We need to abstract a pass manager for different target languages.
class OMPCG : public ExprTreeVisitor {
public:
  OMPCG(CodeGen *Gen, bool IsRowMajor = true, bool IsEmitWrapper = false,
        bool IsRestrictPointer = false, bool IsIccPragmas = false,
        bool IsOpenMpPragmas = false, const std::string FpTypeName = "float")
      : CG(Gen), FloatPointTypeName(FpTypeName), IndexCounter(0),
        RowMajor(IsRowMajor), EmitWrapper(IsEmitWrapper),
        RestrictPointer(IsRestrictPointer), IccPragmas(IsIccPragmas),
        OMPPragmas(IsOpenMpPragmas) {}

  const std::string &getCode() const { return CG->getTgtLangCode(); }
  void genCode(const Program *Prog);

protected:
  const std::string &getFloatPointTypeName() const {
    return FloatPointTypeName;
  }

  const ExpressionNode *getResultTmp() const { return ResultTmp; }
  void setResultTmp(const ExpressionNode *Tmp) { ResultTmp = Tmp; }

  unsigned getIndent() const { return Indent; }
  void setIndent(unsigned Indent) { Indent = Indent; }

  std::string getIndex();

  /// emitSignature - Emit kernel function signature, we need to keep track of
  /// these variables marked with 'in' and 'out' specifiers which will be the
  /// kernel function arguments.
  void emitSignature(std::vector<std::vector<int>> &ArgumentsDims);

  void emitForLoopHeader(unsigned Indent, const std::string &IndexVar,
                         const std::string &Bound);
  void emitForLoopHeader(unsigned Indent, const std::string &IndexVar,
                         int IntBound, bool Unroll = false, bool Simd = false);
  void emitForLoopFooter(unsigned Indent);

  void emitTmpDefinition(unsigned Indent, const std::string &Tmp);

  std::string subscriptString(const std::vector<std::string> &Indices,
                              const std::vector<int> &Dims) const;
  std::string dimsString(const std::vector<int> &Dims,
                         bool EmitRestrict = false) const;

  void updateWithElemInfo(std::vector<std::string> &Indices,
                          std::vector<int> &Dims,
                          const std::string &Name) const;
  std::string
  subscriptedIdentifier(const ExpressionNode *Node,
                        const std::vector<std::string> &Indices = {}) const;

  void emitLoopHeaderNest(const std::vector<int> &ExprDims, bool Unroll = false,
                          bool Simd = false);
  void emitLoopFooterNest();

  std::string visitChildExpr(const ExpressionNode *Node);

  void visitBinOpExpr(const ExpressionNode *Node, const std::string &Operation);

#define GEN_VISIT_EXPR_NODE_DECL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *E) override;

  GEN_VISIT_EXPR_NODE_DECL(Add)
  GEN_VISIT_EXPR_NODE_DECL(Sub)
  GEN_VISIT_EXPR_NODE_DECL(Mul)
  GEN_VISIT_EXPR_NODE_DECL(ScalarMul)
  GEN_VISIT_EXPR_NODE_DECL(Div)
  GEN_VISIT_EXPR_NODE_DECL(ScalarDiv)
  GEN_VISIT_EXPR_NODE_DECL(Contraction)
  GEN_VISIT_EXPR_NODE_DECL(Product)
  GEN_VISIT_EXPR_NODE_DECL(Stack)
  GEN_VISIT_EXPR_NODE_DECL(Transposition)
  GEN_VISIT_EXPR_NODE_DECL(Identifier)

#undef GEN_VISIT_EXPR_NODE_DECL

  void visitTopLevelIdentifier(const ExpressionNode *Node);

private:
  CodeGen *CG;
  const ExpressionNode *ResultTmp;
  const std::string FloatPointTypeName;
  unsigned Indent;
  unsigned IndexCounter;
  const bool RowMajor;
  const bool EmitWrapper;
  const bool RestrictPointer;
  const bool IccPragmas;
  const bool OMPPragmas;

  // We need to keep track of context for the emission of expression trees.
  std::set<std::string> LoopedOverIndices;
  unsigned InitialNestLevel;
  unsigned NestLevel;
  std::vector<std::string> ResultIndices;
  std::vector<std::string> ExprIndices;

  std::string ElementIndex;
  // Helper methods for CodeGen.
  std::string getTmp() { return CG->getTmp(); }
  std::string getTmpWithDims(const std::vector<int> &Dim) {
    return CG->getTmpWithDims(Dim);
  }
  void freeTmpWithDims(const std::string &Name, const std::vector<int> &Dim) {
    CG->freeTmpWithDims(Name, Dim);
  }
  void appendCode(const std::string &Code) { CG->appendCode(Code); }
  const Sema *getSema() const { return CG->getSema(); }
  const std::vector<int> &getDims(const ExpressionNode *Node) const {
    return Node->getDims();
  }
  // FIXME: remove later
  const std::string &getCGFuncName() const { return CG->getCGFuncName(); }
  std::string getCGFuncNameWrapped() const {
    std::string FnName = CG->getCGFuncName();
    if (EmitWrapper) {
      FnName += "_wrapper_autogen";
    }
    return FnName;
  }
  void addFuncArg(const std::string &Name) { CG->addFuncArg(Name); }
  int getNumFuncArgs() const { return CG->getFuncArgs().size(); }
};

} // end namespace phaeton

#endif // PHAETON_TARGET_OMPCG_H
