#include <iostream>
#include <string>
#include "assembler.h"

int main(const int argc, const char ** argv) {
  if (argc <= 2) {
    throw "Invalid args";
  }

  std::cout << "Input file: " << argv[1] << " output file: " << argv[2] << std::endl;
  
  std::cout << "Starting conversion." << std::endl;
  assembler::Assembler builder = {};

  builder.fromFile(argv[1]);
  std::cout << "Loaded file." << std::endl;
  builder.toFile(argv[2]);
  std::cout << "Compiled and done." << std::endl;
  return 0;
}