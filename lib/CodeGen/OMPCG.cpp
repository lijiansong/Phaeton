//==--- OMPCG.cpp ----- Interface to code generation for CPU OpenMP --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class for OpenMP code generation targeting the CPUs.
//
//===----------------------------------------------------------------------===//

#include "ph/CodeGen/OMPCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"

#define OMP_CG_INDENT(indent)                                                  \
  { append(std::string((indent), ' ')); }

#define OMP_INDENT_PER_LEVEL (2)

// TODO: Add pragma for parallel
void OMPCG::genCode(const Program *p) {
  nestingLevel = initialNestingLevel = 0;
  // Emit kernel description
  append("// ----- Autogen kernel by Phaeton -----\n\n");

  if (EmitWrapper) {
    append("static inline\n");
  }

  // Emit C function name signature
  emitSignature();

  // Block for C function body
  append("{\n");
  // Function signature are emitted at nesting level '0',
  // function body starts at level '1':
  nestingLevel = initialNestingLevel = 1;

  // Construct the expression trees, one for each statement in the program
  CG->visitProgram(p);

  for (const auto *s : CG->getStatements()) {
    std::list<CodeGen::Assignment> Assignments;

    // Lift contractions and tensor stacks out of the expression tree
    // on the RHS of the statement 's':
    {
      ExprNode *en = getExprNode(s->getExpr());

      const auto &nodeLiftPredicate = [](const ExprNode *en) {
        return (en->isStackExpr() || en->isContractionExpr());
      };
      ExprTreeLifter lifter(CG, nodeLiftPredicate);

      en->transform(&lifter);

      for (const auto &ass : lifter.getAssignments())
        Assignments.push_back(ass);

      // The result of the expression tree that remains as 'en' after lifting
      // must be assigned to the variable on the LHS of the statement 's'
      Assignments.push_back({s->getIdentifier()->getName(), en});
    }

    for (const auto &ass : Assignments) {
      const Sema &sema = *getSema();

      const std::string result = ass.variable;
      const ExprNode *en = ass.en;
      const std::vector<int> &dims = getDims(en);

      nestingLevel = initialNestingLevel;

      // Emit defintion of 'result' if necessary
      if (!sema.is_in_inputs(result) && !sema.is_in_outputs(result)) {
        auto elements = [](const std::vector<int> &ds) {
          int es = 1;
          for (int i = 0; i < ds.size(); i++)
            es *= ds[i];
          return es;
        };

        OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
        append(getFPTypeName() + " " + result + "[" +
               std::to_string(elements(dims)) + "];\n");
      }

      // Generate enough indices for the expression
      exprIndices.clear();
      for (int i = 0; i < dims.size(); i++) {
        const std::string index = getIndex();
        exprIndices.push_back(index);
      }

      loopedOverIndices.clear();

      // Code generation for identifiers has been optimized out
      if (en->isIdentifier()) {
        assert(0 && "internal error: assigning from identifier at top level");
      } else {
        setResultTemp(result + emitSubscriptString(exprIndices, dims));
        en->visit(this);
      }

      assert((nestingLevel - initialNestingLevel) == loopedOverIndices.size());

      // Emit footer to close all for-loops
      emitLoopFooterNest();
    }
  }

  // Close the block of C function body
  append("}\n");

  // Emit wrapper kernel.
  if (EmitWrapper) {
    const int numArgs = getNumFuncArgs();

    append("\n");
    append("void " + getFuncName() + "(" + getFPTypeName() + " *args[" +
           std::to_string(numArgs) + "]){\n");

    const std::string &functionCall = getFuncNameWrapped() + "(";
    // Emit the call to wrapper function 'FuncName'
    OMP_CG_INDENT(OMP_INDENT_PER_LEVEL)
    append(functionCall);
    // Emit arguments list of wrapper 'FuncName'
    for (int i = 0; i < numArgs; i++) {
      if (i != 0) {
        append(",\n");
        OMP_CG_INDENT(OMP_INDENT_PER_LEVEL + functionCall.length())
      }

      append("args[" + std::to_string(i) + "]");
    }
    append(");\n");

    append("}\n");
  }
}

std::string OMPCG::getIndex() { return "i" + std::to_string(IndexCounter++); }

// TODO: Emit function signature with name mangling
void OMPCG::emitSignature() {
  const Sema &sema = *getSema();

  bool isFirstArgument = true;
  const std::string &fn_name = getFuncName();
  if (!fn_name.size()) {
    append("void omp_func(\n");
  } else {
    // append("void " + getFuncName() + "(\n");
    append("void " + getFuncNameWrapped() + "(\n");
  }

  // FIXME: Emit inputs as function arguments
  for (auto in = sema.inputs_begin(); in != sema.inputs_end(); in++) {
    const Symbol *sym = *in;

    if (isFirstArgument)
      isFirstArgument = false;
    else
      append(",\n");

    const std::string &ArgName = sym->getName();
    // Indent arguments
    OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + ArgName);
    addFuncArg(ArgName);
  }

  // FIXME: Emit outputs as function arguments
  for (auto out = sema.outputs_begin(); out != sema.outputs_end(); out++) {
    const Symbol *sym = *out;

    // Do not re-emit input arguments
    if (sema.is_in_inputs(sym->getName()))
      continue;

    if (isFirstArgument)
      isFirstArgument = false;
    else
      append(",\n");

    const std::string &ArgName = sym->getName();
    // Indent arguments
    OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + ArgName);
    addFuncArg(ArgName);
  }

  // Finish function signature.
  append(")\n");
}

void OMPCG::emitForLoopHeader(unsigned indent, const std::string &indexVar,
                              const std::string &bound) {
  OMP_CG_INDENT(indent)
  append("for (unsigned " + indexVar + " = 0; " + indexVar + " < " + bound +
         "; " + "++" + indexVar + ") {\n");
}

void OMPCG::emitForLoopHeader(unsigned indent, const std::string &indexVar,
                              int intBound) {
  emitForLoopHeader(indent, indexVar, std::to_string(intBound));
}

void OMPCG::emitForLoopFooter(unsigned indent) {
  OMP_CG_INDENT(indent)
  append("}\n");
}

void OMPCG::emitTempDefinition(unsigned indent, const std::string &temp,
                               const std::string &init) {
  OMP_CG_INDENT(indent)
  append(getFPTypeName() + " " + temp);
  if (init.length())
    append(" = " + init);
  append(";\n");
}

std::string OMPCG::emitSubscriptString(const std::vector<std::string> &indices,
                                       const std::vector<int> &dims) const {
  assert(indices.size() == dims.size());

  const int rank = dims.size();

  // for processing scalar op.
  if (rank == 0)
    return "[0]";

  std::string result = "(";

  if (RowMajor) {
    for (int i = (rank - 1); i >= 0; i--) {
      result += indices[i];
      if (i != 0)
        result += " + " + std::to_string(dims[i]) + "*(";
    }
  } else {
    for (int i = 0; i < rank; i++) {
      result += indices[i];
      if (i != (rank - 1))
        result += " + " + std::to_string(dims[i]) + "*(";
    }
  }

  for (int i = 0; i < rank; i++) {
    result += ")";
  }

  return "[" + result + "]";
}

std::string OMPCG::visitChildExpr(const ExprNode *en,
                                  const std::vector<int> &childExprDims) {
  std::string temp;

  if (en->isIdentifier()) {
    temp = en->getName() + emitSubscriptString(exprIndices, childExprDims);
  } else {
    temp = getTemp();
    emitTempDefinition(nestingLevel * OMP_INDENT_PER_LEVEL, temp);
    setResultTemp(temp);
    en->visit(this);
  }

  return temp;
}

void OMPCG::emitLoopHeaderNest(const std::vector<int> &exprDims) {
  const int rank = exprIndices.size();

  if (RowMajor) {
    for (int i = 0; i < rank; i++) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // Note: 'index' is already iterated over in a for-loop
        continue;
      }
      emitForLoopHeader(nestingLevel * OMP_INDENT_PER_LEVEL, index,
                        exprDims[i]);
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  } else {
    for (int i = (rank - 1); i >= 0; i--) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // Note: 'index' is already iterated over in a for-loop
        continue;
      }
      emitForLoopHeader(nestingLevel * OMP_INDENT_PER_LEVEL, index,
                        exprDims[i]);
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  }
}

void OMPCG::emitLoopFooterNest() {
  while (nestingLevel > initialNestingLevel) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel * OMP_INDENT_PER_LEVEL);
  }
}

void OMPCG::visitBinOpExpr(const ExprNode *en, const std::string &op) {
  // FIXME: Here 'BinOp' is elementwise operation, the two
  // operands have the same ranks and dimensions
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
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
    // Note: One argument of binary op is a scalar, we need to
    // split the 'exprIndices' accordingly.
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
    // Note: Both arguments of binary op are tensors,
    // therefore, the same 'exprIndices' must be used for both arguments.
    lhsIndices = rhsIndices = exprIndices;
  }

  // Emit LHS.
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr, lhsDims);

  // Emit RHS.
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr, rhsDims);

  // Emit for-loop nest as appropriate.
  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims);

  OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
  append(result + " = " + lhsTemp + " " + op + " " + rhsTemp + ";\n");
}

void OMPCG::visitAddExpr(const AddExpr *en) { visitBinOpExpr(en, "+"); }

void OMPCG::visitSubExpr(const SubExpr *en) { visitBinOpExpr(en, "-"); }

void OMPCG::visitMulExpr(const MulExpr *en) { visitBinOpExpr(en, "*"); }

void OMPCG::visitDivExpr(const DivExpr *en) { visitBinOpExpr(en, "/"); }

void OMPCG::visitScalarMulExpr(const ScalarMulExpr *en) {
  visitBinOpExpr(en, "*");
}

void OMPCG::visitScalarDivExpr(const ScalarDivExpr *en) {
  visitBinOpExpr(en, "/");
}

void OMPCG::visitProductExpr(const ProductExpr *en) {
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  // Determine which indices belong to LHS.
  for (int i = 0; i < lhsDims.size(); i++) {
    const std::string &index = exprIndices[i];
    lhsIndices.push_back(index);
  }
  // Determine which indices belong to RHS.
  for (int i = 0; i < rhsDims.size(); i++) {
    const std::string &index = exprIndices[lhsDims.size() + i];
    rhsIndices.push_back(index);
  }

  const std::vector<std::string> savedExprIndices = exprIndices;

  // Visit LHS.
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr, lhsDims);

  // Visit RHS, emit loop headers as necessary.
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr, rhsDims);

  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims);

  OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
  append(result + " = " + lhsTemp + " * " + rhsTemp + ";\n");
}

void OMPCG::visitContractionExpr(const ContractionExpr *en) {
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
  std::string lhsTemp, rhsTemp;

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
  // Determine which indices belong to LHS.
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < lhsDims.size(); i++) {
      std::string index;

      if (i == en->getLeftIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      lhsIndices.push_back(index);
    }
  }
  // Determine which indices belong to RHS.
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < rhsDims.size(); i++) {
      std::string index;

      if (i == en->getRightIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      rhsIndices.push_back(index);
    }
  }

  // Emit for-loop nest for result.
  emitLoopHeaderNest(exprDims);
  OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
  append(result + " = 0.0;\n");

  const std::vector<std::string> savedExprIndices = exprIndices;

  // Visit LHS.
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr, lhsDims);
  // Emit for-loop nest for LHS.
  emitLoopHeaderNest(lhsDims);

  // Visit RHS, emit loop headers when necessary.
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr, rhsDims);
  // Emit for-loop nest for RHS.
  emitLoopHeaderNest(rhsDims);

  OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
  append(result + " += " + lhsTemp + " * " + rhsTemp + ";\n");

  // 1. Close for-loops that iterate over the contracted indices;
  for (int i = 0; i < contrIndices.size(); i++) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel * OMP_INDENT_PER_LEVEL);
  }
  // 2. Then remove the contracted indices from 'loopedOverIndices'.
  for (int i = 0; i < contrIndices.size(); i++)
    loopedOverIndices.erase(contrIndices[i]);

  exprIndices = savedExprIndices;
}

void OMPCG::visitStackExpr(const StackExpr *en) {
  // FIXME: Current implementation of this function yields correct results
  // ONLY if it emits code at the top level (i.e. no nest in any for loops)
  assert(nestingLevel == initialNestingLevel);
  const std::string result = getResultTemp();

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  const std::string &firstResultIndex = savedExprIndices[0];
  std::vector<std::string> childExprIndices;
  // Skip the first index.
  for (int i = 1; i < savedExprIndices.size(); i++)
    childExprIndices.push_back(savedExprIndices[i]);

  exprIndices = childExprIndices;

  for (int e = 0; e < en->getNumChildren(); e++) {
    // Replace the first index in 'result' with constant 'e'.
    const std::string &firstResultConstantIndex = std::to_string(e);
    const size_t i = result.find(firstResultIndex);
    std::string resultWithConstantFirstIndex = result;
    if (i != std::string::npos)
      resultWithConstantFirstIndex.replace(i, firstResultIndex.length(),
                                           firstResultConstantIndex);

    const ExprNode *child = en->getChild(e);
    const std::vector<int> childDims = getDims(child);

    exprIndices = childExprIndices;
    std::string temp = visitChildExpr(child, childDims);
    emitLoopHeaderNest(childDims);

    OMP_CG_INDENT(nestingLevel * OMP_INDENT_PER_LEVEL);
    append(resultWithConstantFirstIndex + " = " + temp + ";\n");

    // 1. Close the for-loops over 'childExprIndices';
    while (nestingLevel > savedNestingLevel) {
      --nestingLevel;
      emitForLoopFooter(nestingLevel * OMP_INDENT_PER_LEVEL);
    }
    // 2. Then remove loop indices from 'loopedOverIndices'.
    for (int i = 0; i < childExprIndices.size(); i++)
      loopedOverIndices.erase(exprIndices[i]);
  }

  exprIndices = savedExprIndices;
}

void OMPCG::visitIdentifierExpr(const IdentifierExpr *en) {
  assert(
      0 &&
      "internal error: code generation for identifier has been optimized out");
}

#undef OMP_CG_INDENT
#undef OMP_INDENT_PER_LEVEL
