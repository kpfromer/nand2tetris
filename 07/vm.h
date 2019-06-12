#ifndef __VM_H_
#define __VM_H_

#include <fstream>
#include <vector>
#include <string>
#include <map>

namespace vm {

  std::vector<std::string> tokenizer(const std::string& p_pcstStr, const char& delim);

  enum CommandType {
    C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
  };

  class Parser {
    private:
      std::vector<std::string> lines;
      void sanitize(std::string& value);
    public:
      Parser(const std::string& inputFilename);
      // ~Parser();
      unsigned int position = 0;
      bool hasMoreCommands();
      void advance();
      CommandType commandType();
      std::string arg1();
      int arg2();
  };

  class CodeWriter {
    private:
      unsigned int labelCount = 0;
      std::ofstream outputFile;
      std::string outputFilename;
    public:
      // Opens the outputFile for writing
      CodeWriter(const std::string& outputFilename);
      ~CodeWriter();
      // Writes the assembly version of the vm command
      void writeArithmetic(const std::string& command);
      // Writes the assembly push or pop off the stack
      void writePushPop(const CommandType& command, const std::string& segment, const unsigned int& index);
      // Closes the file
      void close();
  };
}

#endif