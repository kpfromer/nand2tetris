#include <iostream>
#include "compiler.h"


void compiler::CompilationEngine::debug() {
  if (tokenizer.hasMoreTokens()) {
    tokenizer.debug();
  }
  table.print();
}

void compiler::CompilationEngine::error() {
  std::cerr << "Error at line " << std::to_string(tokenizer.getLineNumber()) << " " << std::to_string(tokenizer.getLinePosition()) << std::endl;
}

void compiler::CompilationEngine::requireSymbol(const char & symbol) {
  if (tokenizer.tokenType() == detail::SYMBOL) {
    if (tokenizer.symbol() == symbol) {
      std::string value = std::string(1, symbol);
      if (symbol == '<') {
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

void compiler::CompilationEngine::requireKeyword(const detail::Keyword & keyword) {
  if (tokenizer.tokenType() != detail::KEYWORD) {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    throw std::invalid_argument("Invalid keyword. Wanted keyword but recieved other type of " + std::to_string(tokenizer.tokenType()));
  }
  if (tokenizer.tokenType() == detail::KEYWORD && tokenizer.keyWord() != keyword) {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    throw std::invalid_argument("Invalid keyword. Wanted: " + getKeywordString(keyword) + ". Recieved: " + getKeywordString(tokenizer.keyWord()) + ".");
  }
  if (options.onlyTokenize) {
    output.push_back("<keyword>" + getKeywordString(keyword) + "</keyword>");
  }
  tokenizer.advance();
}

std::string compiler::CompilationEngine::requireIdentifier(const std::string & errorMessage) {
  if (tokenizer.tokenType() == detail::IDENTIFIER) {
    std::string identifier;
    identifier = tokenizer.identifier();
    if (options.onlyTokenize) {
      output.push_back("<identifier>" + tokenizer.identifier() + "</identifier>");
    }
    tokenizer.advance();
    return identifier;
  } else {
    std::cerr << "Line: " << std::to_string(tokenizer.getLineNumber()) << std::endl;
    throw std::invalid_argument(errorMessage);
  }
}

void compiler::CompilationEngine::compileString() {
  if (tokenizer.tokenType() != detail::STRING_CONST) {
    throw std::invalid_argument("Can not compile invalid string type.");
  }
  std::string value = tokenizer.stringVal();
  writer.writePush(VMWriter::CONST, value.length());
  writer.writeCall("String.new", 1);
  for (auto c : value) {
    writer.writePush(VMWriter::CONST, detail::toCharValue(c));
    writer.writeCall("String.appendChar", 2);
  }
  tokenizer.advance();
}

void compiler::CompilationEngine::compileKeyword() {
  if (tokenizer.tokenType() != detail::KEYWORD) {
    throw std::invalid_argument("Can not compile an invalid keyword type.");
  }
  detail::Keyword keyword = tokenizer.keyWord();
  if (keyword == detail::THIS) {
    writer.writePush(VMWriter::POINTER, 0);
  } else {
    writer.writePush(VMWriter::CONST, 0);
    // TODO: the rest of the keywords
    if (keyword == detail::TRUE) {
      writer.writeArithmetic(VMWriter::NOT);
    }
  }
  tokenizer.advance();
}

void compiler::CompilationEngine::writePush(const std::string & name) {
  SymbolicTable::Symbol varSymbol = table.getSymbol(name);
  writer.writePush(detail::convertKindToSegment(varSymbol.kind), varSymbol.numberOfType);
}

void compiler::CompilationEngine::writePop(const std::string & name) {
  SymbolicTable::Symbol varSymbol = table.getSymbol(name);
  writer.writePop(detail::convertKindToSegment(varSymbol.kind), varSymbol.numberOfType);
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

  // try {
  compileClass();
  // } catch (const std::invalid_argument & error) {
  //   std::cerr << "Failed to compile file." << std::endl;
  //   std::cerr << "Reason: " << error.what() << std::endl;
  //   debug();
  // }
}

// 'let' varName ('[' expression ']')? '=' expression ';'
void compiler::CompilationEngine::compileLet() {
  // Checks that is the keyword "let" (this should be impossible since anything calling this method would check for a let)
  // let
  writeTag("<letStatement>");
  requireKeyword(detail::LET);
  // varName
  std::string varName = requireIdentifier("Invalid variable name in let statement.");
  if (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == '[') { // var[expression] // TODO: fix for arrays = arrays
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
  while (tokenizer.hasMoreTokens() && tokenizer.tokenType() == detail::SYMBOL && detail::isOp(tokenizer.symbol())) {
    char operatorValue = tokenizer.symbol();
    requireSymbol(tokenizer.symbol());
    compileTerm();
    // Post fix for stack
    if (!options.onlyTokenize) {
      if (operatorValue == '/') {
        writer.writeCall("Math.divide", 2);
      } else if (operatorValue == '*') {
        writer.writeCall("Math.multiply", 2);
      } else {
        writer.writeArithmetic(detail::getArithmeticType(operatorValue));
      }
    }
  }
  writeTag("</expression>");
}

unsigned int compiler::CompilationEngine::compileExpressionList() {
  writeTag("<expressionList>");
  // TODO: check is a term
  unsigned int count = 0;
  while (!(tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ')')) {
    compileExpression();
    count++;
    if (tokenizer.hasMoreTokens() && tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ',') {
      requireSymbol(',');
    }
  }
  writeTag("</expressionList>");
  return count;
}

// term = integerConstant | stringConstant | keywordConstant | varName | varName'['expression']' | subroutineCall | '('expression')' | unaryOp term
void compiler::CompilationEngine::compileTerm() {
  writeTag("<term>");

  detail::TokenType initialType = tokenizer.tokenType();

  if (initialType == detail::INT_CONST) { // integerConstant
    unsigned int value = tokenizer.intVal();
    writeTag("<integerConstant>" + std::to_string(tokenizer.intVal()) + "</integerConstant>");
    tokenizer.advance();
    writer.writePush(VMWriter::CONST, value);
  } else if (initialType == detail::STRING_CONST) { // stringConstant
    writeTag("<stringConstant>" + tokenizer.stringVal() + "</stringConstant>");
    compileString();
  } else if (initialType == detail::KEYWORD) { // keywordConstant
    compileKeyword();
  } else if (initialType == detail::IDENTIFIER) { // varName | varName'['expression']' | subroutineCall
    tokenizer.peek();
    // TODO: find a better way of peeking
    if (tokenizer.tokenType() == detail::SYMBOL && (tokenizer.symbol() == '.' || tokenizer.symbol() == '(')) {
      tokenizer.resetPeek();
      compileSubroutineCall();
    } else if (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == '[') { // varName'['expression']'
      tokenizer.resetPeek();
      std::string varName = requireIdentifier(); // varName
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
  } else if (initialType == detail::SYMBOL) { // '('expression')' | unaryOp term
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
      requireSymbol('(');
      compileExpression();
      requireSymbol(')');
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
  requireKeyword(detail::IF);
  requireSymbol('(');
  compileExpression();
  writer.writeArithmetic(VMWriter::NOT);
  writer.writeIf(labelFalse);
  requireSymbol(')');
  requireSymbol('{');
  compileStatements();
  requireSymbol('}');
  writer.writeGoto(labelEnd);
  if (tokenizer.tokenType() == detail::KEYWORD && tokenizer.keyWord() == detail::ELSE) { // ('else''{'statements'}')?
    requireKeyword(detail::ELSE);
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
  requireKeyword(detail::WHILE);
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
  if (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == '.') {
    requireSymbol('.');
    std::string subroutineName = requireIdentifier(); // subroutineName
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
  requireKeyword(detail::DO);
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
  requireKeyword(detail::RETURN);
  if (!(tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ';')) {
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
  while (tokenizer.tokenType() == detail::KEYWORD && (
    tokenizer.keyWord() == detail::IF ||
    tokenizer.keyWord() == detail::WHILE || 
    tokenizer.keyWord() == detail::LET || 
    tokenizer.keyWord() == detail::DO ||
    tokenizer.keyWord() == detail::RETURN
  )) {
    if (tokenizer.keyWord() == detail::IF) {
      compileIf();
    } else if (tokenizer.keyWord() == detail::WHILE) {
      compileWhile();
    } else if (tokenizer.keyWord() == detail::RETURN) { // TODO: ensure that return is at the bottom of the statements
      compileReturn();
    } else if (tokenizer.keyWord() == detail::DO) {
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
  requireKeyword(detail::CLASS);
  className = requireIdentifier("Invalid class name.");
  requireSymbol('{');
  while (tokenizer.tokenType() == detail::KEYWORD && (tokenizer.keyWord() == detail::STATIC || tokenizer.keyWord() == detail::FIELD)) {
    compileClassVarDec();
  }
  while (tokenizer.tokenType() == detail::KEYWORD && (tokenizer.keyWord() == detail::CONSTRUCTOR || tokenizer.keyWord() == detail::FUNCTION || tokenizer.keyWord() == detail::METHOD)) {
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
  if (tokenizer.tokenType() != detail::KEYWORD) {
    throw std::invalid_argument("Invalid class variable declare");
  }

  if (tokenizer.keyWord() == detail::STATIC) {
    requireKeyword(detail::Keyword::STATIC);
    variableStorageType = SymbolicTable::Kind::STATIC;
  } else if (tokenizer.keyWord() == detail::Keyword::FIELD) {
    requireKeyword(detail::Keyword::FIELD);
    variableStorageType = SymbolicTable::Kind::FIELD;
  } else {
    throw std::invalid_argument("Invalid class variable declaration.");
  }
  
  if (tokenizer.tokenType() == detail::IDENTIFIER) {
    writeTag("<identifier>" + tokenizer.identifier() + "</identifier>");
    type = tokenizer.identifier();
    tokenizer.advance();
  } else if (tokenizer.tokenType() == detail::KEYWORD) {
    if (tokenizer.keyWord() == detail::INT) {
      requireKeyword(detail::INT);
      type = "int";
    } else if (tokenizer.keyWord() == detail::CHAR) {
      requireKeyword(detail::CHAR);
      type = "char";
    } else if (tokenizer.keyWord() == detail::BOOLEAN) {
      requireKeyword(detail::BOOLEAN);
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

  while (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ',') {
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
  if (tokenizer.tokenType() == detail::IDENTIFIER) {
    writeTag("<identifier>" + tokenizer.identifier() + "</identifier>");
    type = tokenizer.identifier();
    tokenizer.advance();
  } else if (tokenizer.tokenType() == detail::KEYWORD) {
    if (tokenizer.keyWord() == detail::INT) {
      requireKeyword(detail::INT);
      type = "int";
    } else if (tokenizer.keyWord() == detail::CHAR) {
      requireKeyword(detail::CHAR);
      type = "char";
    } else if (tokenizer.keyWord() == detail::BOOLEAN) {
      requireKeyword(detail::BOOLEAN);
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
  detail::TokenType type = tokenizer.tokenType();
  if (type != detail::KEYWORD) {
    throw std::invalid_argument("Invalid subroutine type.");
  }
  detail::Keyword keyword = tokenizer.keyWord();
  if (keyword == detail::CONSTRUCTOR) {
    requireKeyword(detail::CONSTRUCTOR);
  } else if (keyword == detail::FUNCTION) {
    requireKeyword(detail::FUNCTION);
  } else if (keyword == detail::METHOD) {
    requireKeyword(detail::METHOD);
    // Define "this" to the first argument
    // This needs to be defined FIRST before the other arguments, which are defined lower in compileVariable
    table.define("this", className, SymbolicTable::Kind::ARGUMENT);
  } else {
    throw std::invalid_argument("Invalid subroutine type.");
  }

  // Redefine type
  type = tokenizer.tokenType();
  if (type == detail::KEYWORD) {
    detail::Keyword keyword = tokenizer.keyWord();
    if (keyword == detail::VOID) {
      requireKeyword(detail::VOID);
    } else if (keyword == detail::BOOLEAN) {
      requireKeyword(detail::BOOLEAN);
    } else if (keyword == detail::INT) {
      requireKeyword(detail::INT);
    } else if (keyword == detail::CHAR) {
      requireKeyword(detail::CHAR);
    } else {
      throw std::invalid_argument("Invalid keyword for subroutine type.");
    }
  } else if (type == detail::IDENTIFIER) {
    requireIdentifier();
  } else {
    throw std::invalid_argument("Invalid subroutine return type.");
  }

  // subroutineName
  std::string functionName = requireIdentifier();

  requireSymbol('(');
  // parameterList = ( (type varName) (',' type varName)* )?
  writeTag("<parameterList>");
  if (tokenizer.tokenType() == detail::IDENTIFIER || tokenizer.tokenType() == detail::KEYWORD) { // ?
    compileVariable(); // (type varName)
    while (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ',') { // (',' type varName)*
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
  while (tokenizer.tokenType() == detail::KEYWORD && tokenizer.keyWord() == detail::VAR) {
    compileVarDec();
  }

  // VM write function
  unsigned int numberOfLocals = table.varCount(SymbolicTable::Kind::LOCAL);
  writer.writeFunction(className + "." + functionName, numberOfLocals);
  if (keyword == detail::CONSTRUCTOR) {
    // Allocate instance variables in heap memory (call to the OS for allocation)
    writer.writePush(VMWriter::CONST, table.varCount(SymbolicTable::Kind::FIELD));
    writer.writeCall("Memory.alloc", 1);
    writer.writePop(VMWriter::POINTER, 0); // anchors this at the base address (THIS = allocated space from Memory.alloc)
  } else if (keyword == detail::METHOD) {
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
  if (tokenizer.tokenType() == detail::KEYWORD && tokenizer.keyWord() == detail::VAR) {
    std::string type;

    requireKeyword(detail::VAR);
    // Vars
    if (tokenizer.tokenType() == detail::IDENTIFIER) {
      type = tokenizer.identifier();
      requireIdentifier();
    } else if (tokenizer.tokenType() == detail::KEYWORD) {
      if (tokenizer.keyWord() == detail::INT) {
        requireKeyword(detail::INT);
        type = "int";
      } else if (tokenizer.keyWord() == detail::CHAR) {
        requireKeyword(detail::CHAR);
        type = "char";
      } else if (tokenizer.keyWord() == detail::BOOLEAN) {
        requireKeyword(detail::BOOLEAN);
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
    while (tokenizer.tokenType() == detail::SYMBOL && tokenizer.symbol() == ',') {
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