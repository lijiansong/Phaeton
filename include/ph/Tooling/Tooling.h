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

#ifndef __TOOLING_H__
#define __TOOLING_H__

class ToolAction {
public:
  virtual ~ToolAction();
};

#endif // __TOOLING_H__
