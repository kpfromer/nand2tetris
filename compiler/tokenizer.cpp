#include <string>
#include <iostream>
#include "compiler.h"

void compiler::JackTokenizer::debug(const unsigned int & proviedLinePosition) {
  std::string line = lines.at(position);
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
  return tempLinePosition;
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

  if (detail::isSymbol(value)) {
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
        (!isString && currentLine.at(pos) != ' ' && !detail::isSymbol(currentLine.at(pos))) ||
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

compiler::detail::TokenType compiler::JackTokenizer::tokenType() {
  std::string token = getToken();
  // Check if it is a symbol (single char)
  if (token.size() == 1 && detail::isSymbol(token.at(0))) {
    return detail::SYMBOL;
  } else { // check for length
    if (detail::isKeyWord(token)) { // Check if it is a keyword
      return detail::KEYWORD;
    } else if (token.at(0) == '"' && token.at(token.size() - 1) == '"') { // Chec if it is a string constant
      return detail::STRING_CONST;
    } else if (detail::isNumber(token)) { // Check if it is a integer constant
      return detail::INT_CONST;
    } else {
      return detail::IDENTIFIER;
    }
  }

  return detail::INVALID_TOKEN;
}

compiler::detail::Keyword compiler::JackTokenizer::keyWord() {
  if (tokenType() != detail::KEYWORD) {
    throw std::invalid_argument("Invalid call for keyWord when token is not a keyword!");
  }
  return detail::getKeyWord(getToken());
}

char compiler::JackTokenizer::symbol() {
  if (tokenType() != detail::SYMBOL) {
    throw std::invalid_argument("Invalid call for symbol when token is not a symbol!");
  }
  return getToken().at(0);
}

std::string compiler::JackTokenizer::identifier() {
  if (tokenType() != detail::IDENTIFIER) {
    throw std::invalid_argument("Invalid call for identifier when token is not a identifier!");
  }
  return getToken();
}

int compiler::JackTokenizer::intVal() {
  if (tokenType() != detail::INT_CONST) {
    throw std::invalid_argument("Invalid call for intVal when token is not a integer constant!");
  }
  return detail::getNumber(getToken());
}

std::string compiler::JackTokenizer::stringVal() {
  if (tokenType() != detail::STRING_CONST) {
    throw std::invalid_argument("Invalid call for stringVal when token is not a string constant!");
  }
  std::string token = getToken();
  return token.substr(1, token.size() - 2);
}