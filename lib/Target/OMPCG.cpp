//==--- OMPCG.cpp ----- Interface to code generation for CPU OpenMP --------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This provides a class implementation for OpenMP code generation targeting the
// CPUs.
//
//===----------------------------------------------------------------------===//

#include "ph/Target/OMPCG.h"
#include "ph/AST/AST.h"
#include "ph/Opt/ContrExprCounter.h"
#include "ph/Opt/ExprTreeLifter.h"
#include "ph/Opt/ExprTreeTransformer.h"
#include "ph/Opt/ExprTreeVisitor.h"
#include "ph/Opt/IdentifierCopier.h"
#include "ph/Opt/IdentifierFinder.h"
#include "ph/Opt/ParentFinder.h"
#include "ph/Opt/StackRemover.h"
#include "ph/Sema/Sema.h"
#include "ph/Sema/Type.h"
#include "ph/Support/ErrorHandling.h"

#define OMP_CG_INDENT(Indent)                                                  \
  { appendCode(std::string((Indent), ' ')); }

#define OMP_INDENT_PER_LEVEL (2)

using namespace phaeton;

std::string OMPCG::getIndex() {
  return "i" + std::to_string((long long)IndexCounter++);
}

// TODO: add a pass manager to refactor this method.
void OMPCG::genCode(const Program *Prog) {
  NestLevel = 0;
  InitialNestLevel = 0;

  if (EmitWrapper) {
    appendCode("static inline\n");
  }

  // First, we need to emit the function signature.
  std::vector<std::vector<int>> ArgumentsDimensions;
  emitSignature(ArgumentsDimensions);

  // Then we emit the open block for function body.
  appendCode("{\n");
  // Note that here we set function signature is emitted at nest level '0',
  // function body starts at level '1'.
  NestLevel = 1;
  InitialNestLevel = 1;

  // Then we need to construct the expression tree, one for each statement in
  // Phaeton program and visit program for code generation.
  CG->visitProgram(Prog);

  // Before CG, we need some common transformations to get high performance
  // code.
  {
    // 1. move transpositions over to 'LHS', and note that we should do this
    // before lifting stack expressions;
    for (auto &Assign : CG->getAssignments()) {
      if (!Assign.RHS->isTranspositionExpr()) {
        continue;
      }

      IdentifierExpr *Id = static_cast<IdentifierExpr *>(Assign.LHS);
      TranspositionExpr *RHS = static_cast<TranspositionExpr *>(Assign.RHS);

      Id->setPermute(true);
      Id->setIndexPairs(RHS->getIndexPairs());

      Assign.RHS = RHS->getChild(0);
    }

    // 2. We need to remove stack expressions;
    StackRemover Rm(CG);
    Rm.transformAssignments();

    std::map<const ExpressionNode *, const IdentifierExpr *> RHSToLHSMap;
    for (auto &Assign : CG->getAssignments()) {
      RHSToLHSMap[Assign.RHS] = static_cast<const IdentifierExpr *>(Assign.LHS);
    }

    // 3. Multiple steps for contractions.
    // 3.1 Lift contractions to the top level.
    const auto &nodeLiftPredicate =
        [&RHSToLHSMap](const ExpressionNode *Node,
                       const ExpressionNode *Root) -> bool {
      if (Node->isStackExpr()) {
        // Note that tensor stack expressions should not occur here.
        ph_unreachable(INTERNAL_ERROR
                       "stack expressions should not occur any more");
      }

      if (Node->isContractionExpr()) {
        // Note that 'ContrExprCounter' can help to lift contraction expressions
        // out of expression trees only if: 1). there are more than one
        // contraction in the tree, and 2). the contraction to be lifted is the
        // most deeply nested of all contractions in the expression tree.
        ContrExprCounter ContractionExprCounter(Root);
        ContractionExprCounter.run();
        // Lift contractions only if there are more than one,
        // here we start with the most deeply nested contraction.
        if (ContractionExprCounter.getCount() > 1 &&
            Node == ContractionExprCounter.getDeepest()) {
          return true;
        }

        // Contractions should also be lifted if the identifier from
        // 'LHS' occurs in contraction i.e. on 'RHS';
        // Note that for CPU this optimization
        // will produce more efficient code than lift identifiers
        // later in the 'IdentifierCopier'.
        IdentifierFinder IdFinder(Root);
        const IdentifierExpr *LHSId = RHSToLHSMap[Root];
        if (IdFinder.find(LHSId->getName())) {
          if (IdFinder.getIdIncompatible() || LHSId->getPermute()) {
            return true;
          }
        }
      }
      return false;
    };
    ExprTreeLifter ETLifter(CG, nodeLiftPredicate);
    ETLifter.transformAssignments();

    // 3.2 We lift out any expressions below contractions;
    // Note that in general, multiplication or division by a scalar
    // can probably remain under a contraction.
    const auto &subContrLiftPredicate = [](const ExpressionNode *Node,
                                           const ExpressionNode *Root) -> bool {

      // Note that 'ParentFinder' can help to lift out expressions below
      // contractions.
      ParentFinder PFinder(Root);
      auto *Parent = PFinder.find(Node);
      if (Parent && Parent->isContractionExpr()) {
        return true;
      }
      return false;
    };
    ExprTreeLifter SubContrLifter(CG, subContrLiftPredicate);
    SubContrLifter.transformAssignments();

    // 4. We need to copy identifiers if they appear on 'LHS' and 'RHS'
    // in incompatible ways.
    //
    // Note that 'IdentifierCopier' can help to lift identifiers out of
    // expression trees if they appear on both 'LHS' and 'RHS' in conflicting
    // ways, i.e. they come with different index structures.
    IdentifierCopier IdCopier(CG);
    IdCopier.transformAssignments();
  }
  // end of transformations

  const Sema &Sema = *getSema();

  const bool HasElementLoop = Sema.getElemInfo().Present;
  if (HasElementLoop) {
    if (OMPPragmas) {
      OMP_CG_INDENT(InitialNestLevel * OMP_INDENT_PER_LEVEL);
      appendCode("#pragma omp for\n");
    }

    ElementIndex = getIndex();
    emitForLoopHeader(InitialNestLevel * OMP_INDENT_PER_LEVEL, ElementIndex,
                      Sema.getElemInfo().Dim);
    ++InitialNestLevel;
  }

  bool Fuse = false;
  bool FuseNext = false;
  std::set<std::string> EmittedNames;
  auto AssignIt = CG->getAssignments().begin();
  auto AssignIE = CG->getAssignments().end();
  for (; AssignIt != AssignIE; AssignIt++) {
    const auto &NextAssign = std::next(AssignIt);

    assert(AssignIt->LHS->isIdentifier() && INTERNAL_ERROR
           "LHS must be an identifier expression");

    const IdentifierExpr *Result =
        static_cast<const IdentifierExpr *>(AssignIt->LHS);
    const std::string &ResultName = Result->getName();

    const IdentifierExpr *NextResult = nullptr;
    std::string NextResultName = "";

    if (NextAssign == CG->getAssignments().end()) {
      FuseNext = false;
    } else {
      NextResult = static_cast<const IdentifierExpr *>(NextAssign->LHS);
      NextResultName = NextResult->getName();
      FuseNext = (getDims(NextResult) == getDims(Result));
    }
    if (FuseNext) {
      ContrExprCounter ContractionExprCounter(NextAssign->RHS);
      ContractionExprCounter.run();
      FuseNext = FuseNext && (ContractionExprCounter.getCount() == 0);

      IdentifierFinder IdFinder(NextAssign->RHS);
      bool Found = IdFinder.find(ResultName);
      // Note that we cannot fuse if the next 'RHS' contains an incompatible
      // use of the result of the current assignement.
      if (Found && IdFinder.getIdIncompatible()) {
        FuseNext = false;
      }
      if (Result->getPermute() || NextResult->getPermute()) {
        FuseNext = false;
      }
    }

    if (!Fuse) {
      NestLevel = InitialNestLevel;
    }

    // Here we emit defintion of 'Result' if necessary.
    if (!Sema.is_in_inputs(ResultName) && !Sema.is_in_outputs(ResultName) &&
        !EmittedNames.count(ResultName)) {
      OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
      appendCode(getFloatPointTypeName() + " " + ResultName +
                 dimsString(getDims(Result)) + ";\n");

      EmittedNames.insert(ResultName);
    }
    // Here we need to emit the definition of next result if fused.
    if (FuseNext && !Sema.is_in_inputs(NextResultName) &&
        !Sema.is_in_outputs(NextResultName) &&
        !EmittedNames.count(NextResultName)) {
      OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
      appendCode(getFloatPointTypeName() + " " + NextResultName +
                 dimsString(getDims(NextResult)) + ";\n");

      EmittedNames.insert(NextResultName);
    }

    const ExpressionNode *EN = AssignIt->RHS;
    const std::vector<int> &Dims = getDims(EN);

    if (!Fuse) {
      // Note that here we need to generate enough indices for the expression.
      ExprIndices.clear();
      ResultIndices.clear();
      LoopedOverIndices.clear();
      for (int I = 0; I < Dims.size(); ++I) {
        const std::string Index = getIndex();
        ExprIndices.push_back(Index);
        ResultIndices.push_back(Index);
      }
    }

    setResultTmp(AssignIt->LHS);
    // Note that here we need this if branch stmt since code generation
    // for identifiers has been optimized out. We need to visit the top level
    // identifier.
    if (EN->isIdentifier()) {
      // However, as a result of transforming stack expressions,
      // assignments with nothing but an identifier on 'RHS' may
      // appear at top level.
      visitTopLevelIdentifier(EN);
    } else {
      EN->visit(this);
    }

    assert((NestLevel - InitialNestLevel) == LoopedOverIndices.size());

    if (FuseNext) {
      Fuse = true;
    }
    if (!FuseNext) {
      // Here we need to close all for loops and set the 'Fuse' state to false.
      emitLoopFooterNest();
      Fuse = false;
    }
    FuseNext = false;
  }

  if (HasElementLoop) {
    --InitialNestLevel;
    emitForLoopFooter(InitialNestLevel * OMP_INDENT_PER_LEVEL);
  }

  // Here we need to close block of function body.
  appendCode("}\n");

  // FIXME: remove wrapper for current kernel, function call may be need less.
  if (EmitWrapper) {
    const int NumArgs = getNumFuncArgs();
    assert(NumArgs == ArgumentsDimensions.size());

    appendCode("\n");
    const std::string &WrapperDef = "void " + getCGFuncName() + "(";
    appendCode(WrapperDef);
    for (int I = 0; I < NumArgs; ++I) {
      if (I != 0) {
        // Space or new line?
        appendCode(",\n");
        OMP_CG_INDENT(WrapperDef.length())
      }
      appendCode(getFloatPointTypeName() + " *arg" +
                 std::to_string((long long)I));
    }
    appendCode(") {\n");

    const std::string &FunctionCall = getCGFuncNameWrapped() + "(";
    // Here we need tp emit call of wrapper function 'FunctionName'.
    // FIXME: remove here.
    OMP_CG_INDENT(OMP_INDENT_PER_LEVEL)
    appendCode(FunctionCall);
    // Here we need to emit list of actual args of wrapper function.
    for (int I = 0; I < NumArgs; ++I) {
      if (I != 0) {
        appendCode(",\n");
        OMP_CG_INDENT(OMP_INDENT_PER_LEVEL + FunctionCall.length())
      }

      std::vector<int> Dims = ArgumentsDimensions[I];
      if (Dims.size() > 0) {
        Dims = RowMajor ? std::vector<int>(Dims.begin() + 1, Dims.end())
                        : std::vector<int>(Dims.begin(), Dims.end() - 1);
        if (Dims.size() > 0)
          appendCode("(" + getFloatPointTypeName() + "(*)" + dimsString(Dims) +
                     ")");
      } else {
        assert(Dims.size() == 0);
        // Note here we need to dereference the pointer
        appendCode("*");
      }

      appendCode("arg" + std::to_string((long long)I));
    }
    appendCode(");\n");

    appendCode("}\n");
  }
}

void OMPCG::emitSignature(std::vector<std::vector<int>> &ArgsDims) {
  // We need to emit function signature, and
  // populate args with the dimensions of function args
  const Sema &Sema = *getSema();

  bool IsFirstArgument = true;
  appendCode("void " + getCGFuncNameWrapped() + "(\n");

  // Note we need to record a dummy argument for 'updateWithElemInfo' method.
  std::vector<std::string> Dummy;

  // Here we need to emit 'in' marked variable as kernel function args.
  for (auto in = Sema.inputs_begin(); in != Sema.inputs_end(); in++) {
    const Symbol *Sym = *in;

    if (IsFirstArgument) {
      IsFirstArgument = false;
    } else {
      appendCode(",\n");
    }

    const std::string &ArgName = Sym->getName();
    std::vector<int> ArgDims = Sym->getType().getDims();
    updateWithElemInfo(Dummy, ArgDims, ArgName);

    // We need to indent each arg of the function.
    OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL)
    appendCode(getFloatPointTypeName() + " " + ArgName +
               dimsString(ArgDims, RestrictPointer));

    addFuncArg(ArgName);

    ArgsDims.push_back(ArgDims);
  }

  // Then we also need to emit 'out' marked variables as kernel function args.
  for (auto out = Sema.outputs_begin(); out != Sema.outputs_end(); out++) {
    const Symbol *Sym = *out;

    // Note that DO NOT re-emit input args, since they have been generated in
    // previous for-looop.
    if (Sema.is_in_inputs(Sym->getName())) {
      continue;
    }

    if (IsFirstArgument) {
      IsFirstArgument = false;
    } else {
      appendCode(",\n");
    }

    const std::string &ArgName = Sym->getName();
    std::vector<int> ArgDims = Sym->getType().getDims();
    updateWithElemInfo(Dummy, ArgDims, ArgName);

    // Then we indent each args
    OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL)
    appendCode(getFloatPointTypeName() + " " + ArgName +
               dimsString(ArgDims, RestrictPointer));

    addFuncArg(ArgName);

    ArgsDims.push_back(ArgDims);
  }

  // Here we finish to emitting the function signature.
  appendCode(")\n");
}

// This function is used to determine the maximum power of 2 for unrolling such
// that no more than two loop iterations must be peeled off the end of the loop.
// But this function is currently unused!
static int getUnrollFactor(const int Bound) {
  if (Bound <= 2)
    return 0;

  int Result = 2;
  for (int I = Result; I <= Bound; I *= 2) {
    if (Bound % I <= 2) {
      Result = I;
    }
  }
  return Result;
}

void OMPCG::emitForLoopHeader(unsigned Indent, const std::string &IndexVar,
                              const std::string &Bound) {
  OMP_CG_INDENT(Indent)
  appendCode("for (unsigned " + IndexVar + " = 0; " + IndexVar + " < " + Bound +
             "; " + "++" + IndexVar + ") {\n");
}

void OMPCG::emitForLoopHeader(unsigned Indent, const std::string &IndexVar,
                              int IntBound, bool Unroll, bool Simd) {
  if (Unroll && IccPragmas) {
  // Note that unroll factor is currently not used.
  // FIXME: here maybe wrong.
#if 0
    const int Factor = getUnrollFactor(IntBound);
    if (Factor > 0) {
      OMP_CG_INDENT(Indent)
      appendCode("#pragma unroll (" + std::to_string(Factor) + ")\n");
    }
#endif
    OMP_CG_INDENT(Indent)
    appendCode("#pragma unroll\n");
  }

  if (Simd && IccPragmas) {
    OMP_CG_INDENT(Indent)
    appendCode("#pragma simd\n");
  }
  if (Simd && OMPPragmas) {
    OMP_CG_INDENT(Indent)
    appendCode("#pragma omp simd\n");
  }
  emitForLoopHeader(Indent, IndexVar, std::to_string((long long)IntBound));
}

void OMPCG::emitForLoopFooter(unsigned Indent) {
  OMP_CG_INDENT(Indent)
  appendCode("}\n");
}

void OMPCG::emitTmpDefinition(unsigned Indent, const std::string &Tmp) {
  OMP_CG_INDENT(Indent)
  appendCode(getFloatPointTypeName() + " " + Tmp + ";\n");
}

std::string OMPCG::subscriptString(const std::vector<std::string> &Indices,
                                   const std::vector<int> &Dims) const {
  assert(Indices.size() == Dims.size());

  const int Rank = Dims.size();
  // If 'Rank' is 0, that means scalar.
  if (Rank == 0) {
    return "";
  }

  std::string Res = "";

  if (RowMajor) {
    for (int I = 0; I < Rank; ++I) {
      Res += "[" + Indices[I] + "]";
    }
  } else {
    for (int I = (Rank - 1); I >= 0; --I) {
      Res += "[" + Indices[I] + "]";
    }
  }

  return Res;
}

std::string OMPCG::dimsString(const std::vector<int> &Dims,
                              bool EmitRestrict) const {
  const int Rank = Dims.size();
  // Note that if 'Rank' is 0, that means scalar.
  if (Rank == 0) {
    return "";
  }

  std::string Res = "";

  if (RowMajor) {
    for (int I = 0; I < Rank; ++I) {
      const std::string RestrictQual =
          (EmitRestrict && (I == 0)) ? "restrict " : "";
      Res += "[" + RestrictQual + std::to_string(Dims[I]) + "]";
    }
  } else {
    for (int I = (Rank - 1); I >= 0; --I) {
      const std::string RestrictQual =
          (EmitRestrict && (I == (Rank - 1))) ? "restrict " : "";
      Res += "[" + RestrictQual + std::to_string(Dims[I]) + "]";
    }
  }
  return Res;
}

void OMPCG::updateWithElemInfo(std::vector<std::string> &Indices,
                               std::vector<int> &Dims,
                               const std::string &Name) const {
  const Sema &Sema = *getSema();
  const Sema::ElemInfo &Info = Sema.getElemInfo();

  if (!Info.Present) {
    return;
  }

  const Symbol *Sym = Sema.getSymbol(Name);
  if (Info.Syms.find(Sym) == Info.Syms.end()) {
    return;
  }

  if (Info.Pos == ElementDirective::POS_Last) {
    Dims.push_back(Info.Dim);
    Indices.push_back(ElementIndex);
  } else if (Info.Pos == ElementDirective::POS_First) {
    Indices.push_back("");
    for (unsigned I = Indices.size() - 1; I > 0; --I) {
      Indices[I] = Indices[I - 1];
    }
    Indices[0] = ElementIndex;

    Dims.push_back(0);
    for (unsigned I = Dims.size() - 1; I > 0; --I) {
      Dims[I] = Dims[I - 1];
    }
    Dims[0] = Info.Dim;
  } else {
    ph_unreachable(INTERNAL_ERROR "invalid position specifier");
  }
}

std::string
OMPCG::subscriptedIdentifier(const ExpressionNode *Node,
                             const std::vector<std::string> &Indices) const {
  assert(Node->isIdentifier());

  std::vector<std::string> AllIndices;
  for (unsigned I = 0; I < Node->getNumIndices(); ++I) {
    AllIndices.push_back(Node->getIndex(I));
  }
  for (unsigned I = 0; I < Indices.size(); ++I) {
    AllIndices.push_back(Indices.at(I));
  }

  const IdentifierExpr *Id = static_cast<const IdentifierExpr *>(Node);
  if (Id->getPermute()) {
    // Indices must be permuted going through transpositions forwards.
    // Note that when 'TranspositionExpr' is emitted on 'RHS' of
    // assignments, the index pairs are traversed backwards; Here the
    // permutations occur on 'LHS' of assignments, hence index pairs are
    // traversed forwards.
    const auto &IndexPairs = Id->getIndexPairs();
    for (auto PairIt = IndexPairs.begin(); PairIt != IndexPairs.end();
         ++PairIt) {
      const auto Pair = *PairIt;
      assert(Pair.size() == 2);
      const std::string Index = AllIndices[Pair[0]];
      AllIndices[Pair[0]] = AllIndices[Pair[1]];
      AllIndices[Pair[1]] = Index;
    }
  }

  std::vector<int> Dims = Node->getDims();
  updateWithElemInfo(AllIndices, Dims, Id->getName());
  return (Node->getName() + subscriptString(AllIndices, Dims));
}

void OMPCG::emitLoopHeaderNest(const std::vector<int> &ExprDims, bool Unroll,
                               bool Simd) {
  const int Rank = ExprIndices.size();

  if (RowMajor) {
    int IndexFirst = -1, IndexLast = -1;
    // Here we determine the first and last value of 'I',and at position 'I'
    // a new loop header will actually be emitted.
    for (int I = 0; I < Rank; ++I) {
      const std::string &Index = ExprIndices[I];
      if (LoopedOverIndices.find(Index) != LoopedOverIndices.end()) {
        continue;
      }
      if (IndexFirst == -1)
        IndexFirst = I;
      IndexLast = I;
    }

    for (int I = 0; I < Rank; ++I) {
      const std::string &Index = ExprIndices[I];
      if (LoopedOverIndices.find(Index) != LoopedOverIndices.end()) {
        // 'Index' is already iterated over in a for looop, so we need to skip
        // this case.
        continue;
      }
      // Here we emit 'Unroll' pragma only for the first looop header.
      // and emit 'Simd' pragma only for the last loop header, and only
      // if the first and last looop header do no coincide.
      // This helps to avoid having both pragmas before the same loop header.
      emitForLoopHeader(NestLevel * OMP_INDENT_PER_LEVEL, Index, ExprDims[I],
                        Unroll && (I == IndexFirst),
                        Simd && (I == IndexLast) && (IndexFirst != IndexLast));
      LoopedOverIndices.insert(Index);
      ++NestLevel;
    }
  } else {
    int IndexFirst = -1, IndexLast = -1;
    // Here we determine the first and last value of 'I',and at position 'I'
    // a new loop header will actually be emitted.
    for (int I = (Rank - 1); I >= 0; --I) {
      const std::string &Index = ExprIndices[I];
      if (LoopedOverIndices.find(Index) != LoopedOverIndices.end()) {
        continue;
      }
      if (IndexFirst == -1)
        IndexFirst = I;
      IndexLast = I;
    }
    for (int I = (Rank - 1); I >= 0; --I) {
      const std::string &Index = ExprIndices[I];
      if (LoopedOverIndices.find(Index) != LoopedOverIndices.end()) {
        // 'Index' is already iterated over in a for looop, so we need to skip
        // this case.
        continue;
      }
      // Note that here we emit 'Unroll' pragma only for the first looop header.
      // and emit 'simd' pragma only for the last loop header, and only
      // if the first and last looop header do no coincide.
      // This can help to avoid having both pragmas before the same loop header.
      // Another strategy is the following condition:
      //(Simd && (I == IndexLast)  && (ExprDims[i] % 4 == 0)) ||
      //(Simd && (I == IndexFirst) && (ExprDims[i] % 4 != 0)));
      emitForLoopHeader(NestLevel * OMP_INDENT_PER_LEVEL, Index, ExprDims[I],
                        Unroll && (I == IndexFirst),
                        (Simd && (I == IndexLast)));
      LoopedOverIndices.insert(Index);
      ++NestLevel;
    }
  }
}

void OMPCG::emitLoopFooterNest() {
  while (NestLevel > InitialNestLevel) {
    --NestLevel;
    emitForLoopFooter(NestLevel * OMP_INDENT_PER_LEVEL);
  }
}

std::string OMPCG::visitChildExpr(const ExpressionNode *Node) {
  if (Node->isIdentifier()) {
    return subscriptedIdentifier(Node, ExprIndices);
  } else {
    const std::string Tmp = getTmp();
    emitTmpDefinition(NestLevel * OMP_INDENT_PER_LEVEL, Tmp);
    ExpressionNode *TmpNode =
        CG->getExprNodeBuilder()->createIdentifierExpr(Tmp, {});

    std::vector<std::string> SavedResultIndices = ResultIndices;
    setResultTmp(TmpNode);
    ResultIndices = {};
    Node->visit(this);
    ResultIndices = SavedResultIndices;
    return subscriptedIdentifier(TmpNode);
  }
}

void OMPCG::visitBinOpExpr(const ExpressionNode *Node,
                           const std::string &Operation) {
  // Here 'BinOp' refers to element-wise and scalar tensor operations.
  // Hence these operations children's number must be 2.
  assert(Node->getNumChildren() == 2);
  const ExpressionNode *Result = getResultTmp();

  std::string LHSTmp, RHSTmp;

  const std::vector<std::string> SavedExprIndices = ExprIndices;
  const int SavedNestingLevel = NestLevel;

  const ExpressionNode *LHSExpr = Node->getChild(0);
  const ExpressionNode *RHSExpr = Node->getChild(1);

  const std::vector<int> &ExprDims = getDims(Node);
  const std::vector<int> &LHSDims = getDims(Node->getChild(0));
  const std::vector<int> &RHSDims = getDims(Node->getChild(1));

  std::vector<std::string> LHSIndices, RHSIndices;
  if (LHSDims.size() == 0 || RHSDims.size() == 0) {
    // If we get here that means one of the args of this 'BinOp' is a scalar,
    // hence, we need to split the 'ExprIndices' accordingly.
    for (int I = 0; I < LHSDims.size(); ++I) {
      const std::string &Index = ExprIndices[I];
      LHSIndices.push_back(Index);
    }
    for (int I = 0; I < RHSDims.size(); ++I) {
      const std::string &Index = ExprIndices[LHSDims.size() + I];
      RHSIndices.push_back(Index);
    }
  } else {
    assert(LHSDims.size() == RHSDims.size());
    // If we get here, that means both args of this 'BinOps' are tensors,
    // hence, the same 'ExprIndices' must be used for both args.
    LHSIndices = ExprIndices;
    RHSIndices = ExprIndices;
  }

  // Then we can emit 'LHS' and 'RHS'.
  ExprIndices = LHSIndices;
  LHSTmp = visitChildExpr(LHSExpr);
  ExprIndices = RHSIndices;
  RHSTmp = visitChildExpr(RHSExpr);

  // Then we need to emit the for looop nest.
  ExprIndices = SavedExprIndices;
  emitLoopHeaderNest(ExprDims, /*Unroll=*/false, /*Simd*/ true);

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  appendCode(subscriptedIdentifier(Result, ResultIndices) + " = " + LHSTmp +
             " " + Operation + " " + RHSTmp + ";\n");
}

void OMPCG::visitAddExpr(const AddExpr *E) { visitBinOpExpr(E, "+"); }

void OMPCG::visitSubExpr(const SubExpr *E) { visitBinOpExpr(E, "-"); }

void OMPCG::visitMulExpr(const MulExpr *E) { visitBinOpExpr(E, "*"); }

void OMPCG::visitDivExpr(const DivExpr *E) { visitBinOpExpr(E, "/"); }

void OMPCG::visitScalarMulExpr(const ScalarMulExpr *E) {
  visitBinOpExpr(E, "*");
}

void OMPCG::visitScalarDivExpr(const ScalarDivExpr *E) {
  visitBinOpExpr(E, "/");
}

void OMPCG::visitProductExpr(const ProductExpr *PE) {
  assert(PE->getNumChildren() == 2);

  const ExpressionNode *Result = getResultTmp();
  std::string LHSTmp, RHSTmp;

  const ExpressionNode *LHSExpr = PE->getChild(0);
  const ExpressionNode *RHSExpr = PE->getChild(1);

  const std::vector<int> &ExprDims = getDims(PE);
  const std::vector<int> &LHSDims = getDims(PE->getChild(0));
  const std::vector<int> &RHSDims = getDims(PE->getChild(1));

  std::vector<std::string> LHSIndices, RHSIndices;
  // Here we need to determine which indices belong to 'LHS' or 'RHS'.
  {
    for (int I = 0; I < LHSDims.size(); ++I) {
      const std::string &Index = ExprIndices[I];
      LHSIndices.push_back(Index);
    }
    for (int I = 0; I < RHSDims.size(); ++I) {
      const std::string &Index = ExprIndices[LHSDims.size() + I];
      RHSIndices.push_back(Index);
    }
  }

  const std::vector<std::string> SavedExprIndices = ExprIndices;
  // Here we visit 'LHS'.
  ExprIndices = LHSIndices;
  LHSTmp = visitChildExpr(LHSExpr);

  // Here we visit 'RHS' and emit looop headers as necessary.
  ExprIndices = RHSIndices;
  RHSTmp = visitChildExpr(RHSExpr);

  ExprIndices = SavedExprIndices;
  emitLoopHeaderNest(ExprDims);

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  appendCode(subscriptedIdentifier(Result, ResultIndices) + " = " + LHSTmp +
             " * " + RHSTmp + ";\n");
}

void OMPCG::visitContractionExpr(const ContractionExpr *ContrExpr) {
  assert(ContrExpr->getNumChildren() == 2);

  const ExpressionNode *Result = getResultTmp();
  std::string LHSTmp, RHSTmp;
  const std::string Tmp = getTmp();

  // Here 'LHS' and 'RHS' for contraction expression means its two operand.
  const ExpressionNode *LHSExpr = ContrExpr->getChild(0);
  const ExpressionNode *RHSExpr = ContrExpr->getChild(1);

  const std::vector<int> &ExprDims = getDims(ContrExpr);
  const std::vector<int> &LHSDims = getDims(ContrExpr->getChild(0));
  const std::vector<int> &RHSDims = getDims(ContrExpr->getChild(1));

  std::vector<std::string> ContrIndices;
  for (int I = 0; I < ContrExpr->getLeftIndices().size(); ++I) {
    const std::string &Index = getIndex() + "_contr";
    ContrIndices.push_back(Index);
  }

  std::vector<std::string> LHSIndices, RHSIndices;
  int ExprIndexCounter = 0;
  // Here we need to determine which indices belong to 'LHS', i.e. the first
  // operand.
  {
    int ContrIndexCounter = 0;
    for (int I = 0; I < LHSDims.size(); ++I) {
      std::string Index;
      if (ContrIndexCounter < ContrExpr->getLeftIndices().size() &&
          I == ContrExpr->getLeftIndices()[ContrIndexCounter]) {
        Index = ContrIndices[ContrIndexCounter++];
      } else {
        Index = ExprIndices[ExprIndexCounter++];
      }
      LHSIndices.push_back(Index);
    }
  }
  // Here we need to determine which indices belong to 'RHS', i.e. the second
  // operand.
  {
    int ContrIndexCounter = 0;
    for (int I = 0; I < RHSDims.size(); ++I) {
      std::string Index;
      if (ContrIndexCounter < ContrExpr->getRightIndices().size() &&
          I == ContrExpr->getRightIndices()[ContrIndexCounter]) {
        Index = ContrIndices[ContrIndexCounter++];
      } else {
        Index = ExprIndices[ExprIndexCounter++];
      }
      RHSIndices.push_back(Index);
    }
  }

  // Here we need to emit for loop nest for result.
  emitLoopHeaderNest(ExprDims, /*Unroll=*/false, /*Simd=*/true);

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  // FIXME: for different we need to determine the zero format for different
  // kind float point numbers.
  appendCode(getFloatPointTypeName() + " " + Tmp + " = 0.0;\n");

  const std::vector<std::string> SavedExprIndices = ExprIndices;
  // Here we visit 'LHS'.
  ExprIndices = LHSIndices;
  LHSTmp = visitChildExpr(LHSExpr);
  // Here we emit for loop nest for 'LHS' early.
  emitLoopHeaderNest(LHSDims, /*Unroll=*/false, /*Simd=*/false);
  // Here we visit 'RHS' and emit loop headers as necessary.
  ExprIndices = RHSIndices;
  RHSTmp = visitChildExpr(RHSExpr);
  // Here we emit for loop nest for 'RHS'.
  emitLoopHeaderNest(RHSDims, /*Unroll=*/false, /*Simd=*/false);

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  appendCode(Tmp + " += " + LHSTmp + " * " + RHSTmp + ";\n");

  // 1. We need to close those for loops that iterate over those contracted
  // indices.
  for (int I = 0; I < ContrIndices.size(); ++I) {
    --NestLevel;
    emitForLoopFooter(NestLevel * OMP_INDENT_PER_LEVEL);
  }

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  appendCode(subscriptedIdentifier(Result, ResultIndices) + " = " + Tmp +
             ";\n");

  // 2. Then we need to remove contracted indices from 'LoopedOverIndices'.
  for (int I = 0; I < ContrIndices.size(); ++I) {
    LoopedOverIndices.erase(ContrIndices[I]);
  }

  ExprIndices = SavedExprIndices;
}

void OMPCG::visitStackExpr(const StackExpr *Expr) {
  // FIXME: support more general tensor stack expressions.
  // Note that current implementation of tensor stack operation yields correct
  // results at the precondition that ONLY if it emits code at the top level,
  // i.e. there is no nesting in any for loops. assert(NestLevel ==
  // InitialNestLevel);

  // Note that since this method has been called from top level, hence
  // 'ExprIndices' and 'ResultIndices' must agree assert(ExprIndices ==
  // ResultIndices);

  const ExpressionNode *Result = getResultTmp();
  const std::vector<std::string> SavedExprIndices = ExprIndices;
  const int SavedNestingLevel = NestLevel;

  const std::vector<int> &Dims = getDims(Expr);

  const std::string &FirstResultIndex = SavedExprIndices[0];
  std::vector<std::string> ChildExprIndices;
  // Note that here we need to skip the first index, so 'I' start from 1.
  for (int I = 1; I < SavedExprIndices.size(); ++I) {
    ChildExprIndices.push_back(SavedExprIndices[I]);
  }
  ExprIndices = ChildExprIndices;
  for (int C = 0; C < Expr->getNumChildren(); ++C) {
    // Here we need to replace the first index in 'Result' with the constant
    // 'C'.
    IdentifierExpr *Id = CG->getExprNodeBuilder()->createIdentifierExpr(
        Result->getName(), Result->getDims());
    for (unsigned I = 0; I < Result->getNumIndices(); ++I) {
      Id->addIndex(Result->getIndex(I));
    }
    Id->addIndex(std::to_string((long long)C));

    const ExpressionNode *Child = Expr->getChild(C);
    const std::vector<int> ChildDims = getDims(Child);

    ExprIndices = ChildExprIndices;
    std::string Tmp = visitChildExpr(Child);
    emitLoopHeaderNest(ChildDims);

    OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
    appendCode(subscriptedIdentifier(Id, ChildExprIndices) + " = " + Tmp +
               ";\n");

    // 1. We need to close for loops that iterated over 'ChildExprIndices'.
    while (NestLevel > SavedNestingLevel) {
      --NestLevel;
      emitForLoopFooter(NestLevel * OMP_INDENT_PER_LEVEL);
    }
    // 2. Then we need tp remove loop indices from 'LoopedOverIndices'.
    for (int I = 0; I < ChildExprIndices.size(); ++I) {
      LoopedOverIndices.erase(ExprIndices[I]);
    }
  }
  ExprIndices = SavedExprIndices;
}

void OMPCG::visitTranspositionExpr(const TranspositionExpr *TransExpr) {
  const std::vector<std::string> SavedExprIndices = ExprIndices;
  // const int SavedNestingLevel = NestLevel;
  // emit loop header before transposing indices
  emitLoopHeaderNest(getDims(TransExpr), /*Unroll=*/true, /*Simd=*/true);

  std::vector<std::string> TransposedExprIndices = ExprIndices;
  const std::vector<std::vector<int>> &IndexPairs = TransExpr->getIndexPairs();
  // We need to traverse index pairs in reverse order and apply transpositions.
  // Note that previously, e.g. in 'Sema', index pairs are traversed in order to
  // synthesize the type of an expression in an bottom-up manner; now we go in
  // an top-down manner to construct the 'ExprIndices'.
  for (auto PairIt = IndexPairs.rbegin(); PairIt != IndexPairs.rend();
       PairIt++) {
    const auto Pair = *PairIt;
    // For transpostion operation, each pair is the 'from' and 'to' index.
    assert(Pair.size() == 2);
    const std::string Index = TransposedExprIndices[Pair[0]];
    TransposedExprIndices[Pair[0]] = TransposedExprIndices[Pair[1]];
    TransposedExprIndices[Pair[1]] = Index;
  }

  ExprIndices = TransposedExprIndices;
  const ExpressionNode *Child = TransExpr->getChild(0);
  const ExpressionNode *Result = getResultTmp();

  if (Child->isIdentifier()) {
    const IdentifierExpr *Id = static_cast<const IdentifierExpr *>(Child);
    assert(!Id->getPermute() &&
           "internal error: only identifiers on the 'lhs' may be transposed");
    OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
    appendCode(subscriptedIdentifier(Result, ResultIndices) + " = " +
               subscriptedIdentifier(Child, ExprIndices) + ";\n");
  } else {
    Child->visit(this);
  }
  ExprIndices = SavedExprIndices;
}

void OMPCG::visitTopLevelIdentifier(const ExpressionNode *Id) {
  // Note that this function is only allowed to be called from the top level
  assert(NestLevel == InitialNestLevel);
  const ExpressionNode *Result = getResultTmp();
  const int SavedNestingLevel = NestLevel;
  const std::vector<int> &Dims = getDims(Id);
  emitLoopHeaderNest(Dims, /*Unroll=*/true, /*Simd=*/true);

  OMP_CG_INDENT(NestLevel * OMP_INDENT_PER_LEVEL);
  appendCode(subscriptedIdentifier(Result, ExprIndices) + " = " +
             subscriptedIdentifier(Id, ExprIndices) + ";\n");
}

void OMPCG::visitIdentifierExpr(const IdentifierExpr *Id) {
  ph_unreachable(INTERNAL_ERROR
                 "code generation for identifier has been optimized out");
}
