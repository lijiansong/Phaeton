#include <fstream>
#include <iostream>

#include "Parser.h"
#include "Sema.h"

int main(int argc, char *argv[]) {

  std::ifstream in_file;

  if (argc != 2) {
    return 1;
  } else {
    in_file.open(argv[1]);
    if (!in_file.is_open())
      return 2;
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
    return 3;
  }

  parser.getAST()->dump();

  Sema().visitProgram(parser.getAST());

  Program::destroy(parser.getAST());

  delete[] input;

  return 0;
}
