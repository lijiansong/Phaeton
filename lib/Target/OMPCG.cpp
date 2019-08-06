//==--- OMPCG.cpp ----- Interface to code generation for CPU OpenMP --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for OpenMP code generation targeting the CPUs.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/OMPCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ContractionExprCounter.h"
#include "ph/Opt/IdentifierCopier.h"
#include "ph/Opt/IdentifierFinder.h"
#include "ph/Opt/ParentFinder.h"
#include "ph/Opt/StackExprRemover.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#define EMIT_INDENT(indent)                                                    \
  { append(std::string((indent), ' ')); }
#define INDENT_PER_LEVEL (2)

using namespace phaeton;

void OMPCG::genCode(const Program *p) {
  nestingLevel = initialNestingLevel = 0;

  if (EmitWrapper) {
    append("static inline\n");
  }

  // emit the function signature.
  std::vector<std::vector<int>> ArgumentsDimensions;
  emitSignature(ArgumentsDimensions);

  // open block for function body.
  append("{\n");
  // the function signature are emitted at nesting level '0', the function
  // body (emission of which is about to follow) starts at level '1':
  nestingLevel = initialNestingLevel = 1;

  // construct the expression trees, one for each statement in the program:
  CG->visitProgram(p);

  // various transformations
  {
    // (1) move transpositions over to the 'lhs'; we should do this before
    // lifting stack expressions
    for (auto &a : CG->getAssignments()) {
      if (!a.rhs->isTranspositionExpr())
        continue;

      IdentifierExpr *id = static_cast<IdentifierExpr *>(a.lhs);
      TranspositionExpr *rhs = static_cast<TranspositionExpr *>(a.rhs);

      id->setPermute(true);
      id->setIndexPairs(rhs->getIndexPairs());

      a.rhs = rhs->getChild(0);
    }

    // (2) remove stack expressions;
    StackExprRemover remover(CG);
    remover.transformAssignments();

    std::map<const ExprNode *, const IdentifierExpr *> RhsToLhsMap;
    for (auto &a : CG->getAssignments())
      RhsToLhsMap[a.rhs] = static_cast<const IdentifierExpr *>(a.lhs);

    // (3.1) lift contractions to the top level;
    const auto &nodeLiftPredicate =
        [&RhsToLhsMap](const ExprNode *en, const ExprNode *root) -> bool {
      if (en->isStackExpr()) {
        assert(0 &&
               "internal error: stack expressions should not occur any more");
      }

      if (en->isContractionExpr()) {
        ContractionExprCounter CEC(root);
        CEC.run();
        // lift contractions only if there are more than one,
        // starting with the most deeply nested contraction;
        if (CEC.getCount() > 1 && en == CEC.getDeepest())
          return true;

        // contractions should also be lifted if the identifier from
        // the 'lhs' appears in the contraction (on the 'rhs'); this
        // will produce more efficient code than lifting identifiers
        // later in the 'IdentifierCopier' (cf. transformation no. 4)
        IdentifierFinder IF(root);
        const IdentifierExpr *lhsId = RhsToLhsMap[root];
        if (IF.find(lhsId->getName())) {
          if (IF.getIdIncompatible() || lhsId->permute())
            return true;
        }
      }
      return false;
    };
    ExprTreeLifter lifter(CG, nodeLiftPredicate);
    lifter.transformAssignments();

    // (3.1) lift out any expressions below contractions;
    // Note: in general, multiplication or division by a scalar
    // can probably remain under a contraction.
    const auto &subContrLiftPredicate = [](const ExprNode *en,
                                           const ExprNode *root) -> bool {
      ParentFinder PF(root);
      auto *p = PF.find(en);
      if (p && p->isContractionExpr())
        return true;

      return false;
    };
    ExprTreeLifter lifter2(CG, subContrLiftPredicate);
    lifter2.transformAssignments();

    // (4) copy identifiers if they appear on 'lhs' and 'rhs'
    // in incompatible ways.
    IdentifierCopier IDC(CG);
    IDC.transformAssignments();
  }
  // end of transformations

  const Sema &sema = *getSema();

  const bool hasElementLoop = sema.getElemInfo().present;
  if (hasElementLoop) {
    if (OMPPragmas) {
      EMIT_INDENT(initialNestingLevel * INDENT_PER_LEVEL);
      append("#pragma omp for\n");
    }

    ElementIndex = getIndex();
    emitForLoopHeader(initialNestingLevel * INDENT_PER_LEVEL, ElementIndex,
                      sema.getElemInfo().dim);
    ++initialNestingLevel;
  }

  bool fuse = false, fuseNext = false;
  std::set<std::string> emittedNames;
  for (auto a = CG->getAssignments().begin(), e = CG->getAssignments().end();
       a != e; a++) {
    const auto &na = std::next(a);

    assert(a->lhs->isIdentifier() &&
           "internal error: left-hand side must be an identifier expression");

    const IdentifierExpr *result = static_cast<const IdentifierExpr *>(a->lhs);
    const std::string &resultName = result->getName();

    const IdentifierExpr *nextResult = nullptr;
    std::string nextResultName = "";

    if (na == CG->getAssignments().end()) {
      fuseNext = false;
    } else {
      nextResult = static_cast<const IdentifierExpr *>(na->lhs);
      nextResultName = nextResult->getName();
      fuseNext = (getDims(nextResult) == getDims(result));
    }
    if (fuseNext) {
      ContractionExprCounter CEC(na->rhs);
      CEC.run();
      fuseNext = fuseNext && (CEC.getCount() == 0);

      IdentifierFinder IF(na->rhs);
      bool found = IF.find(resultName);
      // cannot fuse if the next 'rhs' contains an incompatible
      // use of the result of the current assignement.
      if (found && IF.getIdIncompatible())
        fuseNext = false;
      if (result->permute() || nextResult->permute())
        fuseNext = false;
    }

    if (!fuse)
      nestingLevel = initialNestingLevel;

    // emit defintion of 'result' if necessary.
    if (!sema.is_in_inputs(resultName) && !sema.is_in_outputs(resultName) &&
        !emittedNames.count(resultName)) {
      EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
      append(getFPTypeName() + " " + resultName + dimsString(getDims(result)) +
             ";\n");

      emittedNames.insert(resultName);
    }
    // emit definition of next result if fused.
    if (fuseNext && !sema.is_in_inputs(nextResultName) &&
        !sema.is_in_outputs(nextResultName) &&
        !emittedNames.count(nextResultName)) {
      EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
      append(getFPTypeName() + " " + nextResultName +
             dimsString(getDims(nextResult)) + ";\n");

      emittedNames.insert(nextResultName);
    }

    const ExprNode *en = a->rhs;
    const std::vector<int> &dims = getDims(en);

    if (!fuse) {
      // generate enough indices for the expression.
      exprIndices.clear();
      resultIndices.clear();
      loopedOverIndices.clear();
      for (int i = 0; i < dims.size(); i++) {
        const std::string index = getIndex();
        exprIndices.push_back(index);
        resultIndices.push_back(index);
      }
    }

    setResultTemp(a->lhs);
    // Note: here we need this if-clause since code emission
    // for identifiers has been optimized out.
    if (en->isIdentifier()) {
      // however, as a result of transforming stack expressions,
      // assignments with nothing but an identifier on the 'rhs' may
      // appear at the top level:
      visitTopLevelIdentifier(en);
    } else {
      en->visit(this);
    }

    assert((nestingLevel - initialNestingLevel) == loopedOverIndices.size());

    if (fuseNext) {
      fuse = true;
    }
    if (!fuseNext) {
      // close all for-loops.
      emitLoopFooterNest();
      fuse = false;
    }
    fuseNext = false;
  }

  if (hasElementLoop) {
    --initialNestingLevel;
    emitForLoopFooter(initialNestingLevel * INDENT_PER_LEVEL);
  }

  // close block of function body.
  append("}\n");

  if (EmitWrapper) {
    const int numArgs = getNumFunctionArguments();
    assert(numArgs == ArgumentsDimensions.size());

    append("\n");
    const std::string &wrapperDef = "void " + getFunctionName() + "(";
    append(wrapperDef);
    for (int i = 0; i < numArgs; i++) {
      if (i != 0) {
        append(",\n");
        EMIT_INDENT(wrapperDef.length())
      }
      append(getFPTypeName() + " *arg" + std::to_string((long long)i));
    }
    append("){\n");

    const std::string &functionCall = getFunctionNameWrapped() + "(";
    // emit the call of 'FunctionName'.
    // FIXME: remove here.
    EMIT_INDENT(INDENT_PER_LEVEL)
    append(functionCall);
    // emit the list of actual arguments of 'FunctionName'
    for (int i = 0; i < numArgs; i++) {
      if (i != 0) {
        append(",\n");
        EMIT_INDENT(INDENT_PER_LEVEL + functionCall.length())
      }

      std::vector<int> dims = ArgumentsDimensions[i];
      if (dims.size() > 0) {
        dims = RowMajor ? std::vector<int>(dims.begin() + 1, dims.end())
                        : std::vector<int>(dims.begin(), dims.end() - 1);
        if (dims.size() > 0)
          append("(" + getFPTypeName() + "(*)" + dimsString(dims) + ")");
      } else {
        assert(dims.size() == 0);
        // dereference pointer
        append("*");
      }

      append("arg" + std::to_string((long long)i));
    }
    append(");\n");

    append("}\n");
  }
}

std::string OMPCG::getIndex() {
  return "i" + std::to_string((long long)IndexCounter++);
}

void OMPCG::emitSignature(std::vector<std::vector<int>> &argumentsDims) {
  // emit function signature, and
  // populate argument with the dimensions of the function arguments
  const Sema &sema = *getSema();

  bool isFirstArgument = true;
  append("void " + getFunctionNameWrapped() + "(\n");

  // dummy argument for 'updateWithElemInfo' method
  std::vector<std::string> dummy;

  // emit inputs as function arguments
  for (auto in = sema.inputs_begin(); in != sema.inputs_end(); in++) {
    const Symbol *sym = *in;

    if (isFirstArgument)
      isFirstArgument = false;
    else
      append(",\n");

    const std::string &ArgName = sym->getName();
    std::vector<int> ArgDims = sym->getType().getDims();
    updateWithElemInfo(dummy, ArgDims, ArgName);

    // indent each argument
    EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL)
    append(getFPTypeName() + " " + ArgName +
           dimsString(ArgDims, RestrictPointer));

    addFunctionArgument(ArgName);

    argumentsDims.push_back(ArgDims);
  }

  // emit outputs as function arguments
  for (auto out = sema.outputs_begin(); out != sema.outputs_end(); out++) {
    const Symbol *sym = *out;

    // do not re-emit input arguments
    if (sema.is_in_inputs(sym->getName()))
      continue;

    if (isFirstArgument)
      isFirstArgument = false;
    else
      append(",\n");

    const std::string &ArgName = sym->getName();
    std::vector<int> ArgDims = sym->getType().getDims();
    updateWithElemInfo(dummy, ArgDims, ArgName);

    // indent each argument
    EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL)
    append(getFPTypeName() + " " + ArgName +
           dimsString(ArgDims, RestrictPointer));

    addFunctionArgument(ArgName);

    argumentsDims.push_back(ArgDims);
  }

  // finish function signature:
  append(")\n");
}

// Function to determine the maximum power of 2 for unrolling such that
// no more than two loop iterations must be peeled off the end of the loop.
//
// But function is currently unused!
// Generally, unroll factor is currently not used!
//
#if 0
static int getUnrollFactor(const int bnd) {
  if (bnd <= 2)
    return 0;

  int result = 2;
  for (int f = result; f <= bnd; f *= 2) {
    if (bnd % f <= 2)
      result = f;
  }
  return result;

}
#endif

void OMPCG::emitForLoopHeader(unsigned indent, const std::string &indexVar,
                              const std::string &bound) {
  EMIT_INDENT(indent)
  append("for (unsigned " + indexVar + " = 0; " + indexVar + " < " + bound +
         "; " + indexVar + "++) {\n");
}

void OMPCG::emitForLoopHeader(unsigned indent, const std::string &indexVar,
                              int intBound, bool unroll, bool simd) {
  if (unroll && IccPragmas) {
  // unroll factor is currently not used.
  // FIXME: here maybe wrong.
#if 0
    const int factor = getUnrollFactor(intBound);
    if (factor > 0) {
      EMIT_INDENT(indent)
      append("#pragma unroll (" + std::to_string(factor) + ")\n");
    }
#endif
    EMIT_INDENT(indent)
    append("#pragma unroll\n");
  }

  if (simd && IccPragmas) {
    EMIT_INDENT(indent)
    append("#pragma simd\n");
  }
  if (simd && OMPPragmas) {
    EMIT_INDENT(indent)
    append("#pragma omp simd\n");
  }

  emitForLoopHeader(indent, indexVar, std::to_string((long long)intBound));
}

void OMPCG::emitForLoopFooter(unsigned indent) {
  EMIT_INDENT(indent)
  append("}\n");
}

void OMPCG::emitTempDefinition(unsigned indent, const std::string &temp) {
  EMIT_INDENT(indent)
  append(getFPTypeName() + " " + temp + ";\n");
}

std::string OMPCG::subscriptString(const std::vector<std::string> &indices,
                                   const std::vector<int> &dims) const {
  assert(indices.size() == dims.size());

  const int rank = dims.size();
  if (rank == 0)
    return "";

  std::string result = "";

  if (RowMajor) {
    for (int i = 0; i < rank; i++) {
      result += "[" + indices[i] + "]";
    }
  } else {
    for (int i = (rank - 1); i >= 0; i--) {
      result += "[" + indices[i] + "]";
    }
  }

  return result;
}

std::string OMPCG::dimsString(const std::vector<int> &dims,
                              bool emitRestrict) const {
  const int rank = dims.size();
  if (rank == 0)
    return "";

  std::string result = "";

  if (RowMajor) {
    for (int i = 0; i < rank; i++) {
      const std::string restrictQual =
          (emitRestrict && (i == 0)) ? "restrict " : "";
      result += "[" + restrictQual + std::to_string(dims[i]) + "]";
    }
  } else {
    for (int i = (rank - 1); i >= 0; i--) {
      const std::string restrictQual =
          (emitRestrict && (i == (rank - 1))) ? "restrict " : "";
      result += "[" + restrictQual + std::to_string(dims[i]) + "]";
    }
  }

  return result;
}

void OMPCG::updateWithElemInfo(std::vector<std::string> &indices,
                               std::vector<int> &dims,
                               const std::string &name) const {
  const Sema &sema = *getSema();
  const Sema::ElemInfo &info = sema.getElemInfo();

  if (!info.present)
    return;

  const Symbol *s = sema.getSymbol(name);
  if (info.syms.find(s) == info.syms.end())
    return;

  if (info.pos == ElemDirect::POS_Last) {
    dims.push_back(info.dim);
    indices.push_back(ElementIndex);
  } else if (info.pos == ElemDirect::POS_First) {
#define PREPEND(vec, element, default)                                         \
  (vec).push_back((default));                                                  \
  for (unsigned i = ((vec).size() - 1); i > 0; i--)                            \
    (vec)[i] = (vec)[i - 1];                                                   \
  (vec)[0] = (element);

    PREPEND(indices, ElementIndex, "")
    PREPEND(dims, info.dim, 0)
  } else {
    assert(0 && "internal error: invalid position specifier");
  }
}

std::string
OMPCG::subscriptedIdentifier(const ExprNode *en,
                             const std::vector<std::string> &indices) const {
  assert(en->isIdentifier());

  std::vector<std::string> allIndices;
  for (unsigned i = 0; i < en->getNumIndices(); i++)
    allIndices.push_back(en->getIndex(i));
  for (unsigned i = 0; i < indices.size(); i++)
    allIndices.push_back(indices.at(i));

  const IdentifierExpr *id = static_cast<const IdentifierExpr *>(en);
  if (id->permute()) {
    // the indices must be permuted going through the transpositions forwards.
    // Note that when 'TranspositionExpr' is emitted on the 'rhs' of
    // assignments, the index pairs are traversed backwards; here the
    // permutations appear on the 'lhs' of assignments, hence index pairs are
    // traversed forwards.
    const auto &indexPairs = id->getIndexPairs();
    for (auto pi = indexPairs.begin(); pi != indexPairs.end(); pi++) {
      const auto p = *pi;
      assert(p.size() == 2);

      const std::string index0 = allIndices[p[0]];
      allIndices[p[0]] = allIndices[p[1]];
      allIndices[p[1]] = index0;
    }
  }

  std::vector<int> dims = en->getDims();
  updateWithElemInfo(allIndices, dims, id->getName());
  return (en->getName() + subscriptString(allIndices, dims));
}

void OMPCG::emitLoopHeaderNest(const std::vector<int> &exprDims, bool unroll,
                               bool simd) {
  const int rank = exprIndices.size();

  if (RowMajor) {
    int ifirst = -1, ilast = -1;
    {
      // determine the first and the last value of 'i' at which
      // a new loop header will actually be emitted.
      for (int i = 0; i < rank; i++) {
        const std::string &index = exprIndices[i];
        if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
          continue;
        }
        if (ifirst == -1)
          ifirst = i;
        ilast = i;
      }
    }

    for (int i = 0; i < rank; i++) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // 'index' is already iterated over in a for-loop.
        continue;
      }
      emitForLoopHeader(
          nestingLevel * INDENT_PER_LEVEL, index, exprDims[i],
          // emit 'unroll' pragma only for the first loop header.
          unroll && (i == ifirst),
          // emit 'simd' pragma only for the last loop header, and only
          // if the first and the last loop header do no coincide.
          // This avoids having both pragmas before the same loop header.
          simd && (i == ilast) && (ifirst != ilast));
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  } else {
    int ifirst = -1, ilast = -1;
    {
      // determine the first and the last value of 'i' at which
      // a new loop header will actually be emitted.
      for (int i = (rank - 1); i >= 0; i--) {
        const std::string &index = exprIndices[i];
        if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
          continue;
        }
        if (ifirst == -1)
          ifirst = i;
        ilast = i;
      }
    }
    for (int i = (rank - 1); i >= 0; i--) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // 'index' is already iterated over in a for-loop.
        continue;
      }
      emitForLoopHeader(
          nestingLevel * INDENT_PER_LEVEL, index, exprDims[i],
          // emit 'unroll' pragma only for the first loop header.
          unroll && (i == ifirst),
          // emit 'simd' pragma only for the last loop header, and only
          // if the first and the last loop header do no coincide:
          // This avoids having both pragmas before the same loop header.
          //(simd && (i == ilast)  && (exprDims[i] % 4 == 0)) ||
          //(simd && (i == ifirst) && (exprDims[i] % 4 != 0)));
          (simd && (i == ilast)));
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  }
}

void OMPCG::emitLoopFooterNest() {
  while (nestingLevel > initialNestingLevel) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel * INDENT_PER_LEVEL);
  }
}

std::string OMPCG::visitChildExpr(const ExprNode *en) {
  if (en->isIdentifier()) {
    return subscriptedIdentifier(en, exprIndices);
  } else {
    const std::string temp = getTemp();
    emitTempDefinition(nestingLevel * INDENT_PER_LEVEL, temp);
    ExprNode *tempNode = CG->getENBuilder()->createIdentifierExpr(temp, {});

    std::vector<std::string> savedResultIndices = resultIndices;
    setResultTemp(tempNode);
    resultIndices = {};
    en->visit(this);
    resultIndices = savedResultIndices;

    return subscriptedIdentifier(tempNode);
  }
}

void OMPCG::visitBinOpExpr(const ExprNode *en, const std::string &op) {
  // Note here 'BinOp' includes elementwise and scalar operations
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();

  std::string lhsTemp, rhsTemp;

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  if (lhsDims.size() == 0 || rhsDims.size() == 0) {
    // one of the arguments of this 'BinOp' is a scalar,
    // hence, split the 'exprIndices' accordingly.
    for (int i = 0; i < lhsDims.size(); i++) {
      const std::string &index = exprIndices[i];
      lhsIndices.push_back(index);
    }
    for (int i = 0; i < rhsDims.size(); i++) {
      const std::string &index = exprIndices[lhsDims.size() + i];
      rhsIndices.push_back(index);
    }
  } else {
    assert(lhsDims.size() == rhsDims.size());
    // both arguments of this 'BinOps' are tensors,
    // hence, the same 'exprIndices' must be used for both arguments.
    lhsIndices = rhsIndices = exprIndices;
  }

  // emit LHS
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);

  // emit RHS
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);

  // emit for-loop nest as appropriate.
  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims, false, true);

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices) + " = " + lhsTemp + " " +
         op + " " + rhsTemp + ";\n");
}

void OMPCG::visitAddExpr(const AddExpr *en) { visitBinOpExpr(en, "+"); }

void OMPCG::visitSubExpr(const SubExpr *en) { visitBinOpExpr(en, "-"); }

void OMPCG::visitMulExpr(const MulExpr *en) { visitBinOpExpr(en, "*"); }

void OMPCG::visitScalarMulExpr(const ScalarMulExpr *en) {
  visitBinOpExpr(en, "*");
}

void OMPCG::visitDivExpr(const DivExpr *en) { visitBinOpExpr(en, "/"); }

void OMPCG::visitScalarDivExpr(const ScalarDivExpr *en) {
  visitBinOpExpr(en, "/");
}

void OMPCG::visitProductExpr(const ProductExpr *en) {
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  // determine which indices belong to the 'lhs'.
  for (int i = 0; i < lhsDims.size(); i++) {
    const std::string &index = exprIndices[i];
    lhsIndices.push_back(index);
  }
  // determine which indices belong to the 'rhs'.
  for (int i = 0; i < rhsDims.size(); i++) {
    const std::string &index = exprIndices[lhsDims.size() + i];
    rhsIndices.push_back(index);
  }

  const std::vector<std::string> savedExprIndices = exprIndices;

  // visit the 'lhs'.
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);

  // visit the 'rhs', emit loop headers as necessary.
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);

  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims);

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices) + " = " + lhsTemp +
         " * " + rhsTemp + ";\n");
}

void OMPCG::visitContractionExpr(const ContractionExpr *en) {
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();
  std::string lhsTemp, rhsTemp;
  const std::string temp = getTemp();

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> contrIndices;
  for (int i = 0; i < en->getLeftIndices().size(); i++) {
    const std::string &index = getIndex() + "_contr";
    contrIndices.push_back(index);
  }

  std::vector<std::string> lhsIndices, rhsIndices;
  int exprIdxCounter = 0;
  // determine which indices belong to the 'lhs'.
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < lhsDims.size(); i++) {
      std::string index;

      if (contrIdxCounter < en->getLeftIndices().size() &&
          i == en->getLeftIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      lhsIndices.push_back(index);
    }
  }
  // determine which indices belong to the 'rhs'.
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < rhsDims.size(); i++) {
      std::string index;

      if (contrIdxCounter < en->getRightIndices().size() &&
          i == en->getRightIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      rhsIndices.push_back(index);
    }
  }

  // emit for-loop nest for the result.
  emitLoopHeaderNest(exprDims, false, true);

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(getFPTypeName() + " " + temp + " = 0.0;\n");

  const std::vector<std::string> savedExprIndices = exprIndices;

  // visit LHS
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);
  // emit for-loop nest for the 'lhs' early
  emitLoopHeaderNest(lhsDims, false, false);

  // visit 'rhs', emit loop headers as necessary
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);
  // emit for-loop nest for 'rhs'
  emitLoopHeaderNest(rhsDims, false, false);

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(temp + " += " + lhsTemp + " * " + rhsTemp + ";\n");

  // 1. close the for-loops that iterate over the contracted indices
  for (int i = 0; i < contrIndices.size(); i++) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel * INDENT_PER_LEVEL);
  }

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices) + " = " + temp + ";\n");

  // 2. remove the contracted indices from 'loopedOverIndices'
  for (int i = 0; i < contrIndices.size(); i++)
    loopedOverIndices.erase(contrIndices[i]);

  exprIndices = savedExprIndices;
}

void OMPCG::visitStackExpr(const StackExpr *en) {
  // Note: the current implementation of this function yields correct results
  // ONLY if it emits code at the top level (no nestinf in any for loops)
  // assert(nestingLevel == initialNestingLevel);
  // Note: since this method has been called from the top level, 'exprIndices'
  // and 'resultIndices' must agree
  // assert(exprIndices == resultIndices);

  const ExprNode *result = getResultTemp();

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  const std::string &firstResultIndex = savedExprIndices[0];
  std::vector<std::string> childExprIndices;
  // skip the first index
  for (int i = 1 /* The '1' is intended! */; i < savedExprIndices.size(); i++)
    childExprIndices.push_back(savedExprIndices[i]);

  exprIndices = childExprIndices;

  for (int e = 0; e < en->getNumChildren(); e++) {
    // replace the first index in 'result' with the constant 'e'
    IdentifierExpr *id = CG->getENBuilder()->createIdentifierExpr(
        result->getName(), result->getDims());
    for (unsigned i = 0; i < result->getNumIndices(); i++)
      id->addIndex(result->getIndex(i));
    id->addIndex(std::to_string((long long)e));

    const ExprNode *child = en->getChild(e);
    const std::vector<int> childDims = getDims(child);

    exprIndices = childExprIndices;
    std::string temp = visitChildExpr(child);
    emitLoopHeaderNest(childDims);

    EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
    append(subscriptedIdentifier(id, childExprIndices) + " = " + temp + ";\n");

    // 1. close the for-loops over the 'childExprIndices';
    while (nestingLevel > savedNestingLevel) {
      --nestingLevel;
      emitForLoopFooter(nestingLevel * INDENT_PER_LEVEL);
    }
    // 2. remove the loop indices from 'loopedOverIndices':
    for (int i = 0; i < childExprIndices.size(); i++)
      loopedOverIndices.erase(exprIndices[i]);
  }

  exprIndices = savedExprIndices;
}

void OMPCG::visitTranspositionExpr(const TranspositionExpr *en) {
  const std::vector<std::string> savedExprIndices = exprIndices;
  // const int savedNestingLevel = nestingLevel;

  // emit loop header before transposing indices
  emitLoopHeaderNest(getDims(en), true, true);

  std::vector<std::string> transposedExprIndices = exprIndices;
  const std::vector<std::vector<int>> &indexPairs = en->getIndexPairs();
  // traverse index pairs in reverse order and apply transpositions
  // Note that previously, e.g. in 'Sema', index pairs are traversed in order to
  // synthesize the type of an expression bottom-up; now going top-down
  // to construct the 'exprIndices'.
  for (auto pi = indexPairs.rbegin(); pi != indexPairs.rend(); pi++) {
    const auto p = *pi;
    assert(p.size() == 2);
    const std::string index0 = transposedExprIndices[p[0]];
    transposedExprIndices[p[0]] = transposedExprIndices[p[1]];
    transposedExprIndices[p[1]] = index0;
  }

  exprIndices = transposedExprIndices;

  const ExprNode *child = en->getChild(0);
  const ExprNode *result = getResultTemp();

  if (child->isIdentifier()) {
    const IdentifierExpr *id = static_cast<const IdentifierExpr *>(child);
    assert(!id->permute() &&
           "internal error: only identifiers on the 'lhs' may be transposed");
    EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
    append(subscriptedIdentifier(result, resultIndices) + " = " +
           subscriptedIdentifier(child, exprIndices) + ";\n");
  } else {
    child->visit(this);
  }

  exprIndices = savedExprIndices;
}

void OMPCG::visitTopLevelIdentifier(const ExprNode *en) {
  // Note that this function is only allowed to be called from the top level
  assert(nestingLevel == initialNestingLevel);
  const ExprNode *result = getResultTemp();

  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  emitLoopHeaderNest(dims, true, true);

  EMIT_INDENT(nestingLevel * INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, exprIndices) + " = " +
         subscriptedIdentifier(en, exprIndices) + ";\n");
}

void OMPCG::visitIdentifierExpr(const IdentifierExpr *en) {
  assert(0 &&
         "internal error: code emission for identifier has been optimized out");
}
