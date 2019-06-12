#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <algorithm>

#include "mapping.h"
#include "vm.h"

using std::string;

std::vector<string> vm::tokenizer(const std::string& p_pcstStr, const char& delim)  {
  std::vector<string> tokens;
  std::stringstream mySstream( p_pcstStr );
  string temp;

  while(getline(mySstream, temp, delim)) {
    tokens.push_back(temp);
  }

  return tokens;
}

void removeCharsFromString(string &str, const std::string& charsToRemove) {
  for (unsigned int i = 0; i < charsToRemove.length(); ++i) {
    str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
  }
}

void vm::Parser::sanitize(std::string& line) {
  // Remove comments
  line = line.substr(0, line.find("//", 0));
  // Uppercase all letters
  transform(line.begin(), line.end(), line.begin(), ::tolower);
  removeCharsFromString(line, "\n\r");
}

vm::Parser::Parser(const string& inputFileName) {
  std::ifstream file (inputFileName);
  std::string line;
  if (file.is_open()) {
    while(getline(file, line)) {
      sanitize(line);
      if (line.length() > 0) {
        lines.push_back(line);
      }
    }
    file.close();
  }
}

bool vm::Parser::hasMoreCommands() {
  return lines.size() > position;
}

void vm::Parser::advance() {
  position++;
}

vm::CommandType vm::Parser::commandType() {
  std::vector<string> split = tokenizer(lines.at(position), ' ');
  const string firstArg = split.at(0);
  if (firstArg == "pop") {
    return C_POP;
  } else if (firstArg == "push") {
    return C_PUSH;
  } else if (
    firstArg == "add" || 
    firstArg == "sub" ||
    firstArg == "neg" ||
    firstArg == "eq" ||
    firstArg == "gt" ||
    firstArg == "lt" ||
    firstArg == "and" ||
    firstArg == "or" ||
    firstArg == "not"
  ) {
    return C_ARITHMETIC;
  } else {
    throw std::invalid_argument("Invalid operator \"" + firstArg + "\"");
  }
}

// TODO: check is there
string vm::Parser::arg1() {
  CommandType type = commandType();
  if (type == C_RETURN) {
    throw std::invalid_argument("arg1 can not be called if command type is return.");
  }
  std::vector<string> split = tokenizer(lines.at(position), ' ');
  if (type == C_ARITHMETIC) {
    return split.at(0);
  } else {
    return split.at(1);
  }
}

// TODO: check is there
int vm::Parser::arg2() {
  CommandType type = commandType();
  if (type == C_POP || type == C_PUSH || type == C_FUNCTION || type == C_CALL) {
    std::vector<string> split = tokenizer(lines.at(position), ' ');
    return std::stoi(split.at(2));
  } else {
    throw std::invalid_argument("Invalid command type for arg2 call.");
  }
}

vm::CodeWriter::CodeWriter(const string& outputFilename): outputFile(outputFilename) {
  boost::filesystem::path p(outputFilename);
  // Gets the filename with extension (removes the path to the file)
  string basename = p.filename().string();
  // Removes the filename extenstion
  basename = basename.substr(0, basename.find_last_of('.'));
  this->outputFilename = basename;
}

void vm::CodeWriter::writeArithmetic(const string& command) {
  string output;

  if (command == "add") {
    output = ASM_ADD;
  } else if (command == "sub") {
    output = ASM_SUB;
  } else if (command == "neg") {
    output = ASM_NEG;
  } else if (command == "eq") { // TODO: reduce
    output = (boost::format(ASM_EQ) % labelCount % labelCount).str();
    labelCount++;
  } else if (command == "gt") {
    output = (boost::format(ASM_GT) % labelCount % labelCount).str();
    labelCount++;
  } else if (command == "lt") {
    output = (boost::format(ASM_LT) % labelCount % labelCount).str();
    labelCount++;
  } else if (command == "and") {
    output = ASM_AND;
  } else if (command == "or") {
    output = ASM_OR;
  } else if (command == "not") {
    output = ASM_NOT;
  } else {
    throw std::invalid_argument("Arithmetic command \"" + command + "\" is invalid.");
  }
  if (outputFile.is_open()) {
    outputFile << output;
  }
}

// TODO: change segment to an enum for speed comparisons
void vm::CodeWriter::writePushPop(const vm::CommandType& command, const string& segment, const unsigned int& index) {
  string output;
  if (command == C_PUSH) {
    if (segment == "local") { // TODO: reduce
      output = (boost::format(ASM_PUSH_GENERIC) % index % "LCL").str();
    } else if (segment == "argument") {
      output = (boost::format(ASM_PUSH_GENERIC) % index % "ARG").str();
    } else if (segment == "this") {
      output = (boost::format(ASM_PUSH_GENERIC) % index % "THIS").str();
    } else if (segment == "that") {
      output = (boost::format(ASM_PUSH_GENERIC) % index % "THAT").str();
    } else if (segment == "constant") {
      output = (boost::format(ASM_PUSH_CONSTANT) % index).str();
    } else if (segment == "static") {
      string staticName = outputFilename + "." + std::to_string(index);
      output = (boost::format(ASM_PUSH_TEMP) % staticName).str();
    } else if (segment == "pointer") {
      if (index == 0) {
        output = (boost::format(ASM_PUSH_TEMP) % "THIS").str();
      } else if (index == 1) {
        output = (boost::format(ASM_PUSH_TEMP) % "THAT").str();
      } else {
        throw std::invalid_argument("index of push of pointer must be 0 or 1.");
      }
    } else if (segment == "temp") {
      if (index >= 10) { // index + 5 >= 15
        throw std::invalid_argument("index of push of temp can not be bigger an 9.");
      }
      output = (boost::format(ASM_PUSH_TEMP) % std::to_string(index + 5)).str();
    } else {
      throw  std::invalid_argument("Invalid segment \"" + segment + "\" for push command.");
    }
  } else if (command == C_POP) {
    if (segment == "local") {
      output = (boost::format(ASM_POP_GENERIC) % index % "LCL").str();
    } else if (segment == "argument") {
      output = (boost::format(ASM_POP_GENERIC) % index % "ARG").str();
    } else if (segment == "this") {
      output = (boost::format(ASM_POP_GENERIC) % index % "THIS").str();
    } else if (segment == "that") {
      output = (boost::format(ASM_POP_GENERIC) % index % "THAT").str();
    } else if (segment == "static") {
      string staticName = outputFilename + "." + std::to_string(index);
      output = (boost::format(ASM_POP_PREDEFINED) % staticName).str();
    } else if (segment == "pointer") {
      if (index == 0) {
        output = (boost::format(ASM_POP_PREDEFINED) % "THIS").str();
      } else if (index == 1) {
        output = (boost::format(ASM_POP_PREDEFINED) % "THAT").str();
      } else {
        throw std::invalid_argument("index of pop of pointer must be 0 or 1.");
      }
    } else if (segment == "temp") {
      if (index >= 10) { // index + 5 >= 15
        throw std::invalid_argument("index of push of temp can not be bigger an 9.");
      }
      output = (boost::format(ASM_POP_PREDEFINED) % std::to_string(index + 5)).str();
    } else {
      throw std::invalid_argument("Invalid segment for pop \"" + segment +"\"");
    }
  } else {
    throw std::invalid_argument("Invalid command type for writePushPop.");
  }

  if (outputFile.is_open()) {
    outputFile << output;
  }
}

void vm::CodeWriter::close() {
  if (outputFile.is_open()) {
    outputFile.close();
  }
}

vm::CodeWriter::~CodeWriter() {
  close();
}