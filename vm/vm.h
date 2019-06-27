#ifndef __VM_H_
#define __VM_H_

#include <fstream>
#include <vector>
#include <string>
#include <map>

// Segment names
// Memory segments
#define SEG_LCL "local"
#define SEG_ARG "argument"
#define SEG_THIS "this"
#define SEG_THAT "that"
// Register segments
#define SEG_REG "reg" // used internally
#define SEG_TEMP "temp"
// #define SEG_REGISTER "register"
#define SEG_POINTER "pointer"
// Static segment
#define SEG_STATIC "static"
// Constant segment
#define SEG_CONSTANT "constant"

// Registers
#define REG_SP 0
#define REG_R0 0
#define REG_LCL 1
#define REG_ARG 2
#define REG_POINTER 3
#define REG_THIS 3
#define REG_THAT 4
#define REG_TEMP 5
// Used for copying
#define REG_COPY 13
// Used for return values
#define REG_FRAME 14
// Used for storing the return address
#define REG_RETURN 15

namespace vm {

  std::vector<std::string> tokenizer(const std::string& p_pcstStr, const char& delim);

  enum CommandType {
    C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL, C_COMMENT
  };

  struct Options {
    bool perserveComments = false;
    bool notes = false;
  };

  class Parser {
    private:
      std::vector<std::string> lines;
      void sanitize(std::string& value);
      Options options;
    public:
      Parser(const std::vector<std::string>& rawLines, const vm::Options & options = {});
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
      unsigned int functionCount = 0;
      std::ofstream outputFile;
      std::string outputFilename;
      vm::Options options;
      // Notes:
      // Glossary
        // reg = temporary register (which is stored in memory since we only have one real register)
      // Basic Operations
      void a_command(const std::string & value); // @value
      std::string jump(const std::string & comp, const std::string & jump); // creates the label and returns it comp;jump
      void jumpNoLabel(const std::string & comp, const std::string & jump);
      void c_command(const std::string & dest, const std::string & comp); // comp;jump
      void c_command(const std::string & dest, const std::string & comp, const std::string & jump); // dest=comp;jump
      void l_command(const std::string & label_name); // (label_name)
      std::string new_label(); // creates a unique label
      // Misc
      std::string static_name(const int & index);
      void value_at(const std::string & dest = "A"); // dest=M
      // Register operators
      std::string reg_name(const unsigned int & reg); // returns R#
      unsigned int reg_num(const std::string & seg, unsigned int i);
      unsigned int reg_base(const std::string & seg);
      void reg_to_dest(const unsigned int & reg, const std::string & dest); // dest = R#
      void comp_to_reg(const int & reg, const std::string & comp); // R# = comp
      void reg_to_reg(const unsigned int & src, const unsigned int & dest); // Rdest = Rsrc
      // Stack Operations
      void inc_sp(); // SP++
      void dec_sp(); // SP--
      void load_sp(); // A=SP (where the top of the stack is)
      void load_sp_offset(const int & offset); // A = D = (*SP) +/- offset
      // Handle loading of addresses
      void load_seg(const std::string & seg, const int & offset = 0, const bool & value_at = true); // A = D = seg +/- offset or if value_at = true, then A = D = *seg +/- offset
      void load_seg_offset(const std::string & seg, int offset, const bool & value_at); // A = D = seg +/- offset or if value_at = true, then A = D = *seg +/- offset
      void load_seg_no_offset(const std::string & seg, const bool & value_at); // A = D = seg, if value_at = true, then A = D = *seg
      // Store values onto stack (does not move SP)
      void val_to_stack(const int & value); // *SP = value
      void val_to_stack(const std::string & value); //*SP = value 
      void reg_to_stack(const std::string & segment, const unsigned int & reg); // *SP = R#
      void mem_to_stack(const std::string & segment, const int & offset, const bool & at_value = true); // *SP = *(seg + offset)
      void static_to_stack(const unsigned int & index); // *SP = func.# (the value at the static location
      void comp_to_stack(const std::string & comp); // *SP = comp
      // Get values from stack (does not move SP)
      void stack_to_reg(const std::string & seg, const unsigned int & reg); // R# = *SP
      void stack_to_mem(const std::string & segment, const int & offset, const bool & at_value = true); // *(seg + offset) = *SP
      void stack_to_static(const unsigned int & index); // func.# = *SP
      void stack_to_dest(const std::string & dest); // dest = *SP
      // Arithemtic and logical operations
      // Pop args off stack, then do something, finally push result on top of stack
      void unary(const std::string & comp); // Only deals with one value ie unary
      void binary(const std::string & comp); // Deals with two values ie binary
      void compare(const std::string & jump); // Compares two values, jump is the type of comparison (JNE, JLT, etc...)
      // stack push / pop
      void pop(const std::string & seg, const int & offset = 0);
      void pop_to_dest(const std::string & dest);
      void push(const std::string & seg, const int & offset = 0);
      void pushConstant(const std::string & constant);
      // Type Checks
      bool is_mem_seg(const std::string & seg);
      std::string mem_seg_name(const std::string & seg);
      bool is_reg_seg(const std::string & seg);
      bool is_static_seg(const std::string & seg);
      bool is_const_seg(const std::string & seg);
      // Frame
      void prev_frame_to_reg(const unsigned int & reg);
      // Etc.
      void writeNote(const std::string & message);
      void writeNoteEnd();
    public:
      // Opens the outputFile for writing
      CodeWriter(const std::string& path, const std::string& outputFilename, const vm::Options & options);
      ~CodeWriter();
      // writes the assembly instructions that effect the bootstrap code which initalizes the VM.
      void writeInit();
      // Writes the assembly code for the label command
      void writeLabel(const std::string& label);
      // Writes the assembly code for the goto command
      void writeGoto(const std::string& label);
      // Writes the assembly code for the if-goto command
      void writeIf(const std::string& label);
      // Writes the assembly code for the function command
      void writeFunction(const std::string& functionName, const unsigned int& numVars);
      // Writes the assembly code for the call command
      void writeCall(const std::string& functionName, const unsigned int& numArgs);
      // Writes the assembly code for the return command
      void writeReturn();
      // Writes the assembly version of the vm command
      void writeArithmetic(const std::string& command);
      // Writes the assembly push or pop off the stack
      void writePushPop(const CommandType& command, const std::string& segment, const unsigned int& index);
      // Writes the comment
      void writeComment(const std::string & comment);
      // Closes the file
      void close();
  };

  enum PATH_TYPE { DIRECTORY, FILE, NOTFOUND };

  void addFileLines(std::vector<std::string>& lines, const std::string& filePath);
  void concatDirectory(std::vector<std::string>& lines, const std::string& path, const bool& recursive = true);

  class Translator {
    private:
      static PATH_TYPE getType(const std::string& path);
      static std::string getPathBaseName(const std::string& path);
    public:
      static void translateFile(const std::string& filePath, const vm::Options & options);
      static void translateDirectory(const std::string& directoryPath, const vm::Options & options);
      static void translate(const std::string& path, const vm::Options & options);
  };
}

#endif