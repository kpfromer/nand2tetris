#include <string>
#include <vector>
#include <map>
#include "argument.h"

inline std::string arg::Argument::cleanArg(const std::string & arg) {
  return arg.substr(1);
}

inline std::string arg::Argument::toArg(const std::string & arg) {
  return "-" + arg;
}

std::map<std::string, std::string> arg::Argument::evaluate (const int & argc, const char ** & args) {
  std::map<std::string, std::string> returnVal;

  for (unsigned int i = 0; i < argc; i++) {
    std::string arg (args[i]);
    if (i < data.values.size()) {
      returnVal.insert(data.values.at(i).name, arg);
    } else {
      bool found = false;
      for (auto option : data.options) {
        // Default is false
        returnVal.insert(option.name, false);

        for (auto type : option.validTypes) {
          if (cleanArg(args[i]) == type) {
            found = true;

            returnVal.insert(option.name, arg);

            break;
          }
        }
        if (found) {
          break;
        }
      }
      if (!found) {
        throw std::invalid_argument("No such argument \"" + arg + "\"");
      }
    }
  }
}

arg::Argument::Argument & arg::Argument::value (const std::string & name, const std::string & description) {  
  arg::Argument::Value newValue;
  newValue.name = name;
  newValue.description = description;
  data.values.emplace_back(newValue);
  return *this;
}

arg::Argument::Argument &arg::Argument::option (const std::string & name, const std::string & option, const std::string & description) {
  arg::Argument::Option newOption;
  newOption.name = name;
  newOption.description = description;
  newOption.validTypes.push_back(option);
  data.options.emplace_back(newOption);
  return *this;
}