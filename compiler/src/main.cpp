#include <iostream>
#include <vector>
#include <algorithm>
#include "compiler.h"

int main (const int argc, const char ** arg) {
  if (argc < 2) {
    std::cerr << "File path is required to run the program.";
    return 1;
  } else if (argc == 2) {
    compiler::Writer::compile(arg[1]);
  } else {
    compiler::Writer::compile(arg[1], arg[2]);
  }
}