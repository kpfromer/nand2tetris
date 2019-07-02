#ifndef __COMPILER_H_
#define __COMPILER_H_

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

namespace compiler {

  enum TokenType {
    SYMBOL, KEYWORD, IDENTIFIER, INT_CONST, STRING_CONST, INVALID_TOKEN
  };
  enum Keyword {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC,
    FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, NULLVAL, THIS
  };
  // Handles the input
  // Ignores white space
  // Advance the input, one token at a time
  // Gets the "value" and "type" of the "current token"
  class JackTokenizer {
    private:
      const std::vector<std::string> & lines;
      unsigned int position = 0;
      unsigned int linePosition = 0;
      unsigned int tempLinePosition = 0;
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
      // Returns the type of the currentToken
      TokenType tokenType();
      // Returns the keyworkd which is the current token (this should only be called only if the tokenType is KEYWORD)
      Keyword keyWord();
      // Returns the character which is the current token (tokenType should be SYMBOL)
      char symbol();
      // Returns the identifier which is the current token (tokenType should be IDENTIFIER)
      std::string identifier();
      // Returns the integer value which is the current token (tokenType should be INT_CONST)
      int intVal();
      // Returns the string value without the double quotes which is the current token (tokenType should be STRING_CONST)
      std::string stringVal();
  };

  // Generates the compiler's output
  class CompilationEngine {
    private:
      const std::vector<std::string> & lines;
      std::vector<std::string> & output;
      JackTokenizer tokenizer;
    public:
      // Get input (TODO:) and then must immeditaly call compileClass
      CompilationEngine(const std::vector<std::string> & lines, std::vector<std::string> & output);
      void debug();
      void requireKeyword(const Keyword & keyword);
      void requireSymbol(const char & symbol);
      void requireIdentifier(const std::string & errorMessage = "Invalid identifier.");
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
      // Compiles a (possibly empty) comma-seperated list of expressions
      void compileExpressionList();
  };

  class Writter {
    private:
      enum PATH_TYPE { DIRECTORY, FILE, NOTFOUND };
      static PATH_TYPE getType(const std::string & path);
      static boost::filesystem::path getPathBaseName(const std::string& path);
    public:
      static std::string getOutputFileLocation(const std::string & path);
  };

  // Creates a JackTokenizer from the file
  // Creates an outputfile called .xml
  // Uses a created CompilationEngine to compile the input JackTokenizer into the outputFile
  // class JackAnalzyer
}

#endif