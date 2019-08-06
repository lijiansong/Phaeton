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
  OMPCG(CodeGen *cg, bool rowMajor = true, bool emitWrapper = false,
        bool restrictPointer = false, bool iccPragmas = false,
        bool ompPragmas = false, const std::string fpTypeName = "float")
      : CG(cg), FPTypeName(fpTypeName), IndexCounter(0), RowMajor(rowMajor),
        EmitWrapper(emitWrapper), RestrictPointer(restrictPointer),
        IccPragmas(iccPragmas), OMPPragmas(ompPragmas) {}

  void genCode(const Program *p);
  const std::string &getCode() const { return CG->getCode(); }

protected:
  const std::string &getFPTypeName() const { return FPTypeName; }

  const ExprNode *getResultTemp() const { return resultTemp; }
  void setResultTemp(const ExprNode *temp) { resultTemp = temp; }

  unsigned getIndent() const { return Indent; }
  void setIndent(unsigned indent) { Indent = indent; }

  std::string getIndex();

  void emitSignature(std::vector<std::vector<int>> &argumentsDims);

  void emitForLoopHeader(unsigned indent, const std::string &indexVar,
                         const std::string &bound);
  void emitForLoopHeader(unsigned indent, const std::string &indexVar,
                         int intBound, bool unroll = false, bool simd = false);
  void emitForLoopFooter(unsigned indent);

  void emitTempDefinition(unsigned indent, const std::string &temp);

  std::string subscriptString(const std::vector<std::string> &indices,
                              const std::vector<int> &dims) const;
  std::string dimsString(const std::vector<int> &dims,
                         bool emitRestrict = false) const;

  void updateWithElemInfo(std::vector<std::string> &indices,
                          std::vector<int> &dims,
                          const std::string &name) const;
  std::string
  subscriptedIdentifier(const ExprNode *en,
                        const std::vector<std::string> &indices = {}) const;

  void emitLoopHeaderNest(const std::vector<int> &exprDims, bool unroll = false,
                          bool simd = false);
  void emitLoopFooterNest();

  std::string visitChildExpr(const ExprNode *en);

#define GEN_VISIT_EXPR_NODE_DECL(ExprName)                                     \
  virtual void visit##ExprName##Expr(const ExprName##Expr *e) override;

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

  void visitBinOpExpr(const ExprNode *en, const std::string &op);
  void visitTopLevelIdentifier(const ExprNode *en);

private:
  CodeGen *CG;
  const std::string FPTypeName;
  const ExprNode *resultTemp;
  unsigned Indent;
  unsigned IndexCounter;
  const bool RowMajor;
  const bool EmitWrapper;
  const bool RestrictPointer;
  const bool IccPragmas;
  const bool OMPPragmas;

  // We need to keep track of context for the emission of expression trees.
  std::set<std::string> loopedOverIndices;
  unsigned nestingLevel, initialNestingLevel;
  std::vector<std::string> exprIndices, resultIndices;

  std::string ElementIndex;
  // Helper methods for CodeGen.
  std::string getTemp() { return CG->getTemp(); }
  std::string getTempWithDims(const std::vector<int> &dims) {
    return CG->getTempWithDims(dims);
  }
  void freeTempWithDims(const std::string &name, const std::vector<int> &dims) {
    CG->freeTempWithDims(name, dims);
  }
  void append(const std::string &code) { CG->append(code); }
  const Sema *getSema() const { return CG->getSema(); }
  const std::vector<int> &getDims(const ExprNode *en) const {
    return en->getDims();
  }
  void addFunctionArgument(const std::string &name) {
    CG->addFunctionArgument(name);
  }
  int getNumFunctionArguments() const {
    return CG->getFunctionArguments().size();
  }

  // FIXME: remove later
  const std::string &getFunctionName() const { return CG->getFunctionName(); }
  std::string getFunctionNameWrapped() const {
    std::string name = CG->getFunctionName();
    if (EmitWrapper)
      name += "_wrapped";

    return name;
  }
};

} // end namespace phaeton

#endif // PHAETON_TARGET_OMPCG_H
