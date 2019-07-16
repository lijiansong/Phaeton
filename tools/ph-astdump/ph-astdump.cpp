#include <fstream>
#include <iostream>

#include "ph/Parse/Parser.h"

int main(int argc, char *argv[]) {

  std::ifstream in_file;
  if (argc != 2) {
    std::cout << "Usage: AST-Dump [input file]... " << '\n';
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

  parser.getAST()->dump();
  Program::destroy(parser.getAST());

  delete[] input;

  return 0;
}
