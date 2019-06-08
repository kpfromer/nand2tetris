#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <map>
#include <sstream>
#include <bitset>
#include "assembler.h"

bool assembler::is_number(const std::string& s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

void assembler::Assembler::parseLines(const std::vector<std::string>& lines) {
  assembler::Parser p (lines);
  assembler::SymbolTable table = {};

  // Needed to account for label being removed causes lines to shift up
  unsigned int labelCount = 0;

  // First pass find the locations of the labels
  while (p.hasMoreCommands()) {
    p.advance();
    if (p.commandType() == assembler::L_COMMAND) {
      if (!table.contains(p.symbol())) {
        table.addEntry(p.symbol(), p.currentLine - 1 - labelCount++);
      }
    }
  }
  p.currentLine = 0;
  // Second pass convert a and c commands to their codes
  while(p.hasMoreCommands()) {
    p.advance();
    if (p.commandType() == assembler::A_COMMAND) {
      int value;
      std::string symbol = p.symbol();
      if (assembler::is_number(symbol)) {
        value = std::stoi(symbol);
      } else {
        if (table.contains(symbol)) {
          value = table.getAddress(symbol);
        } else {
          value = table.addEntry(symbol);
        }
      }
      parsedLines.push_back("0" + std::bitset<15>(value).to_string());
    } else if (p.commandType() == assembler::C_COMMAND) {
      parsedLines.push_back("111" + p.comp() + p.dest() + p.jump());
    }
  }
}

void assembler::Assembler::sanitize(std::string& line) {
  // Remove comments
  line = line.substr(0, line.find("//", 0));
  // Uppercase all letters
  transform(line.begin(), line.end(), line.begin(), ::toupper);
  // Remove spaces and \r
  std::string::iterator end_pos = std::remove_if(line.begin(), line.end(), [](const char& c) {
    return c == ' ' || c == '\r';
  });
  line.erase(end_pos, line.end());
}

void assembler::Assembler::fromFile(const std::string& filename) {
  std::vector<std::string> lines;
  std::string line;
  std::ifstream file (filename);
  if (file.is_open()) {
    while(getline(file, line)) {
      sanitize(line);
      if (line.length() > 0) {
        lines.push_back(line);
      }
    }
    file.close();
  }
  parseLines(lines);
}

void assembler::Assembler::fromString(const std::string& text) {
  std::vector<std::string> lines;
  std::istringstream stream(text);
  std::string line;
  while(std::getline(stream, line)) {
    sanitize(line);
    if (line.length() > 0) {
      lines.push_back(line);
    }
  }
  parseLines(lines);
}

void assembler::Assembler::toFile(const std::string& filename) {
  std::ofstream outfile (filename);
  if (outfile.is_open()) {
    for (unsigned int i = 0; i < parsedLines.size(); i++) {
      outfile << parsedLines.at(i) << std::endl;
    }
    outfile.close();
  }
}

std::string assembler::Assembler::toString() {
  std::string value = "";
  for (unsigned int i = 0; i < parsedLines.size(); i++) {
    value += parsedLines.at(i) + '\n';
  }
  return value;
}

assembler::Parser::Parser (std::vector<std::string> lines) {
  this->lines = lines;
};

assembler::Parser::Parser (std::string filename) {
  std::string line;
  std::ifstream file(filename);
  if (file.is_open()) {
    while(getline(file, line)) {
      // This removes all white space from the line
      std::string::iterator end_pos = std::remove(line.begin(), line.end(), ' ');
      line.erase(end_pos, line.end());
      // Remove comments
      line = line.substr(0, line.find("//", 0));
      // Uppercase all letters
      transform(line.begin(), line.end(), line.begin(), ::toupper);
      if (line.length() > 0) { // If the line actually has content
        lines.push_back(line);
      }
    }
    file.close();
  }
};

bool assembler::Parser::hasMoreCommands () {
  return lines.size() > currentLine;
}

void assembler::Parser::advance () {
  currentCommand = lines.at(currentLine);
  currentLine += 1;
}

assembler::COMMAND assembler::Parser::commandType () {
  if (currentCommand[0] == '@') {
    return assembler::COMMAND::A_COMMAND;
  } else if (currentCommand[0] == '(' && currentCommand[currentCommand.length() - 1] == ')') {
    return assembler::COMMAND::L_COMMAND;
  } else {
    return assembler::COMMAND::C_COMMAND;
  }
}

std::string assembler::Parser::symbol () {
  COMMAND type = commandType();
  if (type == C_COMMAND) {
    throw std::invalid_argument("C command have no symbol.");
  } else if (type == A_COMMAND) {
    return currentCommand.substr(1); // @xxx -> xxx
  } else {
    std::string command;
    for (char c : currentCommand) { // (xxx) -> xxx
      if (c != '(' && c != ')') {
        command += c;
      }
    }
    return command;
  }
}

std::string assembler::Parser::dest () {
  COMMAND type = commandType();
  if (type != C_COMMAND) {
      throw std::invalid_argument("Only c commands can have a dest."); 
  }
  if (currentCommand.find('=') != std::string::npos) {
    return Code::dest(currentCommand.substr(0, currentCommand.find("=")));
  } else {
    return Code::dest("NULL");
  }
}

std::string assembler::Parser::comp () {
  COMMAND type = commandType();
  if (type != C_COMMAND) {
    throw std::invalid_argument("Only c commands can have a dest."); 
  }
  const unsigned int start = currentCommand.find('=') + 1;
  const unsigned int end = currentCommand.find(';');
  return Code::comp(currentCommand.substr(start, end - start));
}

std::string assembler::Parser::jump () {
  COMMAND type = commandType();
  if (type != C_COMMAND) {
    throw std::invalid_argument("Only c commands can have a dest."); 
  }
  if (currentCommand.find(';') != std::string::npos) {
    return Code::jump(currentCommand.substr(currentCommand.find(';') + 1));
  } else {
    return Code::jump("NULL");
  }
}

// TODO: allow for AMD to be in any order
// use address to increase speed
std::string assembler::Code::dest (const std::string& mnemonic) {
  if (mnemonic == "NULL") {
    return "000";
  } else if (mnemonic == "M") {
    return "001";
  } else if (mnemonic == "D") {
    return "010";
  } else if (mnemonic == "MD") {
    return "011";
  } else if (mnemonic == "A") {
    return "100";
  } else if (mnemonic == "AM") {
    return "101";
  } else if (mnemonic == "AD") {
    return "110";
  } else if (mnemonic == "AMD") {
    return "111";
  } else {
    throw std::invalid_argument("Invalid mnemonic \"" + mnemonic + "\" for dest command.");
  }
}

std::string assembler::Code::comp (const std::string& mnemonic) {
  bool mType = mnemonic.find('M') != std::string::npos;
  std::string compVal;
  if (mnemonic == "0") {
    compVal = "101010";
  } else if (mnemonic == "1") {
    compVal = "111111";
  } else if (mnemonic == "-1") {
    compVal = "111010";
  } else if (mnemonic == "D") {
    compVal = "001100";
  } else if ((mType && mnemonic == "M") || (!mType && mnemonic == "A")) {
    compVal = "110000";
  } else if (mnemonic == "!D") {
    compVal = "001101";
  } else if ((mType && mnemonic == "!M") || (!mType && mnemonic == "!A")) {
    compVal = "110001";
  } else if (mnemonic == "-D") {
    compVal = "001111";
  } else if ((mType && mnemonic == "-M") || (!mType && mnemonic == "-A")) {
    compVal = "110011";
  } else if (mnemonic == "D+1" || mnemonic == "1+D") {
    compVal = "011111";
  } else if ((mType && mnemonic == "M+1") || (mType && mnemonic == "1+M") || (!mType && mnemonic == "A+1") || (!mType && mnemonic == "1+A")) {
    compVal = "110111";
  } else if (mnemonic == "D-1") {
    compVal = "001110";
  } else if ((mType && mnemonic == "M-1") || (!mType && mnemonic == "A-1")) {
    compVal = "110010";
  } else if ((mType && mnemonic == "D+M") || (mType && mnemonic == "M+D") || (!mType && mnemonic == "D+A") || (!mType && mnemonic == "A+D")) {
    compVal = "000010";
  } else if ((mType && mnemonic == "D-M") || (!mType && mnemonic == "D-A")) {
    compVal = "010011";
  } else if ((mType && mnemonic == "M-D") || (!mType && mnemonic == "A-D")) {
    compVal = "000111";
  } else if ((mType && mnemonic == "D&M") || (mType && mnemonic == "M&D") || (!mType && mnemonic == "D&A") || (!mType && mnemonic == "A&D")) {
    compVal = "000000";
  } else if ((mType && mnemonic == "D|M") || (mType && mnemonic == "M|D") || (!mType && mnemonic == "D|A") || (!mType && mnemonic == "A|D")) {
    compVal = "010101";
  } else {
    throw std::invalid_argument("Invalid mnemonic \"" + mnemonic + "\" for comp command.");
  }

  return (mType ? "1" : "0") + compVal; 
}

std::string assembler::Code::jump (const std::string& mnemonic) {
  if (mnemonic == "NULL") {
    return "000";
  } else if (mnemonic == "JGT") {
    return "001";
  } else if (mnemonic == "JEG") {
    return "010";
  } else if (mnemonic == "JGE") {
    return "011";
  } else if (mnemonic == "JLT") {
    return "100";
  } else if (mnemonic == "JNE") {
    return "101";
  } else if (mnemonic == "JLE") {
    return "110";
  } else if (mnemonic == "JMP") {
    return "111";
  } else {
    throw std::invalid_argument("Invalid mnemonic \"" + mnemonic + "\" for jump command.");
  }
}

assembler::SymbolTable::SymbolTable () {
  table = std::map<std::string, int>();
  // R0 = 0, R1 = 1 ... R15 = 15
  table["R0"] = 0;
  table["R1"] = 1;
  table["R2"] = 2;
  table["R3"] = 3;
  table["R4"] = 4;
  table["R5"] = 5;
  table["R6"] = 6;
  table["R7"] = 7;
  table["R8"] = 8;
  table["R9"] = 9;
  table["R10"] = 10;
  table["R11"] = 11;
  table["R12"] = 12;
  table["R13"] = 13;
  table["R14"] = 14;
  table["R15"] = 15;

  table["SCREEN"] = 16384;
  table["KBD"] = 24576;
  
  table["SP"] = 0;
  table["LCL"] = 1;
  table["ARG"] = 2;
  table["THIS"] = 3;
  table["THAT"] = 4;
}

void assembler::SymbolTable::addEntry (std::string symbol, int address) {
  if (contains(symbol)) {
    throw std::invalid_argument("Symbol \"" + symbol + "\" already exists in the table.");
  }
  table[symbol] = address;
}

int assembler::SymbolTable::addEntry (std::string variable) {
  addEntry(variable, variableAddr);
  return variableAddr++;
}

bool assembler::SymbolTable::contains (std::string symbol) {
  return table.find(symbol) != table.end();
}

int assembler::SymbolTable::getAddress (std::string symbol) {
  return table.at(symbol);
}