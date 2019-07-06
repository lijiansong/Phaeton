//==------ GraphCG.h ------ Representation of CodeGen ----------------------==//
//
// This file defines intefaces of code generation via graph.
//
//===----------------------------------------------------------------------===//

#ifndef __GRAPH_CG_H__
#define __GRAPH_CG_H__

#include <list>
#include <set>
#include <utility>
#include <vector>

#include "tir/AST/AST.h"
#include "tir/CodeGen/CodeGen.h"
#include "tir/CodeGen/CGUtils.h"
#include "tir/CodeGen/TensorGraph.h"

class GraphCodeGen : public CodeGen {
public:
  using GCG_Graph = TensorGraph<StringID, StringID>;
  using GCG_Node = GraphNode<StringID, StringID>;
  using GCG_Edge = GraphEdge<StringID, StringID>;

  using GCG_LabeledGraph = std::pair<const std::string, GCG_Graph *>;
  using GCG_Legs = std::vector<GCG_Edge::NodeIndexPair>;

  using EdgeSet = std::set<const GCG_Edge *>;

private:
  // map for names of temporary variable to expressions represented by graphs
  // FIXME: we need to record the order
  std::list<GCG_LabeledGraph> Graphs;

  // helpers for graph constuction
  GCG_Graph *curGraph;
  GCG_Legs curLegs;
  GCG_Node *curEnd;

  void updateCurEnd(GCG_Node *n);

public:
  GraphCodeGen(const Sema *sema);
  ~GraphCodeGen();

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);

  virtual void visitProgramPrologue(const Program *p) {}
  virtual void visitProgramEpilogue(const Program *p) {}

  virtual void visitDeclPrologue(const Decl *d) {}
  virtual void visitDeclEpilogue(const Decl *d) {}

  static void dump(const GCG_Graph &g);
  void dump() const;

  virtual void emitGraph(const std::string &name, GCG_Graph *graph);
  virtual void selectEdgesToContract(EdgeSet &result, const GCG_Graph &g) const;
  void getRemainingEdgesAtNode(EdgeSet &result, const GCG_Node &n,
                               const EdgeSet &toContract) const;
  void replaceEdgesAtNode(GCG_Graph &graph, const GCG_Node &oldNode,
                          const EdgeSet &edgesAtOldNode,
                          const GCG_Node &newNode, int shift,
                          const EdgeSet &toContract);

  std::string emitGraphForExpr(const Expr *expr);

  virtual void emitContraction(const std::string &result,
                               const std::string &lhs, const List &srcIndices,
                               const std::string &rhs,
                               const List &tgtIndices) = 0;
  virtual void emitTensorProduct(const std::string &result,
                                 const std::string &lhs,
                                 const std::string &rhs) = 0;
  virtual void emitTensorStack(const std::string &result,
                               const std::list<std::string> &temps) = 0;
  virtual void emitAssignment(const std::string &result,
                              const std::string &expr) = 0;

  virtual void emitAddExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) = 0;
  virtual void emitSubExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) = 0;
  virtual void emitMulExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) = 0;
  virtual void emitDivExpr(const std::string &result, const std::string &lhs,
                           const std::string &rhs) = 0;
};

#endif // __GRAPH_CG_H__
