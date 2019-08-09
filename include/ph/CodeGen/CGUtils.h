//===--- CGUtils.h ------ CodeGen Helper Functions --------------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief This file provides some common utility functions for code generation.
/// This file also defines helper methods for GraphCG. Tensor graph is used for
/// high order tensor contract calculation, this idea is inspired by:
/// https://liwt31.github.io/2018/01/22/graphical_matrix
///
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_CGUTILS_H
#define PHAETON_CODEGEN_CGUTILS_H

#include "ph/AST/AST.h"

#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace phaeton {

/// Comparison - This class provides common helper operatror
/// functions(interface) for comparsion.
template <typename Derived> class Comparison {
public:
  virtual bool operator==(const Derived &RHS) const = 0;
  virtual bool operator!=(const Derived &RHS) const { return !(*this == RHS); }
  virtual bool operator<(const Derived &RHS) const = 0;
  virtual bool operator<=(const Derived &RHS) const {
    return (*this < RHS) || (*this == RHS);
  }
  virtual bool operator>=(const Derived &RHS) const { return !(*this < RHS); }
  virtual bool operator>(const Derived &RHS) const {
    return (*this >= RHS) && (*this != RHS);
  }
};

/// GraphComponentID - Associate with AST nodes.
template <typename Derived>
class GraphComponentID : public Comparison<Derived> {
public:
  GraphComponentID(const std::string Label = "", const ASTNode *Node = nullptr)
      : Label(Label), AST(Node) {}

  const std::string &getLabel() const { return Label; }
  const ASTNode *getAST() const { return AST; }
  virtual const std::string str() const = 0;

private:
  const std::string Label;
  // corresponding AST node.
  const ASTNode *AST;
};

class StringID : public GraphComponentID<StringID> {
public:
  StringID(const std::string &Id, const std::string Label = "",
           const ASTNode *Node = nullptr)
      : GraphComponentID<StringID>(Label, Node), ID(Id) {}

  StringID(const StringID &RHS)
      : GraphComponentID<StringID>(RHS.getLabel(), RHS.getAST()),
        ID(RHS.str()) {}

  virtual const std::string str() const final { return ID; }
  virtual bool operator==(const StringID &StrId) const final {
    return ID == StrId.str();
  }
  virtual bool operator<(const StringID &StrId) const final {
    return ID < StrId.str();
  }

private:
  const std::string ID;
};

template <typename T> class AddressID : public GraphComponentID<AddressID<T>> {
public:
  AddressID(T *Id, const std::string Label = "", const ASTNode *Node = nullptr)
      : GraphComponentID<AddressID>(Label, Node), ID(Id) {}

  AddressID(const AddressID &RHS, const std::string Label = "")
      : GraphComponentID<AddressID>(RHS.getLabel(), RHS.getAST()),
        ID(RHS.get()) {}

  virtual const std::string str() const final {
    std::stringstream StrStream;
    StrStream << std::hex << ID;
    return StrStream.str();
  }

  virtual bool operator==(const AddressID &AddrId) const final {
    return ID == AddrId.get();
  }

  virtual bool operator<(const AddressID &AddrId) const final {
    return ID < AddrId.get();
  }

  T *get() const { return ID; }

private:
  T *ID;
};

template <typename NodeID, typename EdgeID> class GraphEdge;
template <typename NodeID, typename EdgeID> class TensorGraph;

template <typename NodeID, typename EdgeID>
class GraphNode : public Comparison<GraphNode<NodeID, EdgeID>> {
public:
  GraphNode(NodeID &&Id, int Rank);

  const NodeID &getID() const { return ID; }
  NodeID &getID() { return ID; }
  int getRank() const { return Rank; }

  const GraphEdge<NodeID, EdgeID> *const &at(int I) const;
  const GraphEdge<NodeID, EdgeID> *&operator[](int I);

  int countSet() const;
  void unset(int I);

  bool isSet(int I) const;
  bool isUnset(int I) const { return !isSet(I); }
  bool anySet() const;
  bool anyUnset() const;
  bool allSet() const { return !anyUnset(); }
  bool allUnset() const { return !anySet(); }

  const GraphNode<NodeID, EdgeID> *getPred() const { return Seq.Pred; }
  GraphNode<NodeID, EdgeID> *getPred() { return Seq.Pred; }
  const GraphNode<NodeID, EdgeID> *getSucc() const { return Seq.Succ; }
  GraphNode<NodeID, EdgeID> *getSucc() { return Seq.Succ; }

  void setPred(GraphNode<NodeID, EdgeID> *Node) { Seq.Pred = Node; }
  void setSucc(GraphNode<NodeID, EdgeID> *Node) { Seq.Succ = Node; }

  void updateSequence(GraphNode<NodeID, EdgeID> *Pred,
                      GraphNode<NodeID, EdgeID> *Succ);

  bool hasPred() const { return (getPred() != nullptr); }
  bool hasSucc() const { return (getSucc() != nullptr); }

  virtual bool operator==(const GraphNode<NodeID, EdgeID> &RHS) const final {
    return getID() == RHS.getID();
  }
  virtual bool operator<(const GraphNode<NodeID, EdgeID> &RHS) const final {
    return getID() < RHS.getID();
  }

private:
  NodeID ID;
  const int Rank;
  // Refer to tensor graph, for contraction we use graph representation.
  std::vector<const GraphEdge<NodeID, EdgeID> *> Legs;
  /// Seq - Records the 'Succ' and 'Pred' of each graph node.
  struct {
    GraphNode<NodeID, EdgeID> *Pred;
    GraphNode<NodeID, EdgeID> *Succ;
  } Seq;
};

template <typename NodeID, typename EdgeID>
class GraphEdge : public Comparison<GraphEdge<NodeID, EdgeID>> {
public:
  using NodeIndexPair = std::pair<const GraphNode<NodeID, EdgeID> *, int>;
  GraphEdge(EdgeID &&Id, const NodeIndexPair &Src, const NodeIndexPair &Target);
  GraphEdge(EdgeID &&Id, const GraphNode<NodeID, EdgeID> *SrcNode, int SrcIndex,
            const GraphNode<NodeID, EdgeID> *TargetNode, int TargetIndex);

  const EdgeID &getID() const { return ID; }
  EdgeID &getID() { return ID; }

  const GraphNode<NodeID, EdgeID> *getSrcNode() const {
    return Edge.first.first;
  }

  int getSrcIndex() const { return Edge.first.second; }

  const NodeID &getSrcID() const { return getSrcNode()->getID(); }

  const GraphNode<NodeID, EdgeID> *getTargetNode() const {
    return Edge.second.first;
  }

  int getTargetIndex() const { return Edge.second.second; }

  const NodeID &getTargetID() const { return getTargetNode()->getID(); }

  virtual bool operator==(const GraphEdge<NodeID, EdgeID> &RHS) const final {
    return getID() == RHS.getID();
  }

  virtual bool operator<(const GraphEdge<NodeID, EdgeID> &RHS) const final {
    return getID() < RHS.getID();
  }

private:
  std::pair<NodeIndexPair, NodeIndexPair> Edge;
  EdgeID ID;
};

/// TensorGraph- Tensor graph is used for high order tensor contraction
//  calculation, this idea is inspired by:
/// https://liwt31.github.io/2018/01/22/graphical_matrix
template <typename NodeID, typename EdgeID> class TensorGraph {
public:
  using NodeMap = std::map<NodeID, GraphNode<NodeID, EdgeID> *>;
  using EdgeMap = std::map<EdgeID, GraphEdge<NodeID, EdgeID> *>;
  TensorGraph() {}
  ~TensorGraph();

  const NodeMap &getNodes() const { return Nodes; }
  const EdgeMap &getEdges() const { return Edges; }

  bool empty() const;
  int getNumEdges() const;
  int getNumEdges(const NodeID &Id) const;

  void getEdgesFromNode(EdgeMap &Res,
                        const GraphNode<NodeID, EdgeID> *Node) const;
  void getEdgesToNode(EdgeMap &result,
                      const GraphNode<NodeID, EdgeID> *Node) const;
  void getEdgesBetweenNodes(EdgeMap &Res, const GraphNode<NodeID, EdgeID> *Src,
                            const GraphNode<NodeID, EdgeID> *Target) const;

  bool isNode(const NodeID &Id) const;
  bool isEdge(const EdgeID &Id) const;

  bool addNode(const NodeID &Id, int rank);
  const GraphNode<NodeID, EdgeID> *getNode(const NodeID &Id) const;
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &Id);
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &Id, int Rank);
  bool eraseNode(const NodeID &Id);
  bool addEdge(const EdgeID &Id, const GraphNode<NodeID, EdgeID> *SrcNode,
               int SrcIndex, const GraphNode<NodeID, EdgeID> *TargetNode,
               int TargetIndex);
  bool addEdge(const GraphEdge<NodeID, EdgeID> &Edge);

  const GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &Id) const;
  GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &Id);

  GraphEdge<NodeID, EdgeID> *
  getEdge(const EdgeID &Id, const GraphNode<NodeID, EdgeID> *SrcNode,
          int SrcIndex, const GraphNode<NodeID, EdgeID> *TargetNode,
          int TargetIndex);

  bool eraseEdge(const EdgeID &Id);

  void plot(std::ofstream &OS) const;

  typename NodeMap::const_iterator nodes_begin() const { return Nodes.begin(); }
  typename NodeMap::const_iterator nodes_end() const { return Nodes.end(); }
  typename EdgeMap::const_iterator edges_begin() const { return Edges.begin(); }
  typename EdgeMap::const_iterator edges_end() const { return Edges.end(); }

  const GraphNode<NodeID, EdgeID> *getStartNode() const;

private:
  EdgeMap Edges;
  NodeMap Nodes;
};

template <typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID>::GraphNode(NodeID &&Id, int Rank)
    : ID(Id), Rank(Rank) {
  for (int i = 0; i < this->getRank(); i++)
    Legs.push_back(nullptr);

  setPred(nullptr);
  setSucc(nullptr);
}

template <typename NodeID, typename EdgeID>
const GraphEdge<NodeID, EdgeID> *const &
GraphNode<NodeID, EdgeID>::at(int I) const {
  assert(I < this->getRank());
  return Legs.at(I);
}

template <typename NodeID, typename EdgeID>
const GraphEdge<NodeID, EdgeID> *&GraphNode<NodeID, EdgeID>::operator[](int I) {
  assert(I < this->getRank());
  return Legs[I];
}

template <typename NodeID, typename EdgeID>
void GraphNode<NodeID, EdgeID>::unset(int I) {
  assert(I < this->getRank());
  Legs[I] = nullptr;
}

template <typename NodeID, typename EdgeID>
bool GraphNode<NodeID, EdgeID>::isSet(int I) const {
  assert(I < this->getRank());
  return Legs[I] != nullptr;
}

template <typename NodeID, typename EdgeID>
bool GraphNode<NodeID, EdgeID>::anySet() const {
  for (int I = 0; I < this->getRank(); ++I)
    if (isSet(I)) {
      return true;
    }

  return false;
}

template <typename NodeID, typename EdgeID>
bool GraphNode<NodeID, EdgeID>::anyUnset() const {
  for (int I = 0; I < this->getRank(); ++I)
    if (isUnset(I)) {
      return true;
    }

  return false;
}

template <typename NodeID, typename EdgeID>
int GraphNode<NodeID, EdgeID>::countSet() const {
  int Result = 0;
  for (int I = 0; I < this->getRank(); ++I)
    Result += isSet(I);

  return Result;
}

template <typename NodeID, typename EdgeID>
void GraphNode<NodeID, EdgeID>::updateSequence(
    GraphNode<NodeID, EdgeID> *Pred, GraphNode<NodeID, EdgeID> *Succ) {
  setPred(Pred);
  if (Pred != nullptr) {
    Pred->setSucc(this);
  }

  setSucc(Succ);
  if (Succ != nullptr) {
    Succ->setPred(this);
  }
}

template <typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID>::GraphEdge(EdgeID &&Id, const NodeIndexPair &Src,
                                     const NodeIndexPair &Target)
    : ID(Id), Edge(Src, Target) {}

template <typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID>::GraphEdge(
    EdgeID &&Id, const GraphNode<NodeID, EdgeID> *SrcNode, int SrcIndex,
    const GraphNode<NodeID, EdgeID> *TargetNode, int TargetIndex)
    : GraphEdge(EdgeID(Id), NodeIndexPair(SrcNode, SrcIndex),
                NodeIndexPair(TargetNode, TargetIndex)) {}

template <typename NodeID, typename EdgeID>
TensorGraph<NodeID, EdgeID>::~TensorGraph() {
  bool Success = true;

  while (!Edges.empty()) {
    Success &= eraseEdge(Edges.begin()->first);
  }

  while (!Nodes.empty()) {
    Success &= eraseNode(Nodes.begin()->first);
  }

  assert(Success &&
         "internal error: erasing of graph components should not fail");
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::empty() const {
  if (!Nodes.empty())
    return false;

  assert(!getNumEdges() && "internal error: cannot have edges without nodes");
  return true;
}

template <typename NodeID, typename EdgeID>
int TensorGraph<NodeID, EdgeID>::getNumEdges() const {
  return Edges.size();
}

template <typename NodeID, typename EdgeID>
int TensorGraph<NodeID, EdgeID>::getNumEdges(const NodeID &Id) const {
  if (!isNode(Id))
    return 0;

  return Nodes.at(Id).countSet();
}

template <typename NodeID, typename EdgeID>
void TensorGraph<NodeID, EdgeID>::getEdgesFromNode(
    EdgeMap &Result, const GraphNode<NodeID, EdgeID> *Node) const {
  for (const auto &E : Edges) {
    const GraphNode<NodeID, EdgeID> &Src = *E.second->getSrcNode();

    if (*Node == Src)
      Result[E.first] = E.second;
  }
}

template <typename NodeID, typename EdgeID>
void TensorGraph<NodeID, EdgeID>::getEdgesToNode(
    EdgeMap &Result, const GraphNode<NodeID, EdgeID> *Node) const {
  for (const auto &E : Edges) {
    const GraphNode<NodeID, EdgeID> &Target = *E.second->getTargetNode();

    if (*Node == Target) {
      Result[E.first] = E.second;
    }
  }
}

template <typename NodeID, typename EdgeID>
void TensorGraph<NodeID, EdgeID>::getEdgesBetweenNodes(
    EdgeMap &Result, const GraphNode<NodeID, EdgeID> *Src,
    const GraphNode<NodeID, EdgeID> *Target) const {
  EdgeMap Tmp;
  getEdgesFromNode(Tmp, Src);

  for (const auto &E : Tmp) {
    const GraphNode<NodeID, EdgeID> &EdgeSrc = *E.second->getSrcNode();
    const GraphNode<NodeID, EdgeID> &EdgeTarget = *E.second->getTargetNode();

    assert(EdgeSrc == *Src);
    if (EdgeTarget == *Target) {
      Result[E.first] = E.second;
    }
  }
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::isNode(const NodeID &Id) const {
  return Nodes.count(Id);
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::isEdge(const EdgeID &Id) const {
  return Edges.count(Id);
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::addNode(const NodeID &Id, int Rank) {
  if (isNode(Id)) {
    return false;
  }

  auto *Node = new GraphNode<NodeID, EdgeID>(NodeID(Id), Rank);
  Nodes[Id] = Node;
  return true;
}

template <typename NodeID, typename EdgeID>
const GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getNode(const NodeID &Id) const {
  assert(isNode(Id));
  return Nodes.at(Id);
}

template <typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getNode(const NodeID &Id) {
  assert(isNode(Id));
  return Nodes[Id];
}

template <typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getNode(const NodeID &Id, int Rank) {
  if (!isNode(Id)) {
    addNode(Id, Rank);
    return getNode(Id);
  }

  GraphNode<NodeID, EdgeID> *Node = getNode(Id);
  assert(Node->getRank() == Rank);
  return Node;
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::eraseNode(const NodeID &Id) {
  if (!isNode(Id)) {
    return false;
  }

  GraphNode<NodeID, EdgeID> *Node = Nodes[Id];
  if (Node->anySet()) {
    // Note: if this node has outgoing edges, we cannot erase it.
    return false;
  }

  if (Node->hasPred()) {
    GraphNode<NodeID, EdgeID> *Pred = Node->getPred();
    Pred->setSucc(Node->getSucc());
  }
  if (Node->hasSucc()) {
    GraphNode<NodeID, EdgeID> *Succ = Node->getSucc();
    Succ->setPred(Node->getPred());
  }

  Nodes.erase(Id);
  delete Node;
  return true;
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::addEdge(
    const EdgeID &Id, const GraphNode<NodeID, EdgeID> *SrcNode, int SrcIndex,
    const GraphNode<NodeID, EdgeID> *TargetNode, int TargetIndex) {
  if (!isNode(SrcNode->getID()) || !isNode(TargetNode->getID())) {
    return false;
  }
  if (SrcNode->isSet(SrcIndex) || TargetNode->isSet(TargetIndex)) {
    return false;
  }

  auto *Edge = new GraphEdge<NodeID, EdgeID>(EdgeID(Id), SrcNode, SrcIndex,
                                             TargetNode, TargetIndex);
  Edges[Id] = Edge;
  auto *WritableSrcNode = getNode(SrcNode->getID());
  auto *WritableTargetNode = getNode(TargetNode->getID());
  (*WritableSrcNode)[SrcIndex] = Edge;
  (*WritableTargetNode)[TargetIndex] = Edge;
  return true;
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::addEdge(const GraphEdge<NodeID, EdgeID> &E) {
  return addEdge(E.getID(), E.getSrcNode(), E.getSrcIndex(), E.getTargetNode(),
                 E.getTargetIndex());
}

template <typename NodeID, typename EdgeID>
const GraphEdge<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getEdge(const EdgeID &Id) const {
  assert(isEdge(Id));
  return Edges.at(Id);
}

template <typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getEdge(const EdgeID &Id) {
  assert(isEdge(Id));
  return Edges[Id];
}

template <typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID> *TensorGraph<NodeID, EdgeID>::getEdge(
    const EdgeID &Id, const GraphNode<NodeID, EdgeID> *SrcNode, int SrcIndex,
    const GraphNode<NodeID, EdgeID> *TargetNode, int TargetIndex) {
  if (!isEdge(Id)) {
    return addEdge(Id, SrcNode, SrcIndex, TargetNode, TargetIndex);
  }

  GraphEdge<NodeID, EdgeID> *Edge = getEdge(Id);
  assert(Edge->getSrcNode() == SrcNode && Edge->getSrcIndex() == SrcIndex &&
         Edge->getTargetNode() == TargetNode &&
         Edge->getTargetIndex() == TargetIndex);
  return Edge;
}

template <typename NodeID, typename EdgeID>
bool TensorGraph<NodeID, EdgeID>::eraseEdge(const EdgeID &Id) {
  if (!isEdge(Id)) {
    return false;
  }

  GraphEdge<NodeID, EdgeID> *Edge = getEdge(Id);
  auto *SrcNode = getNode(Edge->getSrcNode()->getID());
  auto *TargetNode = getNode(Edge->getTargetNode()->getID());
  SrcNode->unset(Edge->getSrcIndex());
  TargetNode->unset(Edge->getTargetIndex());
  Edges.erase(Id);
  delete Edge;
  return true;
}

template <typename NodeID, typename EdgeID>
void TensorGraph<NodeID, EdgeID>::plot(std::ofstream &OS) const {
  OS << "digraph <" << this << "> {\n";

  for (const auto &N : Nodes) {
    const NodeID &Id = N.first;
    OS << "\"" << Id.str() << "\""
       << " [label=\"" << Id.getLabel() << "\"];\n";
  }

  for (const auto &E : Edges) {
    const EdgeID &Id = E.first;
    const NodeID &Src = E.second->getSrcNode()->getID();
    const NodeID &Target = E.second->getTargetNode()->getID();

    OS << "\"" << Src.str() << "\" -> \"" << Target.str() << "\""
       << " [label=\"" << Id.getLabel() << "\"];\n";
  }

  const GraphNode<NodeID, EdgeID> *N = getStartNode();
  while (N->hasSucc()) {
    const GraphNode<NodeID, EdgeID> *Succ = N->getSucc();
    const NodeID &NId = N->getID();
    const NodeID &SuccId = Succ->getID();

    OS << "\"" << NId.str() << "\" -> \"" << SuccId.str() << "\""
       << " [style=dashed];\n";

    N = Succ;
  }

  OS << "}\n";
}

template <typename NodeID, typename EdgeID>
const GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getStartNode() const {
  const GraphNode<NodeID, EdgeID> *Node = nodes_begin()->second;

  while (Node->hasPred())
    Node = Node->getPred();

  return Node;
}

} // end namespace phaeton

#endif // PHAETON_CODEGEN_CGUTILS_H
