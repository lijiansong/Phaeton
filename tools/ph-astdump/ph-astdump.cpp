//===- ph-astdump.cpp - Phaeton AST Dumper --------------------------------===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
// This utility provides a simple wrapper around the Flex & Bison yyparser,
// which allow the dump of Phaeton programs abstract syntax tree.
//
// This utility may be invoked in the following manner:
//   ph-astdump --help                  - Output help info.
//   ph-translate [options]             - Read from stdin.
//   ph-translate [options] file        - Read from "file" and output to stdout.
//
//===----------------------------------------------------------------------===//
//
// TODO: Refering clang-2.6 ast-print or Swift to dump the AST more elegantly.
//
//===----------------------------------------------------------------------===//

#include <fstream>
#include <iostream>

#include "ph/Parse/Parser.h"
#include "ph/Tooling/CommonOptionsParser.h"

#define PH_ASTDUMP_EXE "ph-astdump"

void createOptions(Options &options) {
  options.positional_help("[optional args]").show_positional_help();
  options.allow_unrecognised_options().add_options()("h, help", "Print help")(
      "i, input", "Input phaeton source file", cxxopts::value<std::string>())(
      "positional",
      "These are the arguments that are entered "
      "without an option",
      cxxopts::value<std::vector<std::string>>());
  options.parse_positional({"input", "positional"});
}

ParseResult parseArgs(Options &options, int &argc, char **argv) {
  if (argc < 2) {
    std::cerr << PH_ASTDUMP_EXE << ":" << FRED(" error: ")
              << "no input files! Usage see option '--help'.\n";
    exit(-1);
  }
  try {
    auto result = options.parse(argc, argv);
    return result;
  } catch (const cxxopts::OptionException &e) {
    std::cerr << PH_ASTDUMP_EXE << ":" << FRED(" error: ")
              << "parsing options: " << e.what() << std::endl;
    exit(-1);
  }
}

void buildJobs(const Options &options, const ParseResult &result) {
  // Dump the help text message
  if (result.count("help")) {
    std::cout << options.help({"", "Group"}) << std::endl;
    exit(0);
  }

  std::ifstream in_ph_file_stream;
  char *in_ph_tokens = NULL;

  // If we get here, that means, we don't pass the 'help' option,
  // so the user must have at least one input.
  if (result.count("input")) {
    std::string ph_src_file = result["input"].as<std::string>();
    in_ph_file_stream.open(ph_src_file);
    // Fail to open input source file.
    if (!in_ph_file_stream.is_open()) {
      std::cerr << PH_ASTDUMP_EXE << ":" << FRED(" error: ") << "Fail to open "
                << ph_src_file << '\n';
      exit(-1);
    }
    std::filebuf *file_buf = in_ph_file_stream.rdbuf();
    std::size_t size =
        file_buf->pubseekoff(0, in_ph_file_stream.end, in_ph_file_stream.in);
    file_buf->pubseekpos(0, in_ph_file_stream.in);

    in_ph_tokens = new char[size + 1];
    file_buf->sgetn(in_ph_tokens, size);
    in_ph_tokens[size] = 0;
    in_ph_file_stream.close();
  } else {
    std::cerr << PH_ASTDUMP_EXE << ":" << FRED(" error: ")
              << "No input files! Usage see option '--help'.\n";
    exit(-1);
  }

  if (result.count("positional")) {
    std::cout << "Positional = {";
    auto &v = result["positional"].as<std::vector<std::string>>();
    for (const auto &s : v) {
      std::cout << s << ", ";
    }
    std::cout << "}" << std::endl;
  }

  // If we get here, that means we have to perform AST dump.
  Parser parser(in_ph_tokens);
  if (parser.parse()) {
    std::cerr << PH_ASTDUMP_EXE << ":" << FRED(" error: ")
              << "Fail to parse input Phaeton tokens\n";
    exit(-1);
  }
  parser.getAST()->dump();
  delete[] in_ph_tokens;

  Program::destroy(parser.getAST());
}

int main(int argc, char *argv[]) {
  Options options(/*argv[0]*/ PH_ASTDUMP_EXE,
                  "Phaeton AST dumper command line options");
  createOptions(options);
  // Parse input command line arguments.
  auto result = parseArgs(options, argc, argv);
  auto arguments = result.arguments();

  buildJobs(options, result);
  return 0;
}
