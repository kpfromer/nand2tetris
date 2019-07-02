#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "compiler.h"

void removeCharsFromString(std::string & str, const std::string& charsToRemove) {
  for (unsigned int i = 0; i < charsToRemove.length(); ++i) {
    str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
  }
}

void sanitize(std::string & line) {
  removeCharsFromString(line, "\n\r\t");
}

int main (const int argc, const char ** arg) {
  if (argc < 2) {
    std::cerr << "File path is required to run the program.";
    return 1;
  }

  std::vector<std::string> engineLines;
  std::string line;
  std::ifstream input (arg[1]);

  while (getline(input, line)) {
    sanitize(line);
    engineLines.push_back(line);
  }
  input.close();

  std::vector<std::string> output;
  compiler::CompilationEngine engine (engineLines, output);

  std::string outputFileLoc;
  if (argc > 2) {
    outputFileLoc = arg[2];
  } else {
    outputFileLoc = compiler::Writter::getOutputFileLocation(arg[1]);
  }
  
  std::ofstream outputFile (outputFileLoc);

  for (unsigned int i = 0; i < output.size(); i++) {
    outputFile << output.at(i) << std::endl;
    std::cout << output.at(i) << std::endl;
  }

  outputFile.close();

  return 0;
}