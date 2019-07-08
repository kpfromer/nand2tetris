#include <string>
#include <math.h>
#include "compiler.h"

bool compiler::detail::isOp(const char & value) {
  return value == '+' ||
    value == '-' ||
    value == '*' ||
    value == '/' ||
    value == '&' ||
    value == '|' ||
    value == '<' ||
    value == '>' ||
    value == '=';
}

bool compiler::detail::isSymbol(const char & value) {
  return value == '{' || 
    value == '}' ||
    value == '[' ||
    value == ']' ||
    value == '(' ||
    value == ')' ||
    value == '.' ||
    value == ',' ||
    value == ';' ||
    value == '~' ||
    isOp(value);
}

bool compiler::detail::isKeyWord(const std::string & word) {
  return word == "class" ||
    word == "constructor" ||
    word == "function" ||
    word == "method" ||
    word == "field" ||
    word == "static" ||
    word == "var" ||
    word == "int" ||
    word == "char" ||
    word == "boolean" ||
    word == "void" ||
    word == "true" ||
    word == "false" ||
    word == "null" ||
    word == "this" ||
    word == "let" ||
    word == "do" ||
    word == "if" ||
    word == "else" ||
    word == "while" ||
    word == "return";
}

compiler::detail::Keyword compiler::detail::getKeyWord(const std::string & value) {
  if (value == "class") {
    return Keyword::CLASS;
  } else if (value == "constructor") {
    return Keyword::CONSTRUCTOR;
  } else if (value == "function") {
    return Keyword::FUNCTION;
  } else if (value == "method") {
    return Keyword::METHOD;
  } else if (value == "field") {
    return Keyword::FIELD;
  } else if (value == "static") {
    return Keyword::STATIC;
  } else if (value == "var") {
    return Keyword::VAR;
  } else if (value == "int") {
    return Keyword::INT;
  } else if (value == "char") {
    return Keyword::CHAR;
  } else if (value == "boolean") {
    return Keyword::BOOLEAN;
  } else if (value == "void") {
    return Keyword::VOID;
  } else if (value == "true") {
    return Keyword::TRUE;
  } else if (value == "false") {
    return Keyword::FALSE;
  } else if (value == "null") {
    return Keyword::NULLVAL;
  } else if (value == "this") {
    return Keyword::THIS;
  } else if (value == "let") {
    return Keyword::LET;
  } else if (value == "do") {
    return Keyword::DO;
  } else if (value == "if") {
    return Keyword::IF;
  } else if (value == "else") {
    return Keyword::ELSE;
  } else if (value == "while") {
    return Keyword::WHILE;
  } else if (value == "return") {
    return Keyword::RETURN;
  } else {
    throw std::invalid_argument("Invalid keyword value");
  }
}

std::string compiler::detail::getKeywordString(const Keyword & keyword) {
  if (keyword == Keyword::CLASS) {
    return "class";
  } else if (keyword == Keyword::CONSTRUCTOR) {
    return "constructor";
  } else if (keyword == Keyword::FUNCTION) {
    return "function";
  } else if (keyword == Keyword::METHOD) {
    return "method";
  } else if (keyword == Keyword::FIELD) {
    return "field";
  } else if (keyword == Keyword::STATIC) {
    return "static";
  } else if (keyword == Keyword::VAR) {
    return "var";
  } else if (keyword == Keyword::INT) {
    return "int";
  } else if (keyword == Keyword::CHAR) {
    return "char";
  } else if (keyword == Keyword::BOOLEAN) {
    return "boolean";
  } else if (keyword == Keyword::VOID) {
    return "void";
  } else if (keyword == Keyword::TRUE) {
    return "true";
  } else if (keyword == Keyword::FALSE) {
    return "false";
  } else if (keyword == Keyword::NULLVAL) {
    return "null";
  } else if (keyword == Keyword::THIS) {
    return "this";
  } else if (keyword == Keyword::LET) {
    return "let";
  } else if (keyword == Keyword::DO) {
    return "do";
  } else if (keyword == Keyword::IF) {
    return "if";
  } else if (keyword == Keyword::ELSE) {
    return "else";
  } else if (keyword == Keyword::WHILE) {
    return "while";
  } else if (keyword == Keyword::RETURN) {
    return "return";
  } else {
    throw std::invalid_argument("Invalid keyword value");
  }
}

bool compiler::detail::isDigit(const char & value) {
  return value == '0' ||
    value == '1' ||
    value == '2' ||
    value == '3' ||
    value == '4' ||
    value == '5' ||
    value == '6' ||
    value == '7' ||
    value == '8' ||
    value == '9';
}

int compiler::detail::getDigit(const char & value) {
  if (value == '0') {
    return 0;
  } else if (value == '1') {
    return 1;
  } else if (value == '2') {
    return 2;
  } else if (value == '3') {
    return 3;
  } else if (value == '4') {
    return 4;
  } else if (value == '5') {
    return 5;
  } else if (value == '6') {
    return 6;
  } else if (value == '7') {
    return 7;
  } else if (value == '8') {
    return 8;
  } else if (value == '9') {
    return 9;
  } else {
    throw std::invalid_argument("Invalid value for get digit");
  }
}

// Do note that * and / are NOT included because they rely on a call to the Math class
compiler::VMWriter::Arithmetic compiler::detail::getArithmeticType(const char & symbol) {
  if (symbol == '+') {
    return compiler::VMWriter::ADD;
  } else if (symbol == '-') {
    return compiler::VMWriter::SUB;
  } else if (symbol == '&') {
    return compiler::VMWriter::AND;
  } else if (symbol == '|') {
    return compiler::VMWriter::OR;
  } else if (symbol == '<') {
    return compiler::VMWriter::LT;
  } else if (symbol == '>') {
    return compiler::VMWriter::GT;
  } else if (symbol == '=') {
    return compiler::VMWriter::EQ;
  } else {
    throw std::invalid_argument("invalid arithmetic type symbol '" + std::string(1, symbol) + "'.");
  }
}

// The paritial Jack Character Set toCharValue (not including arrow keys, home, end, etc.)
unsigned int compiler::detail::toCharValue(const char & value) {
  if (value == ' ') {
    return 32;
  } else if (value == '!') {
    return 33;
  } else if (value == '"') {
    return 34;
  } else if (value == '#') {
    return 35;
  } else if (value == '$') {
    return 36;
  } else if (value == '%') {
    return 37;
  } else if (value == '&') {
    return 38;
  } else if (value == '\'') {
    return 39;
  } else if (value == '(') {
    return 40;
  } else if (value == ')') {
    return 41;
  } else if (value == '*') {
    return 42;
  } else if (value == '+') {
    return 43;
  } else if (value == ',') {
    return 44;
  } else if (value == '-') {
    return 45;
  } else if (value == '.') {
    return 46;
  } else if (value == '/') {
    return 47;
  } else if (value == ':') {
    return 58;
  } else if (value == ';') {
    return 59;
  } else if (value == '<') {
    return 60;
  } else if (value == '=') {
    return 61;
  } else if (value == '>') {
    return 62;
  } else if (value == '?') {
    return 63;
  } else if (value == '@') {
    return 64;
  } else if (value == '[') {
    return 91;
  } else if (value == '\\') {
    return 92;
  } else if (value == ']') {
    return 93;
  } else if (value == '^') {
    return 94;
  } else if (value == '_') {
    return 95;
  } else if (value == '`') {
    return 96;
  } else if (value == '{') {
    return 123;
  } else if (value == '|') {
    return 124;
  } else if (value == '}') {
    return 125;
  } else if (value == '~') {
    return 126;
  } else {
    std::string number = "0123456789";
    for (unsigned int i = 0; i < number.length(); i++) {
      if (number.at(i) == value) {
        return 48 + i;
      }
    }
    std::string lowerCase = "abcdefghijklmnopqrstuvwxyz";
    for (unsigned int i = 0; i < lowerCase.length(); i++) {
      if (lowerCase.at(i) == value) {
        return 97 + i;
      }
    }
    std::string upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (unsigned int i = 0; i < upperCase.length(); i++) {
      if (upperCase.at(i) == value) {
        return 65 + i;
      }
    }
  }
  throw std::invalid_argument("Character '" + std::string(1, value) +"' is not a valid Jack Character.");
}

compiler::VMWriter::Segment compiler::detail::convertKindToSegment(const compiler::SymbolicTable::Kind & kind) {
  if (kind == compiler::SymbolicTable::Kind::STATIC) {
    return compiler::VMWriter::Segment::STATIC;
  } else if (kind == compiler::SymbolicTable::Kind::ARGUMENT) {
    return compiler::VMWriter::Segment::ARG;
  } else if (kind == compiler::SymbolicTable::Kind::LOCAL) {
    return compiler::VMWriter::Segment::LOCAL;
  } else if (kind == compiler::SymbolicTable::Kind::FIELD) {
    return compiler::VMWriter::Segment::THIS;
  } else {
    throw std::invalid_argument("Can not convert invalid kind type to segment.");
  }
}

bool compiler::detail::isNumber(const std::string & word, const bool & decimal) {
  bool hasDecimal = false;
  for (unsigned int i = 0; i < word.size(); i++) {
    char value = word.at(i);
    if (decimal && value == '.') {
      if (hasDecimal) { // has multiple decimals
        return false;
      }
      hasDecimal = true;
    } else if (!isDigit(value)) {
      return false;
    }
  }
  return true;
}

// double getNumber(const std::string & numberString) {
//   double value = 0;
//   unsigned int start = numberString.size();
//   // Check and set start to decimal place position
//   size_t numberOfDecimals = std::count(numberString.begin(), numberString.end(), '.');
//   if (decimcal) {
//     if (numberOfDecimals > 1) {
//       throw std::invalid_argument("Too many decimal places in number string given to getNumber.");
//     }
//     if (numberOfDecimals = 1) {
//       auto decimalPlace = numberString.find('.');
//       start = decimalPlace;
//     }
//   } else {
//     if (numberOfDecimals > 0) {
//       throw std::invalid_argument("getNumber with decimcal false can not have a number with decimals.");
//     }
//   }
//   for (unsigned int i = 0; i < numberString.size(); i++) {
//     if (numberString.at(i) == '.') {
//       continue;
//     }
//     value += pow(10, start - 1 - i) * getDigit(numberString.at(i)); 
//   }
//   return value;
// };

// TODO: double type
unsigned int compiler::detail::getNumber(const std::string & numberString) {
  int value = 0;
  unsigned int start = numberString.size();
  for (unsigned int i = 0; i < numberString.size(); i++) {
    value += pow(10, start - 1 - i) * getDigit(numberString.at(i)); 
  }
  return value;
}