#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <vector>
#include <string>
#include <map>

namespace assembler {
  enum COMMAND {
    A_COMMAND,
    C_COMMAND,
    L_COMMAND
  };

  bool is_number(const std::string&);

  class Assembler {
    private:
      std::vector<std::string> parsedLines = {};
      void sanitize(std::string& line);
      void parseLines(const std::vector<std::string>& lines);
    public:
      void fromFile(const std::string& filename);
      void fromString(const std::string& string);
      void toFile(const std::string& filename);
      std::string toString();
  };

  // TODO: doc
  class Parser {
    private:
      std::vector<std::string> lines;
      std::string currentCommand;
    public:
      unsigned int currentLine = 0;
      Parser(std::vector<std::string> lines);
      Parser(std::string filename);
      bool hasMoreCommands();
      void advance();
      COMMAND commandType();
      std::string symbol();
      std::string dest();
      std::string comp();
      std::string jump();
  };

  class Code {
    public:
      // Returns the destination bits from the string
      static std::string dest(const std::string& mnemonic); // 3 bits
      static std::string comp(const std::string& mnemonic); // 7 bits
      static std::string jump(const std::string& mnemonic); // 3 bits
  };

  class SymbolTable {
    private:
      std::map<std::string, int> table;
      unsigned int variableAddr = 16;
    public:
      // Creates empty symbol table
      SymbolTable();
      // Adds a (symbol, address) pair to the table
      void addEntry(std::string symbol, int address);
      int addEntry(std::string variable);
      // Returns if symbol exists
      bool contains(std::string symbol);
      // Gets the address of symbol
      int getAddress(std::string symbol);
  };
}

#endif