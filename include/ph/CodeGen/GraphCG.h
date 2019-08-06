//==------ GraphCG.h ------ Representation of CodeGen ----------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines intefaces of code generation via graph.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_GRAPH_CODEGEN_H
#define PHAETON_CODEGEN_GRAPH_CODEGEN_H

#include "ph/AST/AST.h"
#include "ph/CodeGen/CGUtils.h"
#include "ph/CodeGen/GraphCGUtils.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTree.h"

#include <list>
#include <set>
#include <utility>
#include <vector>

namespace phaeton {

class GraphCodeGen : public CodeGen {
public:
  // Helper alias for type traits.
  using NodeID = AddressID<ExprNode>;
  using EdgeID = StringID;
  using GCG_Node = GraphNode<NodeID, EdgeID>;
  using GCG_Edge = GraphEdge<NodeID, EdgeID>;
  using GCG_Graph = TensorGraph<NodeID, EdgeID>;
  using GCG_Legs = std::vector<GCG_Edge::NodeIndexPair>;
  using EdgeSet = std::set<const GCG_Edge *>;

  GraphCodeGen(const Sema *sema, const std::string &functionName);

  virtual void visitStmt(const Stmt *) override;

  virtual void visitElemDirect(const ElemDirect *ed) override {}

  virtual void visitBinaryExpr(const BinaryExpr *) override;
  virtual void visitIdentifier(const Identifier *) override;
  virtual void visitInteger(const Integer *) override;
  virtual void visitBrackExpr(const BrackExpr *) override;
  virtual void visitParenExpr(const ParenExpr *) override;

  static void dump(const GCG_Graph &gcg);

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
  // Helpers for graph construction and building.
  GCG_Graph *curGraph;
  GCG_Legs curLegs;
  GCG_Node *curEnd;

  void updateCurEnd(GCG_Node *n);

  // map for 'Exprs' in the AST to graphs representing the 'Expr'.
  std::map<const Expr *, const GCG_Graph *> ExprGraphs;

  // set for keeping track of allocated graphs.
  std::set<const GCG_Graph *> Graphs;
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_GRAPH_CODEGEN_H
