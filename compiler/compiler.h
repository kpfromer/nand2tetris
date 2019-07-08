#ifndef __COMPILER_H_
#define __COMPILER_H_

#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>

// SOME REASON THE Main.main function has to be the very first method!!!!

// Easier
// TODO: add for (inital, condition, increment) {}
// TODO: ensure that constructors return this;
// TODO: ensure that there is only one constructor (do before method overloading)
// TODO: ensure that all functions and methods return
// TODO: Remove need for explicit return this in constructor (typecheck that there is no return in constructor since the compiler will create it)
// Harder
// TODO: type checking (currently types are just for the user and don't serve any purpose)
// TODO: method overloading (symbolic table for methods, add an argument count to table for tracking)
// TODO: operator overloading (change + for strings return a new concatenated string)
// TODO: add ablity to create variables in if elif else while, etc (linked list of symbolic tables)
// TODO: add else if
// TODO: add why to reference this to get values ex. this.value = value
// TODO: remove need for let, do
// TODO: allow for creation of variables anywhere in statements

namespace compiler {

  // forward declare private enums
  namespace detail {
    enum TokenType {
      SYMBOL, KEYWORD, IDENTIFIER, INT_CONST, STRING_CONST, INVALID_TOKEN
    };
    enum Keyword {
      CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC,
      FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, NULLVAL, THIS
    };
  }

  // Handles the input
  // Ignores white space
  // Advance the input, one token at a time
  // Gets the "value" and "type" of the "current token"
  class JackTokenizer {
    private:
      struct Location {
        unsigned int position;
        unsigned int linePosition;
        unsigned int tempLinePosition;
      };
      const std::vector<std::string> & lines;
      unsigned int position = 0;
      unsigned int linePosition = 0;
      unsigned int tempLinePosition = 0;
      Location prePeek;
      std::string getToken();
      void nextPosition(unsigned int pos);
    public:
      JackTokenizer(const std::vector<std::string> & lines);
      void debug(const unsigned int & linePosition);
      void debug();
      unsigned int getLineNumber();
      unsigned int getLinePosition();
      // Are there more tokens left
      bool hasMoreTokens();
      // Gets the next token from the input and makes it the current token
      // Should only be called if hasMoreTokens is true
      // Initally there is no current token
      void advance();
      // Peeks forward (you need to reset to go back to original spot)
      void peek(const unsigned int & numberOfTokens = 1);
      void resetPeek();
      // Returns the type of the currentToken
      detail::TokenType tokenType();
      // Returns the keyworkd which is the current token (this should only be called only if the tokenType is KEYWORD)
      detail::Keyword keyWord();
      // Returns the character which is the current token (tokenType should be SYMBOL)
      char symbol();
      // Returns the identifier which is the current token (tokenType should be IDENTIFIER)
      std::string identifier();
      // Returns the integer value which is the current token (tokenType should be INT_CONST)
      int intVal();
      // Returns the string value without the double quotes which is the current token (tokenType should be STRING_CONST)
      std::string stringVal();
  };

  class SymbolicTable {
    public:
      enum Kind { // TODO: rename local to var
        FIELD, STATIC, ARGUMENT, LOCAL, NONE
      };
      struct Symbol {
        std::string name;
        std::string type;
        Kind kind;
        unsigned int numberOfType;
      };
      void print();
      // starts a new subroutine scope (resets the subroutines symbol table)
      void startSubroutine();
      // Adds a symbol to the table and checks a running count of each type added (throw error if NONE is the kind)
      void define(const std::string & name, const std::string & type, const Kind & kind);
      // Returns if the symbol by name exists
      bool exists(const std::string & name);
      // Gets the symbol in the table by name
      Symbol getSymbol(const std::string & name);
      // returns the number of variables in a given scope
      unsigned int varCount(const Kind & kind);
      // returns the kind of named identifier in the current scope and returns NONE if the identifier is unknown
      Kind kindOf(const std::string & name);
      // returns the type of the named identifier 
      // return NONE if there is no value found
      std::string typeOf(const std::string & name);
      // returns the index of the named identifier
      unsigned int indexOf(const std::string & name);
      // SymbolicTable(const SymbolicTable & nextTable);
    private:
      // Global CLASS
      unsigned int staticCount = 0;
      unsigned int fieldCount = 0;
      // Subroutine
      unsigned int argumentCount = 0;
      unsigned int localCount = 0;
      // TODO: in the future change staticScope, fieldScope = map<className, Symbol>
      // staticScope, fieldScope = map<name, Symbol>
      // Both are global to class (BTW this don't need to be seperate)
      std::map<std::string, Symbol> staticScope; // Only kind with STATIC
      std::map<std::string, Symbol> fieldScope; // Only kind with FIELD
      // subroutineScope = map<name, Symbol>
      // Local to method/function/constructor (subroutineScope should be refreshed for every new class)
      std::map<std::string, Symbol> subroutineScope; // ARGUMENT LOCAL
      // const SymbolicTable & nextTable;
  };

  class VMWriter {
    private:
      std::vector<std::string> & output;
      unsigned int labelCount = 0;
    public:
      VMWriter(std::vector<std::string> & lines);
      enum Segment {
        CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP
      };
      enum Arithmetic {
        ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
      };
      // Creates unique label
      std::string generateLabel();
      // writes a VM push command
      void writePush(const Segment & segment, const unsigned int & index);
      // writes a VM pop command
      void writePop(const Segment & segment, const unsigned int & index);
      // writes a VM arithmetic-logical command
      void writeArithmetic(const Arithmetic & command);
      // writes a VM label command
      void writeLabel(const std::string & label);
      // writes VM goto command
      void writeGoto(const std::string & label);
      // writes VM if-goto command
      void writeIf(const std::string & label);
      // Writes VM call command
      void writeCall(const std::string & name, const unsigned int & numArgs);
      // Writes VM function command
      void writeFunction(const std::string & name, const unsigned int & numLocals);
      // Writes VM return command
      void writeReturn();
  };

  // Generates the compiler's output
  class CompilationEngine {
    struct CompilationEngineOptions {
      bool onlyTokenize;
    };
    private:
      const std::vector<std::string> & lines;
      std::vector<std::string> & output;
      JackTokenizer tokenizer;
      SymbolicTable table;
      VMWriter writer;
      CompilationEngineOptions options;
      std::string className;
    public:
      CompilationEngine(const std::vector<std::string> & lines, std::vector<std::string> & output, CompilationEngineOptions options = { false });
      void debug();
      void error();
      void requireKeyword(const detail::Keyword & keyword);
      void requireSymbol(const char & symbol);
      std::string requireIdentifier(const std::string & errorMessage = "Invalid identifier.");
      void compileString();
      void compileKeyword();
      // VM (abstraction) helper methods
      void writePush(const std::string & name);
      void writePop(const std::string & name);
      // writes the xml tag to output if onlyTokenize is true
      void writeTag(const std::string & tag);
      // Compiles a complete class
      void compileClass();
      // Compiles a static variable declartion, or a field declaration (instance vars)
      void compileClassVarDec();
      // Compiles a complete method, function, or constructor (class methods)
      void compileSubroutineDec();
      // Compiles a subroutine's body
      void compileSubroutineBody();
      // Compiles a var declaration
      void compileVarDec();
      // Compiles a subroutine call
      void compileSubroutineCall();
      // Compiles a sequence of statements (does not handle the enclosing "{}")
      void compileStatements();
      // Compiles a let statement
      void compileLet();
      // Compiles an if statement, with possible else clause.
      void compileIf();
      // Compiles a while statement
      void compileWhile();
      // Compiles a do statement
      void compileDo();
      // Compiles a return statement
      void compileReturn();
      // Compiles an expression
      void compileExpression();
      // Compiles a variable like statement = (type varName)
      void compileVariable(const std::string & errorVariableName = "parameter");
      // Compiles a term
      // If the token is a identifier it must distinguish between a variable, array entry, or a subroutine call.
      // There is only a single look-ahead token which could be a  "[", "(", "." to distingush between the possibilites (If it is not one of those then don't advance!)
      void compileTerm();
      // Compiles a (possibly empty) comma-seperated list of expressions, returns the number of expressions
      unsigned int compileExpressionList();
  };

  class Writer {
    private:
      enum PATH_TYPE { DIRECTORY, FILE, NOTFOUND };
      static PATH_TYPE getType(const std::string & path);
      static boost::filesystem::path getPathBaseName(const std::string& path);
    public:
      static std::string getOutputFileLocation(const std::string & path);
  };


  // helper functions
  // private to compiler
  namespace detail {
    bool isOp(const char & value);
    bool isSymbol(const char & value);
    bool isKeyWord(const std::string & word);
    Keyword getKeyWord(const std::string & value);
    std::string getKeywordString(const Keyword & keyword);
    bool isDigit(const char & value);
    int getDigit(const char & value);
    compiler::VMWriter::Arithmetic getArithmeticType(const char & value);
    unsigned int toCharValue(const char & value);
    compiler::VMWriter::Segment convertKindToSegment(const compiler::SymbolicTable::Kind & kind);
    bool isNumber(const std::string & word, const bool & decimal = false);
    // TODO: future - double getNumber(const std::string & numberString);
    // Does not handle negitive numbers!
    unsigned int getNumber(const std::string & numberString);
  };
}

#endif