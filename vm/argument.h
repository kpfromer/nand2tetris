#ifndef __ARGUMENT_H_
#define __ARGUMENT_H_

#include <string>
#include <vector>
#include <map>

namespace arg {
  class Argument {
    private:
      struct Value {
        std::string name;
        std::string description;
      };
      struct Option {
        std::string name;
        std::vector<std::string> validTypes;
        std::string description;
      };
      struct Data {
        std::vector<Value> values;
        std::vector<Option> options;
      };
      Data data;
      std::string cleanArg(const std::string & arg);
      std::string toArg(const std::string & arg);
    public:
      // Argument & allows for chaining
      // Value is used for path or specific value example: run 5 (5 is the value and run is the program name)
      Argument & value (const std::string & name, const std::string & description);
      Argument & option (const std::string & name, const std::string & option, const std::string & description);
      // Map of names to their string values
      std::map<std::string, std::string> evaluate(const int & argc, const char ** & args);
  };
}

#endif