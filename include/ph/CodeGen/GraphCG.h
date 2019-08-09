//==------ GraphCG.h ------ Representation of CodeGen ----------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines intefaces of code generation via graph.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_GRAPHCG_H
#define PHAETON_CODEGEN_GRAPHCG_H

#include "ph/AST/AST.h"
#include "ph/CodeGen/CGUtils.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/TensorExprTree.h"

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
  using GraphCGNode = GraphNode<NodeID, EdgeID>;
  using GraphCGEdge = GraphEdge<NodeID, EdgeID>;
  using GraphCGGraph = TensorGraph<NodeID, EdgeID>;
  using GraphCGLegs = std::vector<GraphCGEdge::NodeIndexPair>;
  using EdgeSet = std::set<const GraphCGEdge *>;

  GraphCodeGen(const Sema *S, const std::string &FuncName);

#define GEN_GCG_VISIT_EXPR_DECL(ExprName)                                      \
  virtual void visit##ExprName(const ExprName *) override;

  GEN_GCG_VISIT_EXPR_DECL(Stmt)
  GEN_GCG_VISIT_EXPR_DECL(BinaryExpr)
  GEN_GCG_VISIT_EXPR_DECL(Identifier)
  GEN_GCG_VISIT_EXPR_DECL(Integer)
  GEN_GCG_VISIT_EXPR_DECL(BrackExpr)
  GEN_GCG_VISIT_EXPR_DECL(ParenExpr)

#undef GEN_GCG_VISIT_EXPR_DECL

  virtual void visitElementDirective(const ElementDirective *ED) override {}

  static void dump(const GraphCGGraph &Graph);

private:
  void visitContraction(const Expr *E, const TupleList &Index);

  void buildExprTreeForExpr(const Expr *);
  ExprNode *buildExprTreeForGraph(GraphCGGraph *);

  void selectEdgesToContract(EdgeSet &Res, const GraphCGGraph &Graph) const;
  void getRemainingEdgesAtNode(EdgeSet &Res, const GraphCGNode &Node,
                               const EdgeSet &ToContract) const;
  void replaceEdgesAtNode(GraphCGGraph &Graph, const GraphCGNode &Old,
                          const EdgeSet &EdgesAtOldNode, const GraphCGNode &New,
                          int Shift, const EdgeSet &ToContract);
  // Helpers for graph construction and building.
  GraphCGGraph *CurrentGraph;
  GraphCGLegs CurrentLegs;
  GraphCGNode *CurrentEnd;

  void updateCurrentEnd(GraphCGNode *Node);

  // set for keeping track of allocated graphs.
  std::set<const GraphCGGraph *> Graphs;
  // map for 'Exprs' in the AST to graphs representing the 'Expr'.
  std::map<const Expr *, const GraphCGGraph *> ExprGraphs;
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_GRAPHCG_H
