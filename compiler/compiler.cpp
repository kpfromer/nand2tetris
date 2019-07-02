#include <string>
#include <iostream>
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
    // TODO: handle multile line comments

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

// TODO: do i need these checks? (they slow things down)
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
  tokenizer.debug();
}

void compiler::CompilationEngine::requireSymbol(const char & symbol) {
  if (tokenizer.tokenType() == SYMBOL) {
    if (tokenizer.symbol() == symbol) {
      // TODO: escape characters
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
      output.push_back("<symbol>" + value + "</symbol>");
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
  output.push_back("<keyword>" + getKeywordString(keyword) + "</keyword>");
  tokenizer.advance();
}

void compiler::CompilationEngine::requireIdentifier(const std::string & errorMessage) {
  if (tokenizer.tokenType() == IDENTIFIER) {
    output.push_back("<identifier>" + tokenizer.identifier() + "</identifier>");
    tokenizer.advance();
  } else {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    debug();
    throw std::invalid_argument(errorMessage);
  }
}

compiler::CompilationEngine::CompilationEngine(const std::vector<std::string> & inputLines, std::vector<std::string> & outputLines): lines(inputLines), output(outputLines), tokenizer(lines) {
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

void compiler::CompilationEngine::compileLet() {
  // Checks that is the keyword "let" (this should be impossible since anything calling this method would check for a let)
  // let
  output.push_back("<letStatement>");
  requireKeyword(LET);
  // varName
  requireIdentifier("Invalid variable name in let statement.");
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '[') { // var[1]
    requireSymbol('[');
    compileExpression();
    requireSymbol(']');
  }
  // =
  requireSymbol('=');
  // expression
  compileExpression();
  // ;
  requireSymbol(';');
  output.push_back("</letStatement>");
}

// expression = term (op term)*
void compiler::CompilationEngine::compileExpression() {
  output.push_back("<expression>");
  compileTerm();
  while (tokenizer.hasMoreTokens() && tokenizer.tokenType() == SYMBOL && isOp(tokenizer.symbol())) {
    requireSymbol(tokenizer.symbol());
    compileTerm();
  }
  output.push_back("</expression>");
}

void compiler::CompilationEngine::compileExpressionList() {
  output.push_back("<expressionList>");
  // TODO: check is a term
  while (!(tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ')')) {
    compileExpression();
    if (tokenizer.hasMoreTokens() && tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
      requireSymbol(',');
    }
  }
  output.push_back("</expressionList>");
}

// term = integerConstant | stringConstant | keywordConstant | varName | varName'['expression']' | subroutineCall | '('expression')' | unaryOp term
void compiler::CompilationEngine::compileTerm() {
  output.push_back("<term>");

  TokenType initialType = tokenizer.tokenType();

  if (initialType == INT_CONST) { // integerConstant
    output.push_back("<integerConstant>" + std::to_string(tokenizer.intVal()) + "</integerConstant>");
    tokenizer.advance();
  } else if (initialType == STRING_CONST) { // stringConstant
    output.push_back("<stringConstant>" + tokenizer.stringVal() + "</stringConstant>");
    tokenizer.advance();
  } else if (initialType == KEYWORD) { // keywordConstant
    requireKeyword(tokenizer.keyWord());
  } else if (initialType == IDENTIFIER) { // varName | varName'['expression']' | subroutineCall
    requireIdentifier();
    if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '.') { // subroutineCall
      requireSymbol('.');
      requireIdentifier("Invalid subroutine name."); // subroutine name
      requireSymbol('(');
      compileExpressionList();
      requireSymbol(')');
    } else if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '[') { // varName'['expression']'
      requireSymbol('[');
      compileExpression();
      requireSymbol(']');
    }
  } else if (initialType == SYMBOL) { // '('expression')' | unaryOp term
    char symbol = tokenizer.symbol();
    if (symbol == '-' || symbol == '~') { // unaryOp term
      requireSymbol(symbol);
      compileTerm();
    } else if (symbol == '(') { // '('expression')'
      requireSymbol('(');
      compileExpression();
      requireSymbol(')');
    } else {
      throw std::invalid_argument("Invalid symbol in term: " + std::string(1, symbol));
    }
  } else {
    throw std::invalid_argument("Invalid term type.");
  }

  output.push_back("</term>");
}

// 'if''('expression')''{'statements'}'('else''{'statements'}')?
void compiler::CompilationEngine::compileIf() {
  output.push_back("<ifStatement>");
  requireKeyword(IF);
  requireSymbol('(');
  compileExpression();
  requireSymbol(')');
  requireSymbol('{');
  compileStatements();
  requireSymbol('}');
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == ELSE) { // ('else''{'statements'}')?
    requireKeyword(ELSE);
    requireSymbol('{');
    compileStatements();
    requireSymbol('}');
  }
  // TODO: else if in future
  output.push_back("</ifStatement>");
}

// 'while''('expression')''{'statements'}'
void compiler::CompilationEngine::compileWhile() {
  output.push_back("<whileStatement>");
  requireKeyword(WHILE);
  requireSymbol('(');
  compileExpression();
  requireSymbol(')');
  requireSymbol('{');
  compileStatements();
  requireSymbol('}');
  output.push_back("</whileStatement>");
}

// subroutineCall = subroutineName '('expressionList')' | (className | varName)'.'subroutineName'('expressionlist')'
// 'do' subroutineCall';'
void compiler::CompilationEngine::compileDo() {
  // Should never happen
  output.push_back("<doStatement>");
  requireKeyword(DO);

  requireIdentifier(); // subroutineName | (className | varName)
  if (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == '.') {
    requireSymbol('.');
    requireIdentifier(); // subroutineName
  }
  // '('expressionList')'
  requireSymbol('(');
  compileExpressionList();
  requireSymbol(')');

  // TODO: subroutine
  requireSymbol(';');
  output.push_back("</doStatement>");
}

// 'return' expression?';'
void compiler::CompilationEngine::compileReturn() {
  // Should never happen
  output.push_back("<returnStatement>");
  requireKeyword(RETURN);
  if (tokenizer.tokenType() != SYMBOL) {
    compileExpression();
  }
  requireSymbol(';');
  output.push_back("</returnStatement>");
}

// statements = (ifStatement | whileStatement | letStatement)*
void compiler::CompilationEngine::compileStatements() {
  output.push_back("<statements>");
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
  output.push_back("</statements>");
}

// subroutineCall = subroutineName '(' expressionList ')' | (className | varName)'.'subroutineName'(' expressionList ')'

// class = 'class' className '{' classVarDec* subroutineDec* '}'
void compiler::CompilationEngine::compileClass() {
  output.push_back("<class>");
  requireKeyword(CLASS);
  requireIdentifier("Invalid class name.");
  requireSymbol('{');
  while (tokenizer.tokenType() == KEYWORD && (tokenizer.keyWord() == STATIC || tokenizer.keyWord() == FIELD)) {
    compileClassVarDec();
  }
  while (tokenizer.tokenType() == KEYWORD && (tokenizer.keyWord() == CONSTRUCTOR || tokenizer.keyWord() == FUNCTION || tokenizer.keyWord() == METHOD)) {
    compileSubroutineDec();
  }
  requireSymbol('}');
  output.push_back("</class>");
}

// // ('static' | 'field') type varName (', varName')*';'
void compiler::CompilationEngine::compileClassVarDec() {
  output.push_back("<classVarDec>");
  if (tokenizer.tokenType() != KEYWORD) {
    throw std::invalid_argument("Invalid class variable declare");
  }

  if (tokenizer.keyWord() == STATIC) {
    requireKeyword(STATIC);
  } else if (tokenizer.keyWord() == FIELD) {
    requireKeyword(FIELD);
  } else {
    throw std::invalid_argument("Invalid class variable declaration.");
  }
  
  if (tokenizer.tokenType() == IDENTIFIER) {
    output.push_back("<identifier>" + tokenizer.identifier() + "</identifier>");
    tokenizer.advance();
  } else if (tokenizer.tokenType() == KEYWORD) {
    if (tokenizer.keyWord() == INT) {
      requireKeyword(INT);
    } else if (tokenizer.keyWord() == CHAR) {
      requireKeyword(CHAR);
    } else if (tokenizer.keyWord() == BOOLEAN) {
      requireKeyword(BOOLEAN);
    } else {
      throw std::invalid_argument("Invalid class var dec type.");
    }
  } else  {
    throw std::invalid_argument("Invalid class var dec type.");
  }
  
  requireIdentifier();

  while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
    requireSymbol(',');
    requireIdentifier();
  }
  requireSymbol(';');
  output.push_back("</classVarDec>");
}

void compiler::CompilationEngine::compileVariable(const std::string & errorVariableName) {
  // TODO: type
  output.push_back("<" + errorVariableName + ">");
  if (tokenizer.tokenType() == IDENTIFIER) {
    output.push_back("<identifier>" + tokenizer.identifier() + "</identifier>");
    tokenizer.advance();
  } else if (tokenizer.tokenType() == KEYWORD) {
    if (tokenizer.keyWord() == INT) {
      requireKeyword(INT);
    } else if (tokenizer.keyWord() == CHAR) {
      requireKeyword(CHAR);
    } else if (tokenizer.keyWord() == BOOLEAN) {
      requireKeyword(BOOLEAN);
    } else {
      throw std::invalid_argument("Invalid " + errorVariableName + " type.");
    }
  } else  {
    throw std::invalid_argument("Invalid " + errorVariableName + " type.");
  }
  std::string varName = tokenizer.identifier();
  tokenizer.advance();
  output.push_back("<identifier>" + varName + "</identifier>");
  output.push_back("</" + errorVariableName + ">");
}

// ('constructor' | 'function' | 'method') ('void' | type) subroutineName '('parameterList')' subroutineBody
void compiler::CompilationEngine::compileSubroutineDec() {
  output.push_back("<subroutineDec>");
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
  } else {
    throw std::invalid_argument("Invalid subroutine type.");
  }

  // Redefine type
  type = tokenizer.tokenType();;
  if (type == KEYWORD && tokenizer.keyWord() == VOID) {
    requireKeyword(VOID);
  } else if (type == IDENTIFIER) {
    requireIdentifier();
  } else {
    throw std::invalid_argument("Invalid subroutine return type.");
  }

  // subroutineName
  requireIdentifier();

  requireSymbol('(');
  // parameterList = ( (type varName) (',' type varName)* )?

  output.push_back("<parameterList>");
  if (tokenizer.tokenType() == IDENTIFIER || tokenizer.tokenType() == KEYWORD) { // ?
    compileVariable(); // (type varName)
    while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') { // (',' type varName)*
      requireSymbol(',');
      compileVariable();
    }
  }
  output.push_back("</parameterList>");
  requireSymbol(')');
  compileSubroutineBody();
  output.push_back("</subroutineDec>");
}

// varDec = 'var' type varName (',' varName)*';'
void compiler::CompilationEngine::compileVarDec() {
  output.push_back("<varDec>");
  if (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == VAR) {
    requireKeyword(VAR);
    // Vars
    if (tokenizer.tokenType() == IDENTIFIER) {
      requireIdentifier();
    } else if (tokenizer.tokenType() == KEYWORD) {
      if (tokenizer.keyWord() == INT) {
        requireKeyword(INT);
      } else if (tokenizer.keyWord() == CHAR) {
        requireKeyword(CHAR);
      } else if (tokenizer.keyWord() == BOOLEAN) {
        requireKeyword(BOOLEAN);
      } else {
        throw std::invalid_argument("Invalid var type.");
      }
    } else  {
      throw std::invalid_argument("Invalid var type.");
    }
    requireIdentifier("Invalid variable name for variable declartion");
    while (tokenizer.tokenType() == SYMBOL && tokenizer.symbol() == ',') {
      requireSymbol(',');
      requireIdentifier("Invalid variable name for variable declartion");
    }
  }
  requireSymbol(';');
  output.push_back("</varDec>");
}

// '{' varDec* statements '}'
void compiler::CompilationEngine::compileSubroutineBody() {
  output.push_back("<subroutineBody>");
  requireSymbol('{');
  while (tokenizer.tokenType() == KEYWORD && tokenizer.keyWord() == VAR) {
    compileVarDec();
  }
  compileStatements();
  requireSymbol('}');
  output.push_back("</subroutineBody>");
}

// Writter

compiler::Writter::PATH_TYPE compiler::Writter::getType(const std::string & path) {
  // boost exists works for both files and directories
  if (!boost::filesystem::exists(path)) {
    return PATH_TYPE::NOTFOUND;
  } else if (boost::filesystem::is_directory(path)) {
    return PATH_TYPE::DIRECTORY;
  } else {
    return PATH_TYPE::FILE;
  }
}

boost::filesystem::path compiler::Writter::getPathBaseName(const std::string & path) {
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

std::string compiler::Writter::getOutputFileLocation(const std::string & path) {
  boost::filesystem::path filename = getPathBaseName(path);
  boost::filesystem::path fullPath (path);
  // Joins the directory of the file with the new filename
  return (fullPath.parent_path() / filename).string() + ".xml";
}