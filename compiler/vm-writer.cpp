#include <string>
#include "compiler.h"

std::string getSegmentString(const compiler::VMWriter::Segment & segment) {
  if (segment == compiler::VMWriter::CONST) {
    return "constant";
  } else if (segment == compiler::VMWriter::ARG) {
    return "argument";
  } else if (segment == compiler::VMWriter::LOCAL) {
    return "local";
  } else if (segment == compiler::VMWriter::STATIC) {
    return "static";
  } else if (segment == compiler::VMWriter::THIS) {
    return "this";
  } else if (segment == compiler::VMWriter::THAT) {
    return "that";
  } else if (segment == compiler::VMWriter::POINTER) {
    return "pointer";
  } else if (segment == compiler::VMWriter::TEMP) {
    return "temp";
  } else {
    throw std::invalid_argument("Invalid segment value.");
  }
}

// ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
std::string getArithmeticString(const compiler::VMWriter::Arithmetic & type) {
  if (type == compiler::VMWriter::ADD) {
    return "add";
  } else if (type == compiler::VMWriter::SUB) {
    return "sub";
  } else if (type == compiler::VMWriter::NEG) {
    return "neg";
  } else if (type == compiler::VMWriter::EQ) {
    return "eq";
  } else if (type == compiler::VMWriter::GT) {
    return "gt";
  } else if (type == compiler::VMWriter::LT) {
    return "lt";
  } else if (type == compiler::VMWriter::AND) {
    return "and";
  } else if (type == compiler::VMWriter::OR) {
    return "or";
  } else if (type == compiler::VMWriter::NOT) {
    return "not";
  } else {
    throw std::invalid_argument("Invalid arithmetic type .");
  }
}

compiler::VMWriter::VMWriter(std::vector<std::string> & lines): output(lines) {}

std::string compiler::VMWriter::generateLabel() {
  return "LABEL" + std::to_string(labelCount++);
}

void compiler::VMWriter::writePush(const Segment & segment, const unsigned int & index) {
  output.push_back("push " + getSegmentString(segment) + " " + std::to_string(index));
}

void compiler::VMWriter::writePop(const Segment & segment, const unsigned int & index) {
  if (segment == CONST) {
    throw std::invalid_argument("Can not pop into a constant value.");
  } else if (segment == POINTER && (index != 0 && index != 1)) {
    throw std::invalid_argument("Can not pop value into a pointer of index " + std::to_string(index) + ". Only 0 or 1 are valid.");
  }
  output.push_back("pop " + getSegmentString(segment) + " " + std::to_string(index));
}

void compiler::VMWriter::writeArithmetic(const Arithmetic & command) {
  output.push_back(getArithmeticString(command));
}

void compiler::VMWriter::writeLabel(const std::string & label) {
  output.push_back("label " + label);
}

void compiler::VMWriter::writeGoto(const std::string & label) {
  output.push_back("goto " + label);
}

void compiler::VMWriter::writeIf(const std::string & label) {
  output.push_back("if-goto " + label);
}

void compiler::VMWriter::writeCall(const std::string & name, const unsigned int & numArgs) {
  output.push_back("call " + name + " " + std::to_string(numArgs));
}

void compiler::VMWriter::writeFunction(const std::string & name, const unsigned int & numLocals) {
  output.push_back("function " + name + " " + std::to_string(numLocals));
}

void compiler::VMWriter::writeReturn() {
  output.push_back("return");
}