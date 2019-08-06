//===--- Tooling.h - Framework for standalone phaeton tools -----*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file implements functions to run Phaeton tools standalone instead
//  of running them as a plugin.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TOOLING_H
#define PHAETON_TOOLING_H

namespace phaeton {

class ToolAction {
public:
  virtual ~ToolAction();
};

} // end namespace phaeton

#endif // PHAETON_TOOLING_H
