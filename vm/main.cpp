#include <iostream>
#include <vector>

#include "vm.h"

int main(const int argc, const char ** arg) {
  if (argc < 2) {
    std::cerr << "File path is required to run the program.";
    return 1;
  }
  vm::Options options = vm::Options();

  for (unsigned int i = 2; i < argc; i++) {
    std::string argument (arg[i]);
    if (argument == "-c") {
      std::cout << "perserve true" << std::endl;
      options.perserveComments = true;
    } else if (argument == "-n") {
      std::cout << "notes true" << std::endl;
      options.notes = true;
    } else {
      std::cout << "Virtual Language Assembler" << std::endl <<
        "\tUsage: parser [FILE/DIRECTORY] -h for help" << std::endl <<
        "\t-c to perserve comments" << std::endl <<
        "\t-n to write notes" << std::endl <<
        "\t-h or --help for help" << std::endl;
      return 0;
    }
  }
  
  vm::Translator::translate(arg[1], options);

  return 0;
}