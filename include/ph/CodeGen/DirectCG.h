//==------ DirectCG.h ----- Representation of CodeGen ----------------------==//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This file defines intefaces of direct code generation via a naive method.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_CODEGEN_DIRECT_CODEGEN_H
#define PHAETON_CODEGEN_DIRECT_CODEGEN_H

#include "ph/AST/AST.h"
#include "ph/CodeGen/CodeGen.h"
#include "ph/Opt/ExprTree.h"

#include <map>
#include <string>

namespace phaeton {

class DirectCodeGen : public CodeGen {
public:
  DirectCodeGen(const Sema *sema, const std::string &functionName);

  virtual void visitStmt(const Stmt *s) override;

  virtual void visitElemDirect(const ElemDirect *ed) override {}

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override {}
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);
};

} // end namespace phaeton

#endif // PHAETON_CODEGEN_DIRECT_CODEGEN_H
