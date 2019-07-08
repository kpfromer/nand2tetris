#include <map>
#include <string>
#include <iostream>
#include "compiler.h"

void forEach(std::map<std::string, compiler::SymbolicTable::Symbol> list, std::function<void(compiler::SymbolicTable::Symbol)> fn) {
  for (std::map<std::string, compiler::SymbolicTable::Symbol>::iterator it = list.begin(); it != list.end(); it++) {
    fn(it->second);
  }
}

void compiler::SymbolicTable::print() {
  auto print = [](Symbol value) {
    std::cout << "\t\tEntry - Name: " << value.name << " Type: " << value.type << " Kind: " << std::to_string(value.kind) << " Position: " << std::to_string(value.numberOfType) << std::endl;
  };
  std::cout << "Symbolic Table Values:" << std::endl;
  std::cout << "\tStatic Values:" << std::endl;
  forEach(staticScope, print);
  std::cout << "\tEnd Static Values" << std::endl;
  std::cout << "\tField Values:" << std::endl;
  forEach(fieldScope, print);
  std::cout << "\tEnd Field Values" << std::endl;
  std::cout << "\tSubroutine Values:" << std::endl;
  forEach(subroutineScope, print);
  std::cout << "\tEnd Subroutine Values" << std::endl;
  std::cout << "End of Symbolic Table" << std::endl;
}

void compiler::SymbolicTable::startSubroutine() {
  subroutineScope.clear();
  argumentCount = 0;
  localCount = 0;
}

void compiler::SymbolicTable::define(const std::string & name, const std::string & type, const compiler::SymbolicTable::Kind & kind) {
  if (staticScope.find(name) != staticScope.end() || fieldScope.find(name) != fieldScope.end() || subroutineScope.find(name) != subroutineScope.end()) {
    throw std::invalid_argument("Can not redefine a variable. Variable name '" + name + "'");
  }
  Symbol symbol;
  symbol.name = name;
  symbol.type = type;
  symbol.kind = kind;
  if (kind == STATIC) {
    symbol.numberOfType = staticCount++;
    staticScope.emplace(name, symbol);
  } else if (kind == FIELD) {
    symbol.numberOfType = fieldCount++;
    fieldScope.emplace(name, symbol);
  } else {
    if (kind == ARGUMENT) {
      symbol.numberOfType = argumentCount++;
    } else { // kind == LOCAL
      symbol.numberOfType = localCount++;
    }
    subroutineScope.emplace(name, symbol);
  }
}

bool compiler::SymbolicTable::exists(const std::string & name) {
  for (std::map<std::string, Symbol>::iterator it = subroutineScope.begin(); it != subroutineScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return true;
    }
  }
  for (std::map<std::string, Symbol>::iterator it = fieldScope.begin(); it != fieldScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return true;
    }
  }
  for (std::map<std::string, Symbol>::iterator it = staticScope.begin(); it != staticScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return true;
    }
  }
  return false;
}

compiler::SymbolicTable::Symbol compiler::SymbolicTable::getSymbol(const std::string & name) {
  for (std::map<std::string, Symbol>::iterator it = subroutineScope.begin(); it != subroutineScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return value;
    }
  }
  for (std::map<std::string, Symbol>::iterator it = fieldScope.begin(); it != fieldScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return value;
    }
  }
  for (std::map<std::string, Symbol>::iterator it = staticScope.begin(); it != staticScope.end(); it++) {
    Symbol value = it->second;
    if (value.name == name) {
      return value;
    }
  }
  throw std::invalid_argument("No variable by name '" + name + "' found.");
}

unsigned int compiler::SymbolicTable::varCount(const compiler::SymbolicTable::Kind & kind) {
  if (kind == STATIC) {
    return staticCount;
  } else if (kind == FIELD) {
    return fieldCount;
  } else if (kind == LOCAL) {
    return localCount;
  } else if (kind == ARGUMENT) {
    return argumentCount;
  }
  throw std::invalid_argument("Invalid var count type.");
}

compiler::SymbolicTable::Kind compiler::SymbolicTable::kindOf(const std::string & name) {
  return getSymbol(name).kind;
}

std::string compiler::SymbolicTable::typeOf(const std::string & name) {
  return getSymbol(name).type;
}

unsigned int compiler::SymbolicTable::indexOf(const std::string & name) {
  return getSymbol(name).numberOfType;
}