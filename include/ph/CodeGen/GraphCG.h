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

#include "ph/AST/AST.h"
#include "ph/CodeGen/CGUtils.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/CodeGen/ExprTree.h"
#include "ph/CodeGen/TensorGraph.h"

class GraphCodeGen : public CodeGen {
public:
  using NodeID = AddressID<ExprNode>;
  using EdgeID = StringID;

  using GCG_Node = GraphNode<NodeID, EdgeID>;
  using GCG_Edge = GraphEdge<NodeID, EdgeID>;
  using GCG_Graph = TensorGraph<NodeID, EdgeID>;

  using GCG_Legs = std::vector<GCG_Edge::NodeIndexPair>;
  using EdgeSet = std::set<const GCG_Edge *>;

private:
  // helpers for graph constuction
  GCG_Graph *curGraph;
  GCG_Legs curLegs;
  GCG_Node *curEnd;

  void updateCurEnd(GCG_Node *n);

  // map for 'Exprs' in the AST to graphs representing the 'Expr'
  std::map<const Expr *, const GCG_Graph *> ExprGraphs;

  // keep track of allocated graphs
  std::set<const GCG_Graph *> Graphs;

public:
  GraphCodeGen(const Sema *sema);

public:
  virtual void visitStmt(const Stmt *) override;

  virtual void visitBinaryExpr(const BinaryExpr *) override;
  virtual void visitIdentifier(const Identifier *) override;
  virtual void visitInteger(const Integer *) override;
  virtual void visitBrackExpr(const BrackExpr *) override;
  virtual void visitParenExpr(const ParenExpr *) override;

  static void dump(const GCG_Graph &g);

private:
  void visitContraction(const Expr *e, const TupleList &indices);

  void buildExprTreeForExpr(const Expr *);
  ExprNode *buildExprTreeForGraph(GCG_Graph *);

  void selectEdgesToContract(EdgeSet &result, const GCG_Graph &g) const;
  void getRemainingEdgesAtNode(EdgeSet &result, const GCG_Node &n,
                               const EdgeSet &toContract) const;
  void replaceEdgesAtNode(GCG_Graph &graph, const GCG_Node &oldNode,
                          const EdgeSet &edgesAtOldNode,
                          const GCG_Node &newNode, int shift,
                          const EdgeSet &toContract);
};

#endif // __GRAPH_CG_H__
