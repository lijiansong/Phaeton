//===--- ph-translate.cpp - Phaeton-Language Front-end --------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This utility may be invoked in the following manner:
//   ph-translate --help                - Output help info.
//   ph-translate [options]             - Read from stdin.
//   ph-translate [options] file        - Read from "file".
//   ph-translate [options] file1 file2 - Read these files.
//
//===----------------------------------------------------------------------===//
//
// TODO: Refering clang-2.6/tools/clang-cc to add the compiler driver module.
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <iostream>

#include "ph/CodeGen/PyCG.h"
#include "ph/Parse/Parser.h"
#include "ph/Sema/Sema.h"

enum ProgActions {
  EmitAssembly, // Emit a .s file.
  EmitLLVM,     // Emit a LLVM IR .ll file.
  EmitOpenMP,   // Translate input source into CPU OpenMP code.
  EmitOpenCL,   // Translate input source into GPU OpenCL code.
  EmitCuda,     // Translate input source into NVIDIA GPU CUDA code.
  EmitBang,     // Translate input source into Cambricon MLU BANG code.
  EmitCCE,      // Translate input source into Huawei Ascend CCE code.
  EmitTPU,      // Translate input source into Google TPU code.
  EmitNumpy,    // Translate input source into python numpy code.
  ASTDump,      // Parse Phaeton ASTs and dump them.
  DumpTokens,   // Dump out preprocessed tokens.
};

int main(int argc, char *argv[]) {

  std::ifstream in_file;

  if (argc != 2) {
    std::cout << "Usage: Translate [input file]... " << '\n';
    return -1;
  } else {
    in_file.open(argv[1]);
    if (!in_file.is_open()) {
      std::cout << "Error! Fail to open " << argv[1] << '\n';
      return -1;
    }
  }

  std::filebuf *file_buf = in_file.rdbuf();
  std::size_t size = file_buf->pubseekoff(0, in_file.end, in_file.in);
  file_buf->pubseekpos(0, in_file.in);

  char *input = new char[size + 1];
  file_buf->sgetn(input, size);
  input[size] = 0;

  in_file.close();

  Parser parser(input);
  if (parser.parse()) {
    return -1;
  }

  // parser.getAST()->dump();
  delete[] input;

  Sema sema;
  sema.visitProgram(parser.getAST());
  NumpyDirectCG npcg(&sema);
  npcg.visitProgram(parser.getAST());
  std::cout << npcg.getCode();

  Program::destroy(parser.getAST());

  return 0;
}
