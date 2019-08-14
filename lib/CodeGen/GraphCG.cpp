//==--- GraphCG.cpp ----- Interface to code generation via graph -----------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file implements the GraphCodeGen class.
//
//===----------------------------------------------------------------------===//

#include "ph/CodeGen/GraphCG.h"
#include "ph/Opt/ENBuilder.h"
#include "ph/Opt/TensorExprTree.h"
#include "ph/Support/ErrorHandling.h"

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h> // mkdtemp

#define TMP_STRING_TEMPLATE "/tmp/phaeton-XXXXXXXXX"

using namespace phaeton;

void GraphCodeGen::updateCurrentEnd(GraphCGNode *Node) {
  if (CurrentEnd != nullptr) {
    CurrentEnd->setSucc(Node);
    Node->setPred(CurrentEnd);
  }
  CurrentEnd = Node;
  return;
}

GraphCodeGen::GraphCodeGen(const Sema *S, const std::string &FuncName)
    : CodeGen(S, FuncName), CurrentGraph(nullptr), CurrentEnd(nullptr) {
  CurrentLegs.clear();
}

void GraphCodeGen::visitStmt(const Stmt *S) {
  buildExprTreeForExpr(S->getExpr());
  CodeGen::visitStmt(S);
}

void GraphCodeGen::buildExprTreeForExpr(const Expr *E) {
  GraphCGGraph *SavedGraph = CurrentGraph;
  GraphCGLegs SavedLegs = CurrentLegs;
  GraphCGNode *SavedEnd = CurrentEnd;
  {
    GraphCGGraph TempGraph;
    CurrentGraph = &TempGraph;
    CurrentLegs.clear();
    CurrentEnd = nullptr;
    // Here we builds the graph for Expr 'E' into 'CurrentGraph'.
    E->visit(this);
    addExprNode(E, buildExprTreeForGraph(CurrentGraph));
  }
  CurrentGraph = SavedGraph;
  CurrentLegs = SavedLegs;
  CurrentEnd = SavedEnd;
}

void GraphCodeGen::visitIdentifier(const Identifier *Id) {
  const Sema &S = *getSema();

  const std::string &Name = Id->getName();
  const TensorType *Type = S.getType(Id);
  const int Rank = Type->getRank();

  ExprNode *ResNode = ENBuilder->createIdentifierExpr(Name, Type->getDims());
  GraphCGNode *Node = CurrentGraph->getNode(NodeID(ResNode, Name, Id), Rank);

  for (int I = 0; I < Rank; ++I) {
    CurrentLegs.push_back(GraphCGEdge::NodeIndexPair(Node, I));
  }
  updateCurrentEnd(Node);
}

void GraphCodeGen::visitInteger(const Integer *I) {
  ph_unreachable(INTERNAL_ERROR
                 "integer should not be visited for graph code generation");
}

void GraphCodeGen::visitParenExpr(const ParenExpr *PE) {
  const Expr *E = PE->getExpr();
  PE->visit(this);
}

void GraphCodeGen::visitBrackExpr(const BrackExpr *BE) {
  std::vector<ExprNode *> Members;

  const ExprList &Exprs = *BE->getExprs();
  for (unsigned I = 0; I < Exprs.size(); ++I) {
    buildExprTreeForExpr(Exprs[I]);
    assertExprTreeMap(Exprs[I]);

    ExprNode *Node = getExprNode(Exprs[I]);
    Members.push_back(Node);
  }

  ExprNode *ResNode = ENBuilder->createStackExpr(Members);
  addExprNode(BE, ResNode);

  const TensorType *Type = getSema()->getType(BE);
  const int Rank = Type->getRank();
  GraphCGNode *Node =
      CurrentGraph->getNode(NodeID(ResNode, /*Label =*/"stack", BE), Rank);

  for (int I = 0; I < Rank; ++I)
    CurrentLegs.push_back(GraphCGEdge::NodeIndexPair(Node, I));

  updateCurrentEnd(Node);
}

void GraphCodeGen::visitBinaryExpr(const BinaryExpr *BE) {
  const Sema &S = *getSema();
  const ASTNode::ASTNodeKind NK = BE->getASTNodeKind();

  // Handle contraction expression.
  if (NK == ASTNode::AST_NODE_KIND_ContractionExpr) {
    TupleList ContrList;
    if (S.isListOfLists(BE->getRight(), ContrList)) {
      const BinaryExpr *TensorExpr = extractTensorExprOrNull(BE->getLeft());
      if (!TensorExpr)
        ph_unreachable(INTERNAL_ERROR "cannot handle general contractions yet");

      if (ContrList.empty())
        ph_unreachable(INTERNAL_ERROR "cannot have an empty list here");

      visitContraction(TensorExpr, ContrList);
    } else {
      const Expr *Left = BE->getLeft();
      buildExprTreeForExpr(Left);
      assertExprTreeMap(Left);

      const Expr *Right = BE->getRight();
      buildExprTreeForExpr(Right);
      assertExprTreeMap(Right);

      ExprNode *LHS = getExprNode(Left);
      ExprNode *RHS = getExprNode(Right);

      const TensorType *LeftType = S.getType(BE->getLeft());
      const int LeftRank = LeftType->getRank();
      ExprNode *ResNode =
          ENBuilder->createContractionExpr(LHS, {LeftRank - 1}, RHS, {0});
      addExprNode(BE, ResNode);

      const TensorType *Type = S.getType(BE);
      const int Rank = Type->getRank();
      GraphCGNode *Node =
          CurrentGraph->getNode(NodeID(ResNode, /*Lable=*/".", BE), Rank);

      for (int I = 0; I < Rank; ++I)
        CurrentLegs.push_back(GraphCGEdge::NodeIndexPair(Node, I));

      updateCurrentEnd(Node);
    }
    return;
  } else if (NK == ASTNode::AST_NODE_KIND_ProductExpr) {
    // Handle product expression.
    // Note that here we simply add nodes to 'CurrentGraph'
    BE->getLeft()->visit(this);
    BE->getRight()->visit(this);
    return;
  } else if (NK == ASTNode::AST_NODE_KIND_TranspositionExpr) {
    // Hanle transposition expression.
    const Expr *Left = BE->getLeft();
    // Note that here we need a single expression node at which the
    // expression tree for Expr 'Left' is rooted
    buildExprTreeForExpr(Left);

    TupleList IndexPairs;
    if (!Sema::isListOfLists(BE->getRight(), IndexPairs))
      ph_unreachable(INTERNAL_ERROR "right of transposition is not a list");

    if (IndexPairs.empty())
      ph_unreachable(INTERNAL_ERROR "cannot have an empty list here");

    ExprNode *ResNode =
        ENBuilder->createTranspositionExpr(getExprNode(Left), IndexPairs);
    addExprNode(BE, ResNode);

    const TensorType *Type = S.getType(BE);
    const int Rank = Type->getRank();
    GraphCGNode *Node =
        CurrentGraph->getNode(NodeID(ResNode, /*Label=*/"transpose", BE), Rank);

    for (int I = 0; I < Rank; ++I) {
      CurrentLegs.push_back(GraphCGEdge::NodeIndexPair(Node, I));
    }
    updateCurrentEnd(Node);
    return;
  }

  // If we get here that means contraction, product and transposition NOT handle
  // properly, an internal error occurs.
  assert(NK != ASTNode::AST_NODE_KIND_ContractionExpr &&
         NK != ASTNode::AST_NODE_KIND_ProductExpr &&
         NK != ASTNode::AST_NODE_KIND_TranspositionExpr &&
         INTERNAL_ERROR "should not be here");

  // Handle element-wise expression.
  const Expr *Left = BE->getLeft();
  buildExprTreeForExpr(Left);
  assertExprTreeMap(Left);

  const Expr *Right = BE->getRight();
  buildExprTreeForExpr(Right);
  assertExprTreeMap(Right);

  ExprNode *ResNode;
  ExprNode *LHS = getExprNode(Left);
  ExprNode *RHS = getExprNode(Right);
  std::string OperatorLabel;
  switch (NK) {
  case ASTNode::AST_NODE_KIND_AddExpr:
    ResNode = ENBuilder->createAddExpr(LHS, RHS);
    OperatorLabel = "+";
    break;
  case ASTNode::AST_NODE_KIND_SubExpr:
    ResNode = ENBuilder->createSubExpr(LHS, RHS);
    OperatorLabel = "-";
    break;
  case ASTNode::AST_NODE_KIND_MulExpr:
    if (S.isScalar(*S.getType(Left)))
      ResNode = ENBuilder->createScalarMulExpr(LHS, RHS);
    else
      ResNode = ENBuilder->createMulExpr(LHS, RHS);
    OperatorLabel = "*";
    break;
  case ASTNode::AST_NODE_KIND_DivExpr:
    if (S.isScalar(*S.getType(Right)))
      ResNode = ENBuilder->createScalarDivExpr(LHS, RHS);
    else
      ResNode = ENBuilder->createDivExpr(LHS, RHS);
    OperatorLabel = "/";
    break;
  default:
    ph_unreachable(INTERNAL_ERROR "invalid binary expression");
  }

  addExprNode(BE, ResNode);

  const TensorType *Type = S.getType(BE);
  const int Rank = Type->getRank();
  GraphCGNode *Node =
      CurrentGraph->getNode(NodeID(ResNode, OperatorLabel, BE), Rank);

  for (int I = 0; I < Rank; ++I)
    CurrentLegs.push_back(GraphCGEdge::NodeIndexPair(Node, I));

  updateCurrentEnd(Node);
}

void GraphCodeGen::visitContraction(const Expr *E, const TupleList &Index) {
  if (Index.empty()) {
    E->visit(this);
    return;
  }

  const BinaryExpr *TensorExpr = extractTensorExprOrNull(E);
  if (!TensorExpr)
    ph_unreachable(INTERNAL_ERROR "cannot handle general contractions yet");

  if (!isPairList(Index))
    ph_unreachable(INTERNAL_ERROR "only pairs of indices can be contracted");

  const Expr *TensorLeft = TensorExpr->getLeft();
  const Expr *TensorRight = TensorExpr->getRight();
  const TensorType *TypeLeft = getSema()->getType(TensorLeft);
  int RankLeft = TypeLeft->getRank();

  TupleList ContrLeft, ContrRight, ContrMixed;
  // Classify the index pairs into the following three categories:
  // - 'ContrLeft', contractions of the left sub-expression;
  // - 'ContrRight', contractions of the right sub-expression;
  // - 'ContrMixed', means having one index from each sub-expression
  partitionPairList(RankLeft, Index, ContrLeft, ContrRight, ContrMixed);

  GraphCGLegs SavedLegs = CurrentLegs;
  CurrentLegs.clear();
  visitContraction(TensorLeft, ContrLeft);

  // Note: here we determine the rank of the result left sub-expression after
  // contraction has been performed over the set of index pairs 'contrLeft'.
  int RankContractedLeft = RankLeft - 2 * ContrLeft.size();

  // Note that the index pairs of the right sub-expression must be adjusted by
  // the rank of the left sub-expression.
  TupleList ShiftedRight = ContrRight;
  shiftTupleList(-RankContractedLeft, ShiftedRight);

  visitContraction(TensorRight, ShiftedRight);

  if (ContrMixed.empty())
    return;

  List IndexLeft, IndexRight;
  unpackPairList(ContrMixed, IndexLeft, IndexRight);
  // Note that only contractions in 'ContrLeft' affect the adjustments
  // of the left indices in 'IndexLeft'.
  adjustForContractions(IndexLeft, ContrLeft);
  // Note that adjustment of right indices in 'IndexRight' are affected by
  // the contractions in both 'ContrLeft' and 'ContrRight'.
  adjustForContractions(IndexRight, ContrLeft);
  adjustForContractions(IndexRight, ContrRight);

  assert(IndexLeft.size() == IndexRight.size() && INTERNAL_ERROR
         "mismatched numbers of indices to be contracted");

  for (int I = 0; I < IndexLeft.size(); ++I) {
    int IndLeft = IndexLeft[I];
    int IndRight = IndexRight[I];

    // For contraction, the 'srcNode' node correcponds to the left tensor in the
    // contraction.
    const GraphCGNode *SrcNode = CurrentLegs[IndLeft].first;
    const int SrcIndex = CurrentLegs[IndLeft].second;
    const GraphCGNode *TargetNode = CurrentLegs[IndRight].first;
    const int TargetIndex = CurrentLegs[IndRight].second;

    std::stringstream StrStreamLabel;
    StrStreamLabel << "(" << SrcNode->getID().getLabel() << ":" << SrcIndex
                   << " -- " << TargetNode->getID().getLabel() << ":"
                   << TargetIndex << ")";

    bool Success =
        CurrentGraph->addEdge(EdgeID(getTemp(), StrStreamLabel.str(), E),
                              SrcNode, SrcIndex, TargetNode, TargetIndex);
    assert(Success && INTERNAL_ERROR "should not fail to add edge");
  }

  int Erased = 0;
  for (int I = 0; I < IndexLeft.size(); ++I)
    CurrentLegs.erase(CurrentLegs.begin() + IndexLeft[I] - (Erased++));
  for (int I = 0; I < IndexRight.size(); ++I)
    CurrentLegs.erase(CurrentLegs.begin() + IndexRight[I] - (Erased++));

  for (int I = 0; I < CurrentLegs.size(); ++I)
    SavedLegs.push_back(CurrentLegs[I]);
  CurrentLegs = SavedLegs;
}

void GraphCodeGen::dump(const GraphCGGraph &Graph) {
  /// Note: warning: the use of `tmpnam' is dangerous, better use `mkstemp'
  /// std::tmpnam is dangerous, here we use 'mkstemp' in <unistd.h>.
  char TmpDirName[L_tmpnam];
  mkdtemp(strcpy(TmpDirName, TMP_STRING_TEMPLATE));
  const std::string TempFileName = std::string(TmpDirName) + std::string("phaeton.dot");
  // TODO: add a wrapper for IO stream.
  std::ofstream OS(TempFileName);
  std::cout << "Writing graph to file \'" << TempFileName << "\' ... \n";
  Graph.plot(OS);
  OS.close();
}

ExprNode *GraphCodeGen::buildExprTreeForGraph(GraphCGGraph *Graph) {
  bool Success;
  while (Graph->getNumEdges()) {
    EdgeSet EdgesToContract;
    selectEdgesToContract(EdgesToContract, *Graph);
    assert(!EdgesToContract.empty() && INTERNAL_ERROR
           "graph should still have edges");

    const GraphCGEdge *FirstEdge = *EdgesToContract.begin();
    GraphCGNode &Src = *Graph->getNode(FirstEdge->getSrcID());
    GraphCGNode &Target = *Graph->getNode(FirstEdge->getTargetID());

    List SrcIndices, TargetIndices;
    for (const auto *E : EdgesToContract) {
      const GraphCGNode &EdgeSrc = *E->getSrcNode();
      const GraphCGNode &EdgeTarget = *E->getTargetNode();

      // Note that the order here matters, 'Src' node carries lower indices.
      assert((EdgeSrc == Src && EdgeTarget == Target));
      SrcIndices.push_back(E->getSrcIndex());
      TargetIndices.push_back(E->getTargetIndex());
    }

    ExprNode *ResNode = ENBuilder->createContractionExpr(
        Src.getID().get(), SrcIndices, Target.getID().get(), TargetIndices);

    // Here we find edges that remain at 'Src' node or 'Target' node after the
    // contraction.
    EdgeSet EdgesAtSrc, EdgesAtTarget;
    getRemainingEdgesAtNode(EdgesAtSrc, Src, EdgesToContract);
    getRemainingEdgesAtNode(EdgesAtTarget, Target, EdgesToContract);

    int Rank = Src.getRank() + Target.getRank() - 2 * EdgesToContract.size();
    GraphCGNode *Node =
        Graph->getNode(NodeID(ResNode, /*Label=*/"contraction"), Rank);
    Node->updateSequence(&Src, &Target);

    replaceEdgesAtNode(*Graph, Src, EdgesAtSrc, *Node, 0, EdgesToContract);
    replaceEdgesAtNode(*Graph, Target, EdgesAtTarget, *Node,
                       Src.getRank() - EdgesToContract.size(), EdgesToContract);

    // Here we erase all edges that have been contracted over.
    for (const auto *E : EdgesToContract)
      Graph->eraseEdge(E->getID());

    // Here we erase the nodes that are contracted.
    Success = Graph->eraseNode(Src.getID());
    assert(Success && INTERNAL_ERROR "should not fail to erase source node");
    Success = Graph->eraseNode(Target.getID());
    assert(Success && INTERNAL_ERROR "should not fail to erase target node");
  }

  // Here graph has no edges left, hence, form the tensor product
  // of the remaining nodes from left to right.
  const GraphCGNode *Node = Graph->getStartNode();
  ExprNode *ResNode = Node->getID().get();
  while (Node->hasSucc()) {
    const GraphCGNode *Succ = Node->getSucc();
    ResNode = ENBuilder->createProductExpr(ResNode, Succ->getID().get());
    Node = Succ;
  }

  return ResNode;
}

void GraphCodeGen::getRemainingEdgesAtNode(
    EdgeSet &Res, const GraphCGNode &Node,
    const EdgeSet &EdgesToContract) const {
  for (int I = 0; I < Node.getRank(); ++I) {
    if (!Node.isSet(I)) {
      continue;
    }
    const GraphCGEdge *Edge = Node.at(I);
    if (EdgesToContract.count(Edge)) {
      continue;
    }
    Res.insert(Edge);
  }
}

void GraphCodeGen::replaceEdgesAtNode(GraphCGGraph &Graph,
                                      const GraphCGNode &Old,
                                      const EdgeSet &EdgesAtOldNode,
                                      const GraphCGNode &New, int Shift,
                                      const EdgeSet &EdgesToContract) {
  // Helper function for contraction index adjustment.
  std::function<int(int)> adjustForContractions =
      [Old, EdgesToContract](int Index) -> int {
    int Adj = 0;
    for (const auto *Edge : EdgesToContract) {
      int OldIndex = (Old == *Edge->getSrcNode()) ? Edge->getSrcIndex()
                                                  : Edge->getTargetIndex();
      Adj += (OldIndex < Index);
    }
    return Index - Adj;
  };

  for (const auto *Edge : EdgesAtOldNode) {
    assert(Old == *Edge->getSrcNode() || Old == *Edge->getTargetNode());

    const GraphCGNode *NewSrcNode =
        (Old == *Edge->getSrcNode()) ? &New : Edge->getSrcNode();
    const int NewSrcIndex =
        (Old == *Edge->getSrcNode())
            ? adjustForContractions(Edge->getSrcIndex()) + Shift
            : Edge->getSrcIndex();

    const GraphCGNode *NewTargetNode =
        (Old == *Edge->getTargetNode()) ? &New : Edge->getTargetNode();
    const int NewTargetIndex =
        (Old == *Edge->getTargetNode())
            ? adjustForContractions(Edge->getTargetIndex()) + Shift
            : Edge->getTargetIndex();

    std::stringstream StrStreamLabel;
    StrStreamLabel << "(" << NewSrcNode->getID().getLabel() << ":"
                   << NewSrcIndex << " -- " << NewTargetNode->getID().getLabel()
                   << ":" << NewTargetIndex << ")";

    GraphCGEdge NewEdge(EdgeID(getTemp(), StrStreamLabel.str()), NewSrcNode,
                        NewSrcIndex, NewTargetNode, NewTargetIndex);

    bool Success = Graph.eraseEdge(Edge->getID());
    assert(Success && INTERNAL_ERROR "should not fail to erase edge");
    Success = Graph.addEdge(NewEdge);
    assert(Success && INTERNAL_ERROR "should not fail to add edge");
  }
}

void GraphCodeGen::selectEdgesToContract(EdgeSet &Result,
                                         const GraphCGGraph &Graph) const {
  if (!Graph.getNumEdges()) {
    return;
  }

  // Try to find edges that connect pred and succ node.
  const GraphCGNode *Node = Graph.getStartNode();
  GraphCGGraph::EdgeMap Tmp;

  while (Node->hasSucc()) {
    const GraphCGNode *Succ = Node->getSucc();

    Tmp.clear();
    Graph.getEdgesBetweenNodes(Tmp, Node, Succ);
    if (!Tmp.empty()) {
      break;
    }
    Node = Succ;
  }

  // FIXME: Now we cannot yet handle graphs with no contractions between
  // pred and succ node pairs.
  assert(!Tmp.empty() && INTERNAL_ERROR
         "cannot handle malformed contraction yet");

  for (const auto &It : Tmp) {
    Result.insert(It.second);
  }
}
