//===------ ph-translate.cpp --- Phaeton-Language Front-end ---------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This utility may be invoked in the following manner:
//   ph-translate --help                - Output help info.
//   ph-translate [options]             - Read from stdin.
//   ph-translate [options] file        - Read from "file" and output default.
//   ph-translate [options] f1 -o f2    - Read from "f1" and output "f2".
//
//===----------------------------------------------------------------------===//
//
// TODO: Refering clang-2.6/tools/clang-cc to add the compiler driver module.
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <iostream>

#include "ph/CodeGen/GraphCG.h"
#include "ph/Target/OMPCG.h"
#include "ph/Lex/Lexer.h"
#include "ph/Parse/Parser.h"
#include "ph/Sema/Sema.h"
#include "ph/Tooling/CommonOptionsParser.h"

#define PH_COMPILER_EXE "ph-translate"

using namespace phaeton;

namespace {
// FIXME: Current Phaeton has no compiler driver, so the main entrance
// looks all in a mess.
enum ProgActions {
  EmitAssembly, // Emit a .s file.
  EmitLLVM,     // Emit a LLVM IR .ll file.
  EmitNumpy,    // Translate input source into python numpy code.
  EmitOpenMP,   // Translate input source into CPU OpenMP code.
  EmitOpenCL,   // Translate input source into GPU OpenCL code.
  EmitCuda,     // Translate input source into NVIDIA GPU CUDA code.
  EmitBang,     // Translate input source into Cambricon MLU BANG code.
  EmitCCE,      // Translate input source into Huawei Ascend CCE code.
  EmitTPU,      // Translate input source into Google TPU code.
  ASTDump,      // Parse Phaeton ASTs and dump them.
  DumpTokens,   // Dump out preprocessed tokens.
};

/// FinalPhase - Ordered values for successive stages in the compilation process
/// which interact with user options.
enum FinalPhase {
  Translate, // Code Translation from Phaeton to target language.
  Compile,   // Compile with host runtime.
  Assemble,  // Assemble phase.
  Link       // Link to get an executable binary.
};
} // end anonymous namespace

const std::set<std::string> VALID_TARGET_LANG = {
    "OPENMP", "OPENCL", "CUDA", "BANG", "CCE", "TPU", "NUMPY"};

// TODO: refer to ClangCheck to add a factory wrapper.
void createOptions(Options &Opts) {
  Opts.positional_help("[optional args]").show_positional_help();
  Opts.allow_unrecognised_options().add_options()("h, help", "Print help")(
      "i, input", "Input phaeton source file", cxxopts::value<std::string>())(
      "l, lang",
      "Emit target language source code, currently supports: Numpy, OpenMP, "
      "OpenCL, Cuda, "
      "Bang, CCE, TPU",
      cxxopts::value<std::string>()->default_value("OpenMP"))(
      "ast-dump", "Build Phaeton ASTs and then debug dump them")(
      "token-dump", "Build Phaeton source code and dump the tokens")(
      "o, output", "Output file",
      cxxopts::value<std::string>()->default_value("a.cpp"))(
      "positional", "These are the arguments that are entered "
                    "without an option",
      cxxopts::value<std::vector<std::string>>());
  Opts.parse_positional({"input", "positional"});
}

ParseResult parseArgs(Options &Opts, int &argc, char **argv) {
  if (argc < 2) {
    std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
              << "no input files! Usage see option '--help'.\n";
    exit(-1);
  }
  try {
    auto Result = Opts.parse(argc, argv);
    return Result;
  } catch (const cxxopts::OptionException &E) {
    // FIXME: wrong cli options CANNOT be detected now.
    std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
              << "parsing options: " << E.what() << std::endl;
    exit(-1);
  }
}

std::string getDefaultOutputFileName(std::string &TgtLang) {
  if (!std::strcmp(TgtLang.c_str(), "OPENMP")) {
    return "a.cpp";
  } else if (!std::strcmp(TgtLang.c_str(), "NUMPY")) {
    return "a.py";
  } else if (!std::strcmp(TgtLang.c_str(), "OPENCL")) {
    return "a.cl";
  } else if (!std::strcmp(TgtLang.c_str(), "CUDA")) {
    return "a.cu";
  } else if (!std::strcmp(TgtLang.c_str(), "BANG")) {
    return "a.mlu";
  } else if (!std::strcmp(TgtLang.c_str(), "CCE")) {
    return "a.cce";
  } else if (!std::strcmp(TgtLang.c_str(), "TPU")) {
    return "a.jl";
  }
  return "a.cpp";
}

std::string getTargetLanguageCode(const Parser &PhParser,
                                  const std::string &TgtLang) {
  Sema S;
  S.visitProgram(PhParser.getAST());
  std::string TgtCode = "";
  if (!std::strcmp(TgtLang.c_str(), "OPENMP")) {
    GraphCodeGen Gen(&S, "omp_func");
    // FIXME: CodeGen mode must be sync with a specific codegen, i.e.
    // if we omit the function name, the wrapper caller will be wrong.
    OMPCG OMP(&Gen, true);
    OMP.genCode(PhParser.getAST());
    TgtCode = OMP.getCode();
  } else if (!std::strcmp(TgtLang.c_str(), "NUMPY")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  } else if (!std::strcmp(TgtLang.c_str(), "OPENCL")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  } else if (!std::strcmp(TgtLang.c_str(), "CUDA")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  } else if (!std::strcmp(TgtLang.c_str(), "BANG")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  } else if (!std::strcmp(TgtLang.c_str(), "CCE")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  } else if (!std::strcmp(TgtLang.c_str(), "TPU")) {
    std::cout << PH_COMPILER_EXE << ":" << FRED(" wip: ")
              << "Target language not support yet\n";
    exit(EXIT_SUCCESS);
  }
  return TgtCode;
}

void dumpPhaetonAST(Parser &PhParser, char *InPhTokens) {
  PhParser.getAST()->dump();
  delete[] InPhTokens;
  Program::destroy(PhParser.getAST());
  exit(EXIT_SUCCESS);
}

void dumpPhaetonTokens(char *InPhTokens) {
  Lexer PhLexer(InPhTokens);
  while (1) {
    int Token = PhLexer.lex();
    if (Token == EOF) {
      break;
    }

    std::cout << Lexer::getTokenString(Token) << ' ';
  }
  std::cout << '\n';

  delete[] InPhTokens;
  exit(EXIT_SUCCESS);
}

void buildJobs(const Options &Opts, const ParseResult &Result) {
  // Dump the help text message
  if (Result.count("help")) {
    std::cout << Opts.help({"", "Group"}) << std::endl;
    exit(EXIT_SUCCESS);
  }

  std::ifstream InPhFileStream;
  char *InPhTokens = NULL;

  // If we get here, that means, we don't pass the 'help' option,
  // so the user must have at least one input.
  if (Result.count("input")) {
    std::string PhSrcFile = Result["input"].as<std::string>();
    InPhFileStream.open(PhSrcFile);
    // Fail to open input source file.
    if (!InPhFileStream.is_open()) {
      std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ") << "Fail to open "
                << PhSrcFile << '\n';
      exit(-1);
    }
    std::filebuf *FileBuf = InPhFileStream.rdbuf();
    std::size_t Size =
        FileBuf->pubseekoff(0, InPhFileStream.end, InPhFileStream.in);
    FileBuf->pubseekpos(0, InPhFileStream.in);

    InPhTokens = new char[Size + 1];
    FileBuf->sgetn(InPhTokens, Size);
    InPhTokens[Size] = 0;
    InPhFileStream.close();
  } else {
    std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
              << "No input files! Usage see option '--help'.\n";
    exit(-1);
  }

  // Then we need to determine the final phase.

  std::string TgtLang = "";
  // If we get here, that means we need to translate Phaeton into target
  // language.
  if (Result.count("lang")) {
    TgtLang = Result["lang"].as<std::string>();
    // Uniform use of upper case
    toUpperCase(TgtLang);
    if (!VALID_TARGET_LANG.count(TgtLang)) {
      std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
                << "Unknown target language! Now only support 'Numpy, OpenMP, "
                   "OpenCL, Cuda, Bang, CCE, TPU'.\n";
      exit(-1);
    }
  } else {
    // Default target language is OpenMP
    TgtLang = "OpenMP";
  }
  // Uniform use of upper case
  toUpperCase(TgtLang);

  std::string OutTgtSrc = "";
  // If we get here, that means we have already get the target language.
  if (Result.count("output")) {
    OutTgtSrc = Result["output"].as<std::string>();
  } else {
    // Default output name.
    OutTgtSrc = getDefaultOutputFileName(TgtLang);
  }

  if (Result.count("positional")) {
    std::cout << "Positional = {";
    auto &V = Result["positional"].as<std::vector<std::string>>();
    for (const auto &S : V) {
      std::cout << S << ", ";
    }
    std::cout << "}" << std::endl;
  }

  // If we get here, that means we have to perform translation to
  // translate Phaeton input source file into the target language
  // output.
  Parser PhParser(InPhTokens);
  if (PhParser.parse()) {
    std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
              << "Fail to parse input Phaeton tokens\n";
    exit(-1);
  }

  // If we get here, that means we need to build Phaeton ASTs and dump them.
  if (Result.count("ast-dump")) {
    dumpPhaetonAST(PhParser, InPhTokens);
  }

  // If we get here, that means we need to build Phaeton source code and dump
  // the tokens.
  if (Result.count("token-dump")) {
    dumpPhaetonTokens(InPhTokens);
  }

  // FIXME: 'in_ph_tokens' maybe double free.
  // Phaeton translate '-o' option processing.
  {
    delete[] InPhTokens;
    // Write the target language source code to file 'OutTgtSrc'.
    std::ofstream OutTgtFileStream(OutTgtSrc);
    if (OutTgtFileStream.is_open()) {
      OutTgtFileStream << getTargetLanguageCode(PhParser, TgtLang);
      OutTgtFileStream.close();
    } else {
      std::cerr << PH_COMPILER_EXE << ":" << FRED(" error: ")
                << "Fail to open file " << OutTgtSrc << '\n';
      exit(-1);
    }
    Program::destroy(PhParser.getAST());
  }
}

int main(int argc, char *argv[]) {
  Options Opts(/*argv[0]*/ PH_COMPILER_EXE,
                  "Phaeton compiler command line options");
  createOptions(Opts);
  // Parse input command line arguments.
  auto Result = parseArgs(Opts, argc, argv);
  auto Arguments = Result.arguments();

  buildJobs(Opts, Result);
  return 0;
}
