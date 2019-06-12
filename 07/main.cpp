#include <iostream>
#include <vector>

#include "vm.h"

int main(const int argc, const char ** arg) {
  if (argc != 3) {
    throw "Invalid args";
  }

  vm::Parser parser (arg[1]);
  vm::CodeWriter cw (arg[2]);

  while (parser.hasMoreCommands()) {
    vm::CommandType type = parser.commandType();
    if (type == vm::C_ARITHMETIC) {
      cw.writeArithmetic(parser.arg1());
    } else if (type == vm::C_POP || type == vm::C_PUSH) {
      cw.writePushPop(type, parser.arg1(), parser.arg2());
    }
    parser.advance();
  }

  // cw.writeArithmetic("add");
  // cw.writeArithmetic("sub");

  // cw.writePushPop(vm::C_PUSH, "constant", 256);
  // cw.writePushPop(vm::C_PUSH, "temp", 1);
  // cw.writePushPop(vm::C_PUSH, "local", 10);
  // cw.writePushPop(vm::C_PUSH, "static", 10);

  // cw.writePushPop(vm::C_PUSH, "pointer", 0);
  // cw.writePushPop(vm::C_PUSH, "pointer", 1);

  // cw.writePushPop(vm::C_POP, "local", 1);

  return 0;
}