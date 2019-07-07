#include <string>
#include <iostream>
#include <map>
#include <math.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include "compiler.h"

bool isOp(const char & value) {
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

bool isSymbol(const char & value) {
  return value == '{' || 
    value == '}' ||
    value == '[' ||
    value == ']' ||
    value == '(' ||
    value == ')' ||
    value == '.' ||
    value == ',' ||
    value == ';' ||
    value == '~' || isOp(value);
}

bool isKeyWord(const std::string & word) {
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

compiler::Keyword getKeyWord(const std::string & value) {
  if (value == "class") {
    return compiler::Keyword::CLASS;
  } else if (value == "constructor") {
    return compiler::Keyword::CONSTRUCTOR;
  } else if (value == "function") {
    return compiler::Keyword::FUNCTION;
  } else if (value == "method") {
    return compiler::Keyword::METHOD;
  } else if (value == "field") {
    return compiler::Keyword::FIELD;
  } else if (value == "static") {
    return compiler::Keyword::STATIC;
  } else if (value == "var") {
    return compiler::Keyword::VAR;
  } else if (value == "int") {
    return compiler::Keyword::INT;
  } else if (value == "char") {
    return compiler::Keyword::CHAR;
  } else if (value == "boolean") {
    return compiler::Keyword::BOOLEAN;
  } else if (value == "void") {
    return compiler::Keyword::VOID;
  } else if (value == "true") {
    return compiler::Keyword::TRUE;
  } else if (value == "false") {
    return compiler::Keyword::FALSE;
  } else if (value == "null") {
    return compiler::Keyword::NULLVAL;
  } else if (value == "this") {
    return compiler::Keyword::THIS;
  } else if (value == "let") {
    return compiler::Keyword::LET;
  } else if (value == "do") {
    return compiler::Keyword::DO;
  } else if (value == "if") {
    return compiler::Keyword::IF;
  } else if (value == "else") {
    return compiler::Keyword::ELSE;
  } else if (value == "while") {
    return compiler::Keyword::WHILE;
  } else if (value == "return") {
    return compiler::Keyword::RETURN;
  } else {
    throw std::invalid_argument("Invalid keyword value");
  }
}

std::string getKeywordString(const compiler::Keyword & keyword) {
  if (keyword == compiler::Keyword::CLASS) {
    return "class";
  } else if (keyword == compiler::Keyword::CONSTRUCTOR) {
    return "constructor";
  } else if (keyword == compiler::Keyword::FUNCTION) {
    return "function";
  } else if (keyword == compiler::Keyword::METHOD) {
    return "method";
  } else if (keyword == compiler::Keyword::FIELD) {
    return "field";
  } else if (keyword == compiler::Keyword::STATIC) {
    return "static";
  } else if (keyword == compiler::Keyword::VAR) {
    return "var";
  } else if (keyword == compiler::Keyword::INT) {
    return "int";
  } else if (keyword == compiler::Keyword::CHAR) {
    return "char";
  } else if (keyword == compiler::Keyword::BOOLEAN) {
    return "boolean";
  } else if (keyword == compiler::Keyword::VOID) {
    return "void";
  } else if (keyword == compiler::Keyword::TRUE) {
    return "true";
  } else if (keyword == compiler::Keyword::FALSE) {
    return "false";
  } else if (keyword == compiler::Keyword::NULLVAL) {
    return "null";
  } else if (keyword == compiler::Keyword::THIS) {
    return "this";
  } else if (keyword == compiler::Keyword::LET) {
    return "let";
  } else if (keyword == compiler::Keyword::DO) {
    return "do";
  } else if (keyword == compiler::Keyword::IF) {
    return "if";
  } else if (keyword == compiler::Keyword::ELSE) {
    return "else";
  } else if (keyword == compiler::Keyword::WHILE) {
    return "while";
  } else if (keyword == compiler::Keyword::RETURN) {
    return "return";
  } else {
    throw std::invalid_argument("Invalid keyword value");
  }
}

bool isDigit(const char & value) {
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

int getDigit(const char & value) {
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
compiler::VMWriter::Arithmetic getArithmeticType(const char & symbol) {
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
unsigned int toCharValue(const char & value) {
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

compiler::VMWriter::Segment convertKindToSegment(const compiler::SymbolicTable::Kind & kind) {
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

bool isNumber(const std::string & word, const bool & decimcal = false) {
  bool hasDecimal = false;
  for (unsigned int i = 0; i < word.size(); i++) {
    char value = word.at(i);
    if (decimcal && value == '.') {
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
int getNumber(const std::string & numberString) {
  int value = 0;
  unsigned int start = numberString.size();
  for (unsigned int i = 0; i < numberString.size(); i++) {
    value += pow(10, start - 1 - i) * getDigit(numberString.at(i)); 
  }
  return value;
}

void compiler::JackTokenizer::debug(const unsigned int & proviedLinePosition) {
  std::string line = lines.at(position);
  std::cout << line << std::endl;
  std::string positionString;
  for (unsigned int i = 0; i <= proviedLinePosition; i++) {
    if (i != proviedLinePosition) {
      positionString += "_";
    } else {
      positionString += "^";
    }
  }
  std::cout << positionString << std::endl;
}

void compiler::JackTokenizer::debug() {
  debug(linePosition);
}

unsigned int compiler::JackTokenizer::getLineNumber() {
  return position;
}

unsigned int compiler::JackTokenizer::getLinePosition() {
  return linePosition;
}

compiler::JackTokenizer::JackTokenizer(const std::vector<std::string> & newLines): lines(newLines) {}

bool compiler::JackTokenizer::hasMoreTokens() {
  return position < lines.size() && linePosition < lines.at(position).size();
}

void compiler::JackTokenizer::nextPosition(unsigned int tempLinePosition) {
  if (tempLinePosition >= lines[position].size()) { // Reached the end of a line, go to next line
    tempLinePosition = 0;
    position++;
  }

  bool isMultilineComment = false;

  // While there are more lines to look at
  while (position < lines.size()) {
    unsigned int lineSize = lines[position].size();

    if (
      lineSize == 0 || // if the current line is empty then go next line
      (tempLinePosition < lineSize - 1 && lines[position].substr(tempLinePosition, 2) == "//") // there is a comment for the rest of the line (skip to next line)
    ) {
      tempLinePosition = 0;
      position++;
      continue;
    } else if (tempLinePosition < lineSize - 1 && lines[position].substr(tempLinePosition, 2) == "/*") { // is a multiline comment
      isMultilineComment = true;
      tempLinePosition += 2;
    } else if (tempLinePosition < lineSize - 1 && lines[position].substr(tempLinePosition, 2) == "*/") { // the end of the a multiline comment
      if (!isMultilineComment) {
        throw std::invalid_argument("No start to the multiline comment!");
      }
      isMultilineComment = false;
      tempLinePosition += 2;
    } else if (
      isMultilineComment || // if it is a multline comment just keep going till you reach the end
      (tempLinePosition < lineSize && (lines[position].at(tempLinePosition) == ' ')) // reach end of spaces
    ) {
      tempLinePosition++;
    } else { // if the character is not a space ' ' then break out of while loop
      break;
    }
    // if we reached the end of line go to the next line
    if (tempLinePosition >= lines[position].size()) {
      tempLinePosition = 0;
      position++;
    }
  }

  if (tempLinePosition >= lines[position].size()) { // Reached the end of a line, go to next line
    tempLinePosition = 0;
    position++;
  }
  linePosition = tempLinePosition;
}

std::string compiler::JackTokenizer::getToken() {
  std::string currentLine = lines.at(position);
  char value = currentLine.at(linePosition);

  if (isSymbol(value)) {
    tempLinePosition = linePosition + 1;
    return std::string(1, value);
  } else {
    // get the word (the string until a symbol)
    bool isString = false;
    unsigned int pos = linePosition;
    if (currentLine.at(linePosition) == '"') {
      isString = true;
      pos++;
    }
    
    std::string word = "";
    while (pos < currentLine.size() && (
        (!isString && currentLine.at(pos) != ' ' && !isSymbol(currentLine.at(pos))) ||
        (isString && currentLine.at(pos) != '"')
      )
    ) {
      word.push_back(currentLine.at(pos++));
    }

    if (isString) {
      // Account for " in position
      pos++;
    }

    tempLinePosition = pos;
    if (isString) {
      return "\"" + word + "\"";
    } else {
      return word;
    }
  }
}

void compiler::JackTokenizer::advance() {
  nextPosition(tempLinePosition);
}

void compiler::JackTokenizer::peek(const unsigned int & numberOfTokens) {
  prePeek.position = position;
  prePeek.linePosition = linePosition;
  prePeek.tempLinePosition = tempLinePosition;
  for (unsigned int i = 0; i < numberOfTokens; i++) {
    advance();
  }
}

void compiler::JackTokenizer::resetPeek() {
  position = prePeek.position;
  linePosition = prePeek.linePosition;
  tempLinePosition = prePeek.tempLinePosition;
}

compiler::TokenType compiler::JackTokenizer::tokenType() {
  std::string token = getToken();
  // Check if it is a symbol (single char)
  if (token.size() == 1 && isSymbol(token.at(0))) {
    return SYMBOL;
  } else { // check for length
    if (isKeyWord(token)) { // Check if it is a keyword
      return KEYWORD;
    } else if (token.at(0) == '"' && token.at(token.size() - 1) == '"') { // Chec if it is a string constant
      return STRING_CONST;
    } else if (isNumber(token)) { // Check if it is a integer constant
      return INT_CONST;
    } else {
      return IDENTIFIER;
    }
  }

  return INVALID_TOKEN;
}

compiler::Keyword compiler::JackTokenizer::keyWord() {
  if (tokenType() != KEYWORD) {
    throw std::invalid_argument("Invalid call for keyWord when token is not a keyword!");
  }
  return getKeyWord(getToken());
}

char compiler::JackTokenizer::symbol() {
  if (tokenType() != SYMBOL) {
    throw std::invalid_argument("Invalid call for symbol when token is not a symbol!");
  }
  return getToken().at(0);
}

std::string compiler::JackTokenizer::identifier() {
  if (tokenType() != IDENTIFIER) {
    throw std::invalid_argument("Invalid call for identifier when token is not a identifier!");
  }
  return getToken();
}

int compiler::JackTokenizer::intVal() {
  if (tokenType() != INT_CONST) {
    throw std::invalid_argument("Invalid call for intVal when token is not a integer constant!");
  }
  return getNumber(getToken());
}

std::string compiler::JackTokenizer::stringVal() {
  if (tokenType() != STRING_CONST) {
    throw std::invalid_argument("Invalid call for stringVal when token is not a string constant!");
  }
  std::string token = getToken();
  return token.substr(1, token.size() - 2);
}

// CompilationEngine

void compiler::CompilationEngine::debug() {
  if (tokenizer.hasMoreTokens()) {
    tokenizer.debug();
  }
  table.print();
}

void compiler::CompilationEngine::requireSymbol(const char & symbol) {
  if (tokenizer.tokenType() == SYMBOL) {
    if (tokenizer.symbol() == symbol) {
      std::string value = std::string(1, symbol);
      if (symbol == '<') {
        std::cout << "TRUE" << std::endl;
        value = "&lt;";
      } else if (symbol == '>') {
        value = "&gt;";
      } else if (symbol == '"') {
        value = "&quot;";
      } else if (symbol == '&') {
        value = "&amp;";
      }
      if (options.onlyTokenize) {
        output.push_back("<symbol>" + value + "</symbol>");
      }
      tokenizer.advance();
    } else {
      throw new std::invalid_argument("Invalid symbol. Wanted '" + std::string(1, tokenizer.symbol()) + "' got '" + std::string(1, symbol) + "'");
    }
  } else {
    throw new std::invalid_argument("Invalid type.");
  }
}

void compiler::CompilationEngine::requireKeyword(const Keyword & keyword) {
  if (tokenizer.tokenType() != KEYWORD) {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    debug();
    throw std::invalid_argument("Invalid keyword. Wanted keyword but recieved other type of " + std::to_string(tokenizer.tokenType()));
  }
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() != keyword) {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    debug();
    throw std::invalid_argument("Invalid keyword. Wanted: " + getKeywordString(keyword) + ". Recieved: " + getKeywordString(tokenizer.keyWord()) + ".");
  }
  if (options.onlyTokenize) {
    output.push_back("<keyword>" + getKeywordString(keyword) + "</keyword>");
  }
  tokenizer.advance();
}

std::string compiler::CompilationEngine::requireIdentifier(const std::string & errorMessage) {
  if (tokenizer.tokenType() == IDENTIFIER) {
    std::string identifier;
    identifier = tokenizer.identifier();
    if (options.onlyTokenize) {
      output.push_back("<identifier>" + tokenizer.identifier() + "</identifier>");
    }
    tokenizer.advance();
    return identifier;
  } else {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    debug();
    throw std::invalid_argument(errorMessage);
  }
}

void compiler::CompilationEngine::compileString() {
  if (tokenizer.tokenType() != STRING_CONST) {
    throw std::invalid_argument("Can not compile invalid string type.");
  }
  std::string value = tokenizer.stringVal();
  writer.writePush(VMWriter::CONST, value.length());
  writer.writeCall("String.new", 1);
  for (auto c : value) {
    writer.writePush(VMWriter::CONST, toCharValue(c));
    writer.writeCall("String.appendChar", 2);
  }
  tokenizer.advance();
}

void compiler::CompilationEngine::compileKeyword() {
  if (tokenizer.tokenType() != KEYWORD) {
    throw std::invalid_argument("Can not compile an invalid keyword type.");
  }
  Keyword keyword = tokenizer.keyWord();
  if (keyword == THIS) {
    writer.writePush(VMWriter::POINTER, 0);
  } else {
    writer.writePush(VMWriter::CONST, 0);
    // TODO: the rest of the keywords
    if (keyword == TRUE) {
      writer.writeArithmetic(VMWriter::NOT);
    }
  }
  tokenizer.advance();
}

void compiler::CompilationEngine::writePush(const std::string & name) {
  SymbolicTable::Symbol varSymbol = table.getSymbol(name);
  writer.writePush(convertKindToSegment(varSymbol.kind), varSymbol.numberOfType);
}

void compiler::CompilationEngine::writePop(const std::string & name) {
  SymbolicTable::Symbol varSymbol = table.getSymbol(name);
  writer.writePop(convertKindToSegment(varSymbol.kind), varSymbol.numberOfType);
}

void compiler::CompilationEngine::writeTag(const std::string & tag) {
  if (options.onlyTokenize) {
    output.push_back(tag);
  }
}

compiler::CompilationEngine::CompilationEngine(const std::vector<std::string> & inputLines, std::vector<std::string> & outputLines, CompilationEngineOptions options): lines(inputLines), output(outputLines), writer(output), tokenizer(lines) {
  this->options = options;
  // TODO: advance til you reach a class or the end of the tokens
  // TODO: Throw error if the next token is not a class
  // std::cout << std::to_string(tokenizer.tokenType()) << std::endl;
  tokenizer.advance();
  // if (tokenizer.tokenType() != KEYWORD || (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() != CLASS)) {
  //   tokenizer.advance();
  // }
  // tokenizer.debug();
  // TODO: Throw error if the next token is not a class
  // std::cout << (tokenizer.hasMoreTokens() && tokenizer.tokenType() != KEYWORD || (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() != CLASS)) << std::endl;
  // while (tokenizer.hasMoreTokens() && tokenizer.tokenType() != KEYWORD || (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() != CLASS)) {
  //   tokenizer.tokenType();
  //   tokenizer.debug();
  //   tokenizer.advance();
  // }
  // std::cout << "Done" << std::endl;
  // if (!tokenizer.hasMoreTokens()) {
  //   throw std::invalid_argument("No Class found!");
  // }
  compileClass();
}

// 'let' varName ('[' expression ']')? '=' expression ';'
void compiler::CompilationEngine::compileLet() {
  // Checks that is the keyword "let" (this should be impossible since anything calling this method would check for a let)
  // let
  writeTag("<letStatement>");
  requireKeyword(LET);
  // varName
  std::string varName = requireIdentifier("Invalid variable name in let statement.");
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '[') { // var[expression] // TODO: fix for arrays = arrays
    requireSymbol('[');
    compileExpression();
    requireSymbol(']');

    writePush(varName);
    writer.writeArithmetic(VMWriter::ADD);
    // writer.writePop(VMWriter::TEMP, 0); // TEMP[0] = *varName + expression = varName[expression]

    // =
    requireSymbol('=');
    // expression
    compileExpression();

    // Having a temp variable handles let a[i] = b[j]
    writer.writePop(VMWriter::TEMP, 0);
    writer.writePop(VMWriter::POINTER, 1);
    writer.writePush(VMWriter::TEMP, 0);
    writer.writePop(VMWriter::THAT, 0);

    // ;
    requireSymbol(';');
    writeTag("</letStatement>");
  } else {
    // =
    requireSymbol('=');
    // expression
    compileExpression();
    // ;
    requireSymbol(';');
    writeTag("</letStatement>");
    writePop(varName); 
  }
}

// expression = term (op term)*
void compiler::CompilationEngine::compileExpression() {
  writeTag("<expression>");
  compileTerm();
  while (tokenizer.hasMoreTokens() && tokenizer.tokenType() == SYMBOL && isOp(tokenizer.symbol())) {
    char operatorValue = tokenizer.symbol();
    requireSymbol(tokenizer.symbol());
    compileTerm();
    // Post fix for stack
    debug();
    if (!options.onlyTokenize) {
      if (operatorValue == '/') {
        writer.writeCall("Math.divide", 2);
      } else if (operatorValue == '*') {
        writer.writeCall("Math.multiply", 2);
      } else {
        writer.writeArithmetic(getArithmeticType(operatorValue));
      }
    }
  }
  writeTag("</expression>");
}

unsigned int compiler::CompilationEngine::compileExpressionList() {
  writeTag("<expressionList>");
  // TODO: check is a term
  unsigned int count = 0;
  while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')')) {
    compileExpression();
    count++;
    if (tokenizer.hasMoreTokens() && tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
      requireSymbol(',');
    }
  }
  writeTag("</expressionList>");
  return count;
}

// term = integerConstant | stringConstant | keywordConstant | varName | varName'['expression']' | subroutineCall | '('expression')' | unaryOp term
void compiler::CompilationEngine::compileTerm() {
  writeTag("<term>");

  TokenType initialType = tokenizer.tokenType();

  if (initialType == INT_CONST) { // integerConstant
    unsigned int value = tokenizer.intVal();
    writeTag("<integerConstant>" + std::to_string(tokenizer.intVal()) + "</integerConstant>");
    tokenizer.advance();
    writer.writePush(VMWriter::CONST, value);
  } else if (initialType == STRING_CONST) { // stringConstant
    writeTag("<stringConstant>" + tokenizer.stringVal() + "</stringConstant>");
    compileString();
  } else if (initialType == KEYWORD) { // keywordConstant
    compileKeyword();
  } else if (initialType == IDENTIFIER) { // varName | varName'['expression']' | subroutineCall
    // TODO: peek ahead -----------------------------------------------------------
    // TODO:
    // peak ahead to see if subroutineCall or varName'['expression']', default is varName
    // reset peak to go back to current token
    std::cout << "PRE PEEK" << std::endl;
    debug();
    tokenizer.peek();
    // TODO: find a better way of peeking
    if (tokenizer.tokenType() == SYMBOL && (tokenizer.symbol() == '.' || tokenizer.symbol() == '(')) {
      tokenizer.resetPeek();
      debug();
      compileSubroutineCall();
    } else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '[') { // varName'['expression']'
      // TODO: array handling
      tokenizer.resetPeek();
      std::string varName = requireIdentifier(); // varName
      // TODO: array call
      requireSymbol('[');
      compileExpression();
      requireSymbol(']');

      // push varName[expression] onto stack
      writePush(varName);
      writer.writeArithmetic(VMWriter::ADD);
      writer.writePop(VMWriter::POINTER, 1);
      writer.writePush(VMWriter::THAT, 0);

    } else { // varName
      tokenizer.resetPeek();
      std::string varName = requireIdentifier();
      // SymbolicTable::Symbol symbol = table.getSymbol(varName);
      // TODO: write push on stack with variable type and name (if Symbol )
      // writer.writePush();
      writePush(varName);
    }
    // std::cout << "POST PEEK" << std::endl;
    // debug();
    // std::cout << "RESET PEEK" << std::endl;
    // tokenizer.resetPeek();
    // debug();
    // compileSubroutineCall();
    // TODO: original CLEAN UP
    // std::string varName = requireIdentifier();
    // if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '.') { // subroutineCall
    //   compileSubroutineCall(); // TODO: fix
    //   // requireSymbol('.');
    //   // std::string functionName = requireIdentifier("Invalid subroutine name."); // subroutine name
    //   // requireSymbol('(');
    //   // unsigned int numArgs = compileExpressionList();
    //   // requireSymbol(')');
    //   // writer.writeCall(functionName, numArgs);
    // } else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '[') { // varName'['expression']'
    //   // TODO: array call
    //   requireSymbol('[');
    //   compileExpression();
    //   requireSymbol(']');
    // } else { // varName
    //   // SymbolicTable::Symbol symbol = table.getSymbol(varName);
    //   // TODO: write push on stack with variable type and name (if Symbol )
    //   // writer.writePush();
    //   writePush(varName);
    // }
  } else if (initialType == SYMBOL) { // '('expression')' | unaryOp term
    char symbol = tokenizer.symbol();
    if (symbol == '-' || symbol == '~') { // unaryOp term
      requireSymbol(symbol);
      compileTerm();
      // Post fix
      if (!options.onlyTokenize) {
        if (symbol == '-') {
          writer.writeArithmetic(compiler::VMWriter::NEG);
        } else { // ~
          writer.writeArithmetic(compiler::VMWriter::NOT);
        }
      }
    } else if (symbol == '(') { // '('expression')'
      debug();
      requireSymbol('(');
      compileExpression();
      requireSymbol(')');
      debug();
    } else {
      throw std::invalid_argument("Invalid symbol in term: " + std::string(1, symbol));
    }
  } else {
    throw std::invalid_argument("Invalid term type.");
  }

  writeTag("</term>");
}

// 'if''('expression')''{'statements'}'('else''{'statements'}')?
void compiler::CompilationEngine::compileIf() {
  std::string labelFalse = writer.generateLabel();
  std::string labelEnd = writer.generateLabel();
  writeTag("<ifStatement>");
  requireKeyword(IF);
  requireSymbol('(');
  compileExpression();
  writer.writeArithmetic(VMWriter::NOT);
  writer.writeIf(labelFalse);
  requireSymbol(')');
  requireSymbol('{');
  compileStatements();
  requireSymbol('}');
  writer.writeGoto(labelEnd);
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == ELSE) { // ('else''{'statements'}')?
    requireKeyword(ELSE);
    requireSymbol('{');
    writer.writeLabel(labelFalse);
    compileStatements();
    requireSymbol('}');
  } else {
    writer.writeLabel(labelFalse); // If there is no else then the if-goto efficiently goes to labelEnd
  }
  writer.writeLabel(labelEnd);
  writeTag("</ifStatement>");
}

// 'while''('expression')''{'statements'}'
void compiler::CompilationEngine::compileWhile() {
  std::string labelLoop = writer.generateLabel();
  std::string labelEnd = writer.generateLabel();
  writeTag("<whileStatement>");
  requireKeyword(WHILE);
  requireSymbol('(');
  writer.writeLabel(labelLoop);
  compileExpression();
  writer.writeArithmetic(VMWriter::NOT);
  writer.writeIf(labelEnd);
  requireSymbol(')');
  requireSymbol('{');
  compileStatements();
  writer.writeGoto(labelLoop);
  requireSymbol('}');
  writeTag("</whileStatement>");
  writer.writeLabel(labelEnd);
}

// subroutineCall = subroutineName '('expressionList')' | (className | varName)'.'subroutineName'('expressionlist')'
// WARNING: compileSubroutineCall will ALWAYS put the return value on the stack (meaning it does not handle void functions). The caller of compileSubroutineCall must handle void function calls on their own! 
void compiler::CompilationEngine::compileSubroutineCall() {
  std::string functionName;
  unsigned int argumentCount = 0;
  std::string firstValue = requireIdentifier(); // subroutineName | className | varName
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '.') {
    requireSymbol('.');
    std::string subroutineName = requireIdentifier(); // subroutineName
    std::cout << "SUB " << subroutineName << std::endl;
    requireSymbol('(');
    argumentCount = compileExpressionList();
    requireSymbol(')');
    bool isVariable = table.exists(firstValue); // firstValue = className | varName
    if (isVariable) { // firstValue = varName
      // BTW varName should be a instance not a primitive (check that symbol is not a boolean, char, int)
      // TODO: type check that varName is a class
      
      // Push the address of the class instance
      writePush(firstValue);
      argumentCount++;
      // functionName = className.subroutine
      functionName = table.typeOf(firstValue) + "." + subroutineName;
    } else { // firstValue = className
      // the subroutine should be a FUNCTION (static method in java)
      // TODO: type check that subroutine is a FUNCTION
      functionName = firstValue + "." + subroutineName;
    }
  } else { // firstValue = subroutineName (has to be a method)
    functionName = className + "." + firstValue;
    requireSymbol('(');
    argumentCount = compileExpressionList();
    requireSymbol(')');
    // push THIS to arguments
    writer.writePush(VMWriter::POINTER, 0);
    argumentCount++;
  }
  writer.writeCall(functionName, argumentCount);
}

// 'do' subroutineCall';'
void compiler::CompilationEngine::compileDo() {
  // Should never happen
  writeTag("<doStatement>");
  requireKeyword(DO);
  // return value is pushed onto stack
  compileSubroutineCall();
  // remove the added value from the stack (handles the void function default return value of 0)
  writer.writePop(VMWriter::TEMP, 0);
  requireSymbol(';');
  writeTag("</doStatement>");
}

// 'return' expression?';'
void compiler::CompilationEngine::compileReturn() {
  // Should never happen
  writeTag("<returnStatement>");
  requireKeyword(RETURN);
  if (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ';')) {
    compileExpression();
  } else {
    // by default return 0
    writer.writePush(VMWriter::CONST, 0);
  }
  requireSymbol(';');
  writeTag("</returnStatement>");
  writer.writeReturn();
}

// statements = (ifStatement | whileStatement | letStatement)*
void compiler::CompilationEngine::compileStatements() {
  writeTag("<statements>");
  while (tokenizer.tokenType() == KEYWORD && (
    tokenizer.keyWord() == IF ||
    tokenizer.keyWord() == WHILE || 
    tokenizer.keyWord() == LET || 
    tokenizer.keyWord() == DO ||
    tokenizer.keyWord() == RETURN
  )) {
    if (tokenizer.keyWord() == IF) {
      compileIf();
    } else if (tokenizer.keyWord() == WHILE) {
      compileWhile();
    } else if (tokenizer.keyWord() == RETURN) { // TODO: ensure that return is at the bottom of the statements
      compileReturn();
    } else if (tokenizer.keyWord() == DO) {
      compileDo();
    } else { // LET
      compileLet();
    }
  }
  writeTag("</statements>");
}

// subroutineCall = subroutineName '(' expressionList ')' | (className | varName)'.'subroutineName'(' expressionList ')'

// class = 'class' className '{' classVarDec* subroutineDec* '}'
void compiler::CompilationEngine::compileClass() {
  writeTag("<class>");
  requireKeyword(CLASS);
  className = requireIdentifier("Invalid class name.");
  requireSymbol('{');
  while (tokenizer.tokenType() == KEYWORD && (tokenizer.keyWord() == STATIC || tokenizer.keyWord() == FIELD)) {
    compileClassVarDec();
  }
  while (tokenizer.tokenType() == KEYWORD && (tokenizer.keyWord() == CONSTRUCTOR || tokenizer.keyWord() == FUNCTION || tokenizer.keyWord() == METHOD)) {
    compileSubroutineDec();
  }
  requireSymbol('}');
  writeTag("</class>");
}

// // ('static' | 'field') type varName (', varName')*';'
void compiler::CompilationEngine::compileClassVarDec() {
  SymbolicTable::Kind variableStorageType;
  std::string type;
  writeTag("<classVarDec>");
  if (tokenizer.tokenType() != KEYWORD) {
    throw std::invalid_argument("Invalid class variable declare");
  }

  if (tokenizer.keyWord() == STATIC) {
    requireKeyword(Keyword::STATIC);
    variableStorageType = SymbolicTable::Kind::STATIC;
  } else if (tokenizer.keyWord() == Keyword::FIELD) {
    requireKeyword(Keyword::FIELD);
    variableStorageType = SymbolicTable::Kind::FIELD;
  } else {
    throw std::invalid_argument("Invalid class variable declaration.");
  }
  
  if (tokenizer.tokenType() == IDENTIFIER) {
    writeTag("<identifier>" + tokenizer.identifier() + "</identifier>");
    type = tokenizer.identifier();
    tokenizer.advance();
  } else if (tokenizer.tokenType() == KEYWORD) {
    if (tokenizer.keyWord() == INT) {
      requireKeyword(INT);
      type = "int";
    } else if (tokenizer.keyWord() == CHAR) {
      requireKeyword(CHAR);
      type = "char";
    } else if (tokenizer.keyWord() == BOOLEAN) {
      requireKeyword(BOOLEAN);
      type = "boolean";
    } else {
      throw std::invalid_argument("Invalid class var dec type.");
    }
  } else  {
    throw std::invalid_argument("Invalid class var dec type.");
  }
  
  // First field/static defintion
  table.define(tokenizer.identifier(), type, variableStorageType);
  requireIdentifier();

  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
    requireSymbol(',');
    // Other field/static defintions
    table.define(tokenizer.identifier(), type, variableStorageType);
    requireIdentifier();
  }
  requireSymbol(';');
  writeTag("</classVarDec>");
}

void compiler::CompilationEngine::compileVariable(const std::string & errorVariableName) {
  // TODO: type
  std::string type;
  writeTag("<" + errorVariableName + ">");
  if (tokenizer.tokenType() == IDENTIFIER) {
    writeTag("<identifier>" + tokenizer.identifier() + "</identifier>");
    type = tokenizer.identifier();
    tokenizer.advance();
  } else if (tokenizer.tokenType() == KEYWORD) {
    if (tokenizer.keyWord() == INT) {
      requireKeyword(INT);
      type = "int";
    } else if (tokenizer.keyWord() == CHAR) {
      requireKeyword(CHAR);
      type = "char";
    } else if (tokenizer.keyWord() == BOOLEAN) {
      requireKeyword(BOOLEAN);
      type = "boolean";
    } else {
      throw std::invalid_argument("Invalid " + errorVariableName + " type.");
    }
  } else  {
    throw std::invalid_argument("Invalid " + errorVariableName + " type.");
  }
  std::string varName = tokenizer.identifier();
  tokenizer.advance();
  table.define(varName, type, compiler::SymbolicTable::ARGUMENT);
  writeTag("<identifier>" + varName + "</identifier>");
  writeTag("</" + errorVariableName + ">");
}

// ('constructor' | 'function' | 'method') ('void' | type) subroutineName '('parameterList')' subroutineBody
void compiler::CompilationEngine::compileSubroutineDec() {
  table.startSubroutine();
  writeTag("<subroutineDec>");
  TokenType type = tokenizer.tokenType();
  if (type != KEYWORD) {
    throw std::invalid_argument("Invalid subroutine type.");
  }
  Keyword keyword = tokenizer.keyWord();
  if (keyword == CONSTRUCTOR) {
    requireKeyword(CONSTRUCTOR);
  } else if (keyword == FUNCTION) {
    requireKeyword(FUNCTION);
  } else if (keyword == METHOD) {
    requireKeyword(METHOD);
    // Define "this" to the first argument
    // This needs to be defined FIRST before the other arguments, which are defined lower in compileVariable
    table.define("this", className, SymbolicTable::Kind::ARGUMENT);
  } else {
    throw std::invalid_argument("Invalid subroutine type.");
  }

  // Redefine type
  type = tokenizer.tokenType();
  if (type == KEYWORD) {
    Keyword keyword = tokenizer.keyWord();
    if (keyword == VOID) {
      requireKeyword(VOID);
    } else if (keyword == BOOLEAN) {
      requireKeyword(BOOLEAN);
    } else if (keyword == INT) {
      requireKeyword(INT);
    } else if (keyword == CHAR) {
      requireKeyword(CHAR);
    } else {
      throw std::invalid_argument("Invalid keyword for subroutine type.");
    }
  } else if (type == IDENTIFIER) {
    requireIdentifier();
  } else {
    throw std::invalid_argument("Invalid subroutine return type.");
  }

  // subroutineName
  std::string functionName = requireIdentifier();

  requireSymbol('(');
  // parameterList = ( (type varName) (',' type varName)* )?
  writeTag("<parameterList>");
  if (tokenizer.tokenType() == IDENTIFIER || tokenizer.tokenType() == KEYWORD) { // ?
    compileVariable(); // (type varName)
    while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') { // (',' type varName)*
      requireSymbol(',');
      compileVariable();
    }
  }

  writeTag("</parameterList>");
  requireSymbol(')');

  // Subroutine body
  // '{' varDec* statements '}'
  writeTag("<subroutineBody>");
  requireSymbol('{');
  while (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == VAR) {
    compileVarDec();
  }

  // VM write function
  unsigned int numberOfLocals = table.varCount(SymbolicTable::Kind::LOCAL);
  writer.writeFunction(className + "." + functionName, numberOfLocals);
  if (keyword == CONSTRUCTOR) {
    // Allocate instance variables in heap memory (call to the OS for allocation)
    writer.writePush(VMWriter::CONST, table.varCount(SymbolicTable::Kind::FIELD));
    writer.writeCall("Memory.alloc", 1);
    writer.writePop(VMWriter::POINTER, 0); // anchors this at the base address (THIS = allocated space from Memory.alloc)
  } else if (keyword == METHOD) {
    // The first parameter in a method is ALWAYS THIS, so that the method can use field variables (like python's self)
    writer.writePush(VMWriter::ARG, 0);
    writer.writePop(VMWriter::POINTER, 0);
  }

  compileStatements();
  requireSymbol('}');
  writeTag("</subroutineBody>");

  writeTag("</subroutineDec>");

}

// varDec = 'var' type varName (',' varName)*';'
void compiler::CompilationEngine::compileVarDec() {
  writeTag("<varDec>");
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == VAR) {
    std::string type;

    requireKeyword(VAR);
    // Vars
    if (tokenizer.tokenType() == IDENTIFIER) {
      type = tokenizer.identifier();
      requireIdentifier();
    } else if (tokenizer.tokenType() == KEYWORD) {
      if (tokenizer.keyWord() == INT) {
        requireKeyword(INT);
        type = "int";
      } else if (tokenizer.keyWord() == CHAR) {
        requireKeyword(CHAR);
        type = "char";
      } else if (tokenizer.keyWord() == BOOLEAN) {
        requireKeyword(BOOLEAN);
        type = "boolean";
      } else {
        throw std::invalid_argument("Invalid var type.");
      }
    } else  {
      throw std::invalid_argument("Invalid var type.");
    }
    // First variable defintion
    table.define(tokenizer.identifier(), type, compiler::SymbolicTable::LOCAL);
    requireIdentifier("Invalid variable name for variable declartion");
    while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
      requireSymbol(',');
      // Other variables
      table.define(tokenizer.identifier(), type, compiler::SymbolicTable::LOCAL);
      requireIdentifier("Invalid variable name for variable declartion");
    }
  }
  requireSymbol(';');
  writeTag("</varDec>");
}

// '{' varDec* statements '}'
void compiler::CompilationEngine::compileSubroutineBody() {
  
}

// Writter

compiler::Writer::PATH_TYPE compiler::Writer::getType(const std::string & path) {
  // boost exists works for both files and directories
  if (!boost::filesystem::exists(path)) {
    return PATH_TYPE::NOTFOUND;
  } else if (boost::filesystem::is_directory(path)) {
    return PATH_TYPE::DIRECTORY;
  } else {
    return PATH_TYPE::FILE;
  }
}

boost::filesystem::path compiler::Writer::getPathBaseName(const std::string & path) {
  PATH_TYPE type = getType(path);
  if (type == PATH_TYPE::FILE) {
    boost::filesystem::path p (path);
    // Gets the filename with extension (removes the path to the file)
    return p.filename().stem();
    // Removes the filename extenstion
    // return basename.substr(0, basename.find_last_of('.'));
  } else if (type == PATH_TYPE::DIRECTORY) {
    boost::filesystem::path p (path);
    if (p.filename().string() == ".") {
      if (!p.has_parent_path()) {
        throw std::invalid_argument("Can not create a filename for directory with not parent.");
      }
      return p.parent_path().filename();
    }
    // Gets the directory name
    return p.filename();
  } else {
    throw std::invalid_argument("File or directory by path \"" + path + "\" was not found.");
  }
}

std::string compiler::Writer::getOutputFileLocation(const std::string & path) {
  boost::filesystem::path filename = getPathBaseName(path);
  boost::filesystem::path fullPath (path);
  // Joins the directory of the file with the new filename
  return (fullPath.parent_path() / filename).string() + ".xml";
}

// SymbolicTable

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

// VMWriter
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
