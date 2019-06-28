#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <string>
#include <functional>
#include "vm.h"

// TODO: improve space concerns by using short instead of int

void vm::CodeWriter::a_command(const std::string & value) {
  output.push_back("@" + value + "\n");
}

// Jumps to new label and returns the new label for later definiton
std::string vm::CodeWriter::jump(const std::string & comp, const std::string & jump) {
  std::string label = new_label();
  a_command(label);
  output.push_back(comp + ";" + jump + "\n");
  return label;
}

void vm::CodeWriter::jumpNoLabel(const std::string & comp, const std::string & jump) {
  output.push_back(comp + ";" + jump + "\n");
}

void vm::CodeWriter::c_command(const std::string & dest, const std::string & comp) {
  output.push_back(dest + "=" + comp + "\n");
}

void vm::CodeWriter::c_command(const std::string & dest, const std::string & comp, const std::string & jump) {
  output.push_back(dest + "=" + comp + ";" + jump + "\n");
}

void vm::CodeWriter::l_command(const std::string & label_name) {
  output.push_back("(" + label_name + ")" + "\n");
}

// Just returns a random label name
std::string vm::CodeWriter::new_label() {
  return "LABEL" + std::to_string(labelCount++);
}

std::string vm::CodeWriter::static_name(const int & index) {
  return outputFilename + "." + std::to_string(index);
}

void vm::CodeWriter::value_at(const std::string & dest) {
  c_command(dest, "M"); // dest=M
}

std::string vm::CodeWriter::reg_name(const unsigned int & reg) {
  return "R" + std::to_string(reg); // R#
}

unsigned int vm::CodeWriter::reg_num(const std::string & seg, unsigned int i) {
  return reg_base(seg) + i;
}

unsigned int vm::CodeWriter::reg_base(const std::string & seg) {
  if (seg == "pointer") {
    return REG_POINTER;
  } else if (seg == "temp") {
    return REG_TEMP;
  } else if (seg == "reg") {
    return REG_R0;
  } else {
    throw std::invalid_argument("invalid register segment");
  }
}

void vm::CodeWriter::reg_to_dest(const unsigned int & reg, const std::string & dest) {
  a_command(reg_name(reg)); // @R#
  c_command(dest, "M"); // dest = R#
} // dest = R#

void vm::CodeWriter::comp_to_reg(const int & reg, const std::string & comp) {
  a_command(reg_name(reg)); // @R#
  c_command("M", comp); // R# = comp
} // R# = comp

void vm::CodeWriter::reg_to_reg(const unsigned int & src, const unsigned int & dest) {
  reg_to_dest(src, "D"); // D = #Rsrc
  comp_to_reg(dest, "D"); // #Rdest = D
} // #Rdest = #Rsrc

void vm::CodeWriter::inc_sp() {
  a_command("SP"); // A = &SP
  c_command("M", "M+1"); // SP = SP + 1
} // SP++

void vm::CodeWriter::dec_sp() {
  a_command("SP"); // A = &SP
  c_command("M", "M-1"); // SP = SP - 1
} // SP--

void vm::CodeWriter::load_sp() {
  a_command("SP"); // A = &SP
  c_command("A", "M"); // A=SP
} // A = SP (where the top of the stack is)

void vm::CodeWriter::load_sp_offset(const int & offset) {
  load_seg("SP", offset);
} // A = D = (*SP) +/- offset

void vm::CodeWriter::load_seg(const std::string & seg, const int & offset, const bool & value_at) {
  if (offset == 0) {
    load_seg_no_offset(seg, value_at);
  } else {
    load_seg_offset(seg, offset, value_at);
  }
} // A = D = seg +/- offset or if value_at = true, then A = D = *seg +/- offset

void vm::CodeWriter::load_seg_offset(const std::string & seg, int offset, const bool & value_at) {
  std::string comp = "A+D";
  if (offset < 0) {
    offset = -offset;
    comp = "A-D";
  }
  a_command(std::to_string(offset)); // A = offset
  c_command("D", "A"); // D = offset = A
  a_command(seg); // A = seg
  if (value_at) { this->value_at(); } // A = *seg
  c_command("AD", comp);
} // A = D = seg +/- offset or if value_at = true, then A = D = *seg +/- offset

void vm::CodeWriter::load_seg_no_offset(const std::string & seg, const bool & value_at) {
  a_command(seg); // A = seg
  if (value_at) { this->value_at("AD"); } // A = D = *seg
} // A = D = seg, if value_at = true, then A = D = *seg

// Store values to stack

void vm::CodeWriter::val_to_stack(const std::string & value) {
  a_command(value); // @value
  c_command("D", "A"); // D = value
  comp_to_stack("D"); // *SP = D
} // *SP = value

void vm::CodeWriter::val_to_stack(const int & value) {
  a_command(std::to_string(value)); // @value
  c_command("D", "A"); // D = value
  comp_to_stack("D"); // *SP = D
} // *SP = value

void vm::CodeWriter::reg_to_stack(const std::string & seg, const unsigned int & reg) {
  reg_to_dest(reg_num(seg, reg), "D"); // D = R#
  comp_to_stack("D"); // *SP = D
} // *SP = R#

void vm::CodeWriter::mem_to_stack(const std::string & segment, const int & offset, const bool & at_value) {
  load_seg(segment, offset, at_value);
  c_command("D", "M");
  comp_to_stack("D"); // *SP = D
} // *SP = *(seg + offset)

void vm::CodeWriter::static_to_stack(const unsigned int & index) {
  a_command(static_name(index)); // A = &func.#
  c_command("D", "M");
  comp_to_stack("D");
} // *SP = func.# (the value at the static location

void vm::CodeWriter::comp_to_stack(const std::string & comp) {
  load_sp(); // A = SP
  c_command("M", comp); // *SP = comp
} // *SP = comp

// Get values from stack

void vm::CodeWriter::stack_to_reg(const std::string & seg, const unsigned int & reg) {
  stack_to_dest("D");
  comp_to_reg(reg_num(seg, reg), "D");
} // R# = *SP

void vm::CodeWriter::stack_to_mem(const std::string & segment, const int & offset, const bool & at_value) {
  load_seg(segment, offset, at_value);
  comp_to_reg(REG_COPY, "D"); // R_COPY = D
  stack_to_dest("D"); // D = *SP
  reg_to_dest(REG_COPY, "A"); // A=R_COPY
  c_command("M", "D"); // *(seg + offset) = *SP
} // *(seg + offset) = *SP

void vm::CodeWriter::stack_to_static(const unsigned int & index) {
  stack_to_dest("D");
  a_command(static_name(index));
  c_command("M", "D");
} // func.# = *SP

void vm::CodeWriter::stack_to_dest(const std::string & dest) {
  load_sp(); // A = SP
  c_command(dest, "M"); // dest = *SP
} // dest = *SP

// Pop/push

void vm::CodeWriter::unary(const std::string & comp) {
  dec_sp(); // SP--
  stack_to_dest("D"); // D = *SP
  c_command("D", comp);
  comp_to_stack("D"); // D = comp (some operation that uses *SP, which is D)
  inc_sp(); // SP++
} // Only deals with one value ie unary

void vm::CodeWriter::binary(const std::string & comp) {
  dec_sp(); // SP--
  stack_to_dest("D"); // D = *SP
  dec_sp(); // SP--
  stack_to_dest("A"); // A = *SP
  c_command("D", comp);
  comp_to_stack("D"); // D = comp (some operation that uses *SP, which is D, and *SP = A, which will be M)
  inc_sp(); // SP++
} // Deals with two values ie binary

void vm::CodeWriter::compare(const std::string & jump) {
  dec_sp(); // SP--
  stack_to_dest("D"); // D = *SP
  dec_sp(); // SP--
  stack_to_dest("A"); // A = *SP
  c_command("D", "A-D"); // D = A - D
  std::string label_eq = this->jump("D", jump); // D;jump to label_eq
  comp_to_stack("0"); // *SP = 0 = false
  std::string label_ne = this->jump("0", "JMP"); // jump to label_ne
  l_command(label_eq);
  comp_to_stack("-1"); // *SP = -1 = true
  l_command(label_ne);
  inc_sp(); // SP++
} // Compares two values, jump is the type of comparison (JNE, JLT, etc...)

void vm::CodeWriter::pop(const std::string & seg, const int & offset) {
  dec_sp();
  if (is_mem_seg(seg)) {
    stack_to_mem(mem_seg_name(seg), offset);
  } else if (is_reg_seg(seg)) {
    stack_to_reg(seg, offset);
  } else if (is_static_seg(seg)) {
    stack_to_static(offset);
  } else {
    throw std::invalid_argument("Invalid pop segment");
  }
}

void vm::CodeWriter::pop_to_dest(const std::string & dest) {
  dec_sp();
  stack_to_dest(dest);
}

void vm::CodeWriter::pushConstant(const std::string & constant) {
  val_to_stack(constant);
  inc_sp();
}

void vm::CodeWriter::push(const std::string & seg, const int & offset) {
  if (is_mem_seg(seg)) {
    mem_to_stack(mem_seg_name(seg), offset);
  } else if (is_reg_seg(seg)) {
    reg_to_stack(seg, offset);
  } else if (is_static_seg(seg)) {
    static_to_stack(offset);
  } else if (is_const_seg(seg)) {
    val_to_stack(offset);
  } else {
    throw std::invalid_argument("Invalid push segment");
  }
  inc_sp();
}

bool vm::CodeWriter::is_mem_seg(const std::string & seg) {
  return seg == SEG_LCL || seg == SEG_ARG || seg == SEG_THIS || seg == SEG_THAT;
}

std::string vm::CodeWriter::mem_seg_name(const std::string & seg) {
  if (seg == SEG_LCL) {
    return "LCL";
  } else if (seg == SEG_ARG) {
    return "ARG";
  } else if (seg == SEG_THIS) {
    return "THIS";
  } else if (seg == SEG_THAT) {
    return "THAT";
  } else {
    throw std::invalid_argument("Invalid memory segment name.");
  }
}

bool vm::CodeWriter::is_reg_seg(const std::string & seg) {
  return seg == SEG_TEMP || seg == SEG_REG || seg == SEG_POINTER;
}

bool vm::CodeWriter::is_static_seg(const std::string & seg) {
  return seg == SEG_STATIC;
}

bool vm::CodeWriter::is_const_seg(const std::string & seg) {
  return seg == SEG_CONSTANT;
}

void vm::CodeWriter::writeNote(const std::string & message) {
  if (options.notes) {
    output.push_back("// " + message + "\n");
  }
}

void vm::CodeWriter::writeNoteEnd() {
  if (options.notes) {
  }
}

// Opens the outputFile for writing
vm::CodeWriter::CodeWriter(std::vector<std::string> & outputLines, const std::string& outputFilename, const vm::Options & options): output(outputLines) {
  // this->output = output;
  this->options = options;
  this->outputFilename = outputFilename;
}
// writes the assembly instructions that effect the bootstrap code which initalizes the VM.
void vm::CodeWriter::writeInit() {
  writeNote("Initalize");
  // SP = 256
  a_command("256");
  c_command("D", "A");
  comp_to_reg(REG_SP, "D");
  // call Sys.init
  writeCall("Sys.init", 0);
  writeNoteEnd();
}
// Writes the assembly code for the label command
void vm::CodeWriter::writeLabel(const std::string& label) {
  writeNote("label " + label);
  l_command(label);
  writeNoteEnd();
}
// Writes the assembly code for the goto command
void vm::CodeWriter::writeGoto(const std::string& label) {
  writeNote("goto " + label);
  a_command(label);
  jumpNoLabel("0", "JMP");
  writeNoteEnd();
}
// Writes the assembly code for the if-goto command
void vm::CodeWriter::writeIf(const std::string& label) {
  writeNote("if-goto " + label);
  pop_to_dest("D");
  a_command(label);
  jumpNoLabel("D", "JNE");
  writeNoteEnd();
}
// Writes the assembly code for the function command
void vm::CodeWriter::writeFunction(const std::string& functionName, const unsigned int& numVars) {
  writeNote("function " + functionName + " " + std::to_string(numVars));
  writeLabel(functionName);
  for (unsigned int i = 0; i < numVars; i++) {
    writePushPop(C_PUSH, SEG_CONSTANT, 0);
  }
  writeNoteEnd();
}
// Writes the assembly code for the call command
void vm::CodeWriter::writeCall(const std::string& functionName, const unsigned int& numArgs) {
  writeNote("call " + functionName + " " + std::to_string(numArgs));
  std::string returnAddress = new_label();
  // Add the return address to get back to caller (the return label will be converted to a line number by the assembler)
  pushConstant(returnAddress);
  // Save LCL of the caller
  push(SEG_REG, REG_LCL);
  // Save ARG of the caller
  push(SEG_REG, REG_ARG);
  // Save THIS of the caller
  push(SEG_REG, REG_THIS);
  // Save THAT of the caller
  push(SEG_REG, REG_THAT);
  // Repositions ARG (Moves ARG to the first argument passed to the calle)
  // The 5 is number of values added to stack above (return address, saved LCL, saved ARG, saved THIS, saved THAT)
  load_sp_offset(-numArgs - 5); // D = SP - 5 - numArgs
  comp_to_reg(REG_ARG, "D"); // ARG = D = SP - 5 - numArgs
  // Repositions LCL (Moves LCL to the end of the saved state (all of this stuff))
  reg_to_reg(REG_SP, REG_LCL); // LCL = SP = end of the saved state
  // Transfers control to the called function
  writeGoto(functionName);
  // Creates the return label used to move the program back up to caller after the call has finished returning it's value
  writeLabel(returnAddress);
  writeNoteEnd();
}
// Writes the assembly code for the return command
void vm::CodeWriter::writeReturn() {
  writeNote("return");
  reg_to_reg(REG_LCL, REG_FRAME); // D = FRAME = LCL
  // REG_RETURN = return address
  a_command("5"); // A = 5
  c_command("A", "D-A"); // A = FRAME - 5
  c_command("D", "M"); // D = *(FRAME - 5)
  comp_to_reg(REG_RETURN, "D"); // RETURN = *(FRAME - 5) = D
  // *ARG = pop() sets the *ARG (the first arg pushed by the caller) to the return value
  // SP = ARG + 1 repositions the stack back to caller (which is after the return value)
  pop(SEG_ARG, 0); // *ARG = return value
  reg_to_dest(REG_ARG, "D");
  comp_to_reg(REG_SP, "D+1");
  // THAT = *(endFrame - 1) restores THAT of the caller
  prev_frame_to_reg(REG_THAT);
  // THIS = *(endFrame - 2) restores THIS of the caller
  prev_frame_to_reg(REG_THIS);
  // ARG = *(endFrame - 3) restores ARG of the caller
  prev_frame_to_reg(REG_ARG);
  // LCL = *(endFrame - 4) restores LCL of the caller
  prev_frame_to_reg(REG_LCL);
  // goto retAddr Go back to the caller return address
  reg_to_dest(REG_RETURN, "A");
  jumpNoLabel("0", "JMP");
  writeNoteEnd();
}
void vm::CodeWriter::writeComment(const std::string & comment) {
  output.push_back("// " + comment + "\n");
}
void vm::CodeWriter::prev_frame_to_reg(const unsigned int & reg) {
  reg_to_dest(REG_FRAME, "D"); // D = FRAME
  c_command("D", "D-1"); // D = FRAME-1
  comp_to_reg(REG_FRAME, "D"); // FRAME = FRAME-1
  c_command("A", "D"); // A = FRAME-1
  c_command("D", "M"); // D = *(FRAME-1)
  comp_to_reg(reg, "D"); // reg = D
}
// Writes the assembly version of the vm command
void vm::CodeWriter::writeArithmetic(const std::string& command) {
  writeComment(command);
  if (command == "add") {
    binary("D+A");
  } else if (command == "sub") {
    binary("A-D");
  } else if (command == "neg") {
    unary("-D");
  } else if (command == "eq") {
    compare("JEQ");
  } else if (command == "gt") {
    compare("JGT");
  } else if (command == "lt") {
    compare("JLT");
  } else if (command == "and") {
    binary("D&A");
  } else if (command == "or") {
    binary("D|A");
  } else if (command == "not") {
    unary("!D");
  } else {
    throw std::invalid_argument("Invalid arithemtic command.");
  }
}
// Writes the assembly push or pop off the stack
void vm::CodeWriter::writePushPop(const CommandType& command, const std::string& segment, const unsigned int& index) {
  if (command == C_POP) {
    pop(segment, index);
  } else if (command == C_PUSH) {
    push(segment, index);
  } else {
    throw std::invalid_argument("Invalid command type for push pop.");
  }
}


void vm::addFileLines(std::vector<std::string>& lines, const std::string& filePath) {
  std::string line;
  std::ifstream file (filePath);
  if (file.is_open()) {
    while (getline(file, line)) {
      lines.push_back(line);
    }
    file.close();
  } else {
    std::cout << "Can not read file" << std::endl;
  }
}

void vm::forFile(const std::string & path, const bool & recursive, const std::function<void(std::string)> & fn) {
  if (boost::filesystem::is_directory(path)) {
    for(auto & p : boost::filesystem::directory_iterator(path)) {
      const std::string subPath = p.path().string();
      if (recursive && boost::filesystem::is_directory(subPath)) {
        forFile(subPath, recursive, fn);
      } else if (boost::filesystem::is_regular_file(subPath) && boost::filesystem::extension(subPath) == ".vm") { // Note: this checks that the file has an extenstion
        fn(subPath);
      }
    }
  } else if (boost::filesystem::is_regular_file(path)) {
    fn(path);
  } else {
    throw std::invalid_argument("Can not concat a directory that does not exist.");
  }
}

vm::PATH_TYPE vm::Translator::getType(const std::string& path) {
  // boost exists works for both files and directories
  if (!boost::filesystem::exists(path)) {
    return vm::PATH_TYPE::NOTFOUND;
  } else if (boost::filesystem::is_directory(path)) {
    return vm::PATH_TYPE::DIRECTORY;
  } else {
    return vm::PATH_TYPE::FILE;
  }
}

std::string vm::Translator::getPathBaseName(const std::string& path) {
  vm::PATH_TYPE type = getType(path);
  if (type == vm::PATH_TYPE::FILE) {
    boost::filesystem::path p (path);
    // Gets the filename with extension (removes the path to the file)
    std::string basename = p.filename().string();
    // Removes the filename extenstion
    return basename.substr(0, basename.find_last_of('.'));
  } else if (type == vm::PATH_TYPE::DIRECTORY) {
    boost::filesystem::path p (path);
    if (p.filename().string() == ".") {
      if (!p.has_parent_path()) {
        throw std::invalid_argument("Can not create a filename for directory with not parent.");
      }
      return p.parent_path().filename().string();
    }
    // Gets the directory name
    return p.filename().string();
  } else {
    throw std::invalid_argument("File or directory by path \"" + path + "\" was not found.");
  }
}

void vm::Translator::translateFile(std::vector<std::string> & output, const std::string& filePath, const vm::Options & options) {
  std::vector<std::string> lines;
  addFileLines(lines, filePath);

  std::string filename = getPathBaseName(filePath);
  // TODO: make getPathBaseName return path instead to save time coverting string backt to path
  boost::filesystem::path filenamePath (filename);
  boost::filesystem::path fullPath (filePath);
  // Joins the directory of the file with the new filename

  vm::Parser parser (lines, options);
  vm::CodeWriter cw (output, filename, options);

  if (options.writeInit) {
    cw.writeInit();
  }
  while (parser.hasMoreCommands()) {
    vm::CommandType type = parser.commandType();
    if (type == vm::C_ARITHMETIC) {
      cw.writeArithmetic(parser.arg1());
    } else if (type == vm::C_POP || type == vm::C_PUSH) {
      cw.writePushPop(type, parser.arg1(), parser.arg2());
    } else if (type == vm::C_GOTO) {
      cw.writeGoto(parser.arg1());
    } else if (type == vm::C_IF) {
      cw.writeIf(parser.arg1());
    } else if (type == vm::C_FUNCTION) {
      cw.writeFunction(parser.arg1(), parser.arg2());
    } else if (type == vm::C_LABEL) {
      cw.writeLabel(parser.arg1());
    } else if (type == vm::C_CALL) {
      cw.writeCall(parser.arg1(), parser.arg2());
    } else if (type == vm::C_RETURN) {
      cw.writeReturn();
    } else if (type == vm::C_COMMENT) {
      cw.writeComment(parser.arg1());
    }
    parser.advance();
  }
}

void vm::Translator::translateFile(const std::string& filePath, const vm::Options & options) {
  std::vector<std::string> outputLines;
  
  // Get the assembly code
  translateFile(outputLines, filePath, options);

  // Get write file location
  std::string filename = getPathBaseName(filePath);
  boost::filesystem::path fullPath (filePath);
  // Joins the directory of the file with the new filename
  std::string output = (fullPath.parent_path() / filename).string() + ".asm";

  // Write to file
  std::ofstream outputFile (output);
  for (unsigned int i = 0; i < outputLines.size(); i++) {
    outputFile << outputLines.at(i);
  }
  // Makes sure everything is written to the file
  outputFile.flush();
  outputFile.close();
}

void vm::Translator::translateDirectory(const std::string& directoryPath, const vm::Options & options) {
  std::vector<std::string> outputLines;
  bool writtenInit = false;
  Options postInitOptions;
  postInitOptions.notes = options.notes;
  postInitOptions.perserveComments = options.perserveComments;
  postInitOptions.writeInit = false;
  // Translates each file and adds to outputLines (maintaining their static file names!)
  auto translateEachFile = [&](std::string path) {
    if (writtenInit && options.writeInit) {
      // Already has written the init code, no needed to rewrite it again.
      translateFile(outputLines, path, postInitOptions);
    } else {
      writtenInit = true;
      translateFile(outputLines, path, options);
    }
  };
  forFile(directoryPath, true, translateEachFile);

  // Get write file location
  std::string filename = getPathBaseName(directoryPath);
    // TODO: make getPathBaseName return path instead to save time coverting string backt to path
  boost::filesystem::path filenamePath (filename);
  boost::filesystem::path fullPath (directoryPath);
  // Joins the directory of the file with the new directory filename
  std::string output = (fullPath.parent_path() / filename).string() + ".asm";

  // Write to file
  std::ofstream outputFile (output);
  for (unsigned int i = 0; i < outputLines.size(); i++) {
    outputFile << outputLines.at(i);
  }
  // Makes sure everything is written to the file
  outputFile.flush();
  outputFile.close();
}

void vm::Translator::translate(const std::string& path, const vm::Options & options) {
  const vm::PATH_TYPE type = getType(path);
  if (type == vm::PATH_TYPE::DIRECTORY) {
    translateDirectory(path, options);
  } else if (type == vm::PATH_TYPE::FILE) {
    translateFile(path, options);
  } else {
    throw std::invalid_argument("File or directory by path \"" + path + "\" was not found.");
  }
}

// TODO: reserved keywords?

std::vector<std::string> vm::tokenizer(const std::string& p_pcstStr, const char& delim)  {
  std::vector<std::string> tokens;
  std::stringstream mySstream( p_pcstStr );
  std::string temp;

  while(getline(mySstream, temp, delim)) {
    tokens.push_back(temp);
  }

  return tokens;
}

void removeCharsFromString(std::string &str, const std::string& charsToRemove) {
  for (unsigned int i = 0; i < charsToRemove.length(); ++i) {
    str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
  }
}

bool isCommentOnly(std::string line) {
  std::size_t commentPos = line.find("//", 0);
  if (commentPos != std::string::npos) {
    removeCharsFromString(line, " "); // Remove all spaces to see if there is any text in front of the comment
    if (line.find("//", 0) == 0) { // If there is not text in front of the comment
      return true;
    }
  }
  return false;
}

void vm::Parser::sanitize(std::string& line) {
  removeCharsFromString(line, "\n\r");
  if (!(options.perserveComments && isCommentOnly(line))) { // If not a full line comment
    line = line.substr(0, line.find("//", 0)); // Remove comment
  }
}

vm::Parser::Parser(const std::vector<std::string>& rawLines, const vm::Options & options) {
  this->options = options;
  std::string line;
  for (unsigned int i = 0; i < rawLines.size(); i++) {
    line = rawLines.at(i);
    sanitize(line);
    if (line.length() > 0) {
      lines.push_back(line);
    }
  }
}

bool vm::Parser::hasMoreCommands() {
  return lines.size() > position;
}

void vm::Parser::advance() {
  position++;
}

vm::CommandType vm::Parser::commandType() {
  std::vector<std::string> split = tokenizer(lines.at(position), ' ');
  const std::string firstArg = split.at(0);
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
  } else if (firstArg == "if-goto") {
    return C_IF;
  } else if (firstArg == "goto") {
    return C_GOTO;
  } else if (firstArg == "call") {
    return C_CALL;
  } else if (firstArg == "function") {
    return C_FUNCTION;
  } else if (firstArg == "return") {
    return C_RETURN;
  } else if (firstArg == "label") {
    return C_LABEL;
  } else if (isCommentOnly(lines.at(position))) {
    return C_COMMENT;
  } else {
    throw std::invalid_argument("Invalid operator \"" + firstArg + "\"");
  }
}

// TODO: check is there
std::string vm::Parser::arg1() {
  CommandType type = commandType();
  if (type == C_RETURN) {
    throw std::invalid_argument("arg1 can not be called if command type is return.");
  }
  std::vector<std::string> split = tokenizer(lines.at(position), ' ');
  if (type == C_ARITHMETIC) {
    return split.at(0);
  } else if (type == C_COMMENT) {
    return lines.at(position);
  } else {
    return split.at(1);
  }
}

// TODO: check is there
int vm::Parser::arg2() {
  CommandType type = commandType();
  if (type == C_POP || type == C_PUSH || type == C_FUNCTION || type == C_CALL) {
    std::vector<std::string> split = tokenizer(lines.at(position), ' ');
    return std::stoi(split.at(2));
  } else {
    throw std::invalid_argument("Invalid command type for arg2 call.");
  }
}