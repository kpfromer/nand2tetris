#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "compiler.h"

namespace {
  void removeCharsFromString(std::string & str, const std::string& charsToRemove) {
    for (unsigned int i = 0; i < charsToRemove.length(); ++i) {
      str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
    }
  };

  void sanitize(std::string & line) {
    removeCharsFromString(line, "\n\r\t");
  };

  void getFileLines (std::vector<std::string> & lines, std::string filePath) {
    std::string line;
    std::ifstream input (filePath);

    while (getline(input, line)) {
      sanitize(line);
      lines.push_back(line);
    }
    input.close();
  }
}

void compiler::Writer::forFile(const std::string & path, const bool & recursive, const std::function<void(boost::filesystem::path)> & fn) {
  if (boost::filesystem::is_directory(path)) {
    for(auto & p : boost::filesystem::directory_iterator(path)) {
      const boost::filesystem::path subPath = p.path();
      if (recursive && boost::filesystem::is_directory(subPath)) {
        forFile(subPath.string(), recursive, fn);
      } else if (boost::filesystem::is_regular_file(subPath) && boost::filesystem::extension(subPath) == ".jack") { // Note: this checks that the file has an extenstion
        fn(subPath);
      }
    }
  } else if (boost::filesystem::is_regular_file(path)) {
    fn(path);
  } else {
    throw std::invalid_argument("Can not concat a directory that does not exist.");
  }
}

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
        throw std::invalid_argument("Can not create a filename for directory with no parent.");
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
  return (fullPath.parent_path() / filename).string() + ".vm";
}


int compiler::Writer::compile(const std::string & inputPath, const std::string & outputPath) {

  PATH_TYPE inputType = getType(inputPath);
  PATH_TYPE outputType = getType(outputPath);

  std::vector<std::string> lines;
  std::vector<std::string> output;
  std::string currentFile;

  try {
    auto compileFile = [&](boost::filesystem::path filePath) {
      lines.clear();
      getFileLines(lines, filePath.string());
      currentFile = filePath.filename().string();
      CompilationEngine engine (lines, output);
    };

    forFile(inputPath, true, compileFile);

    std::ofstream outputFile (outputPath);
    for (unsigned int i = 0; i < output.size(); i++) {
      outputFile << output.at(i) << std::endl;
    }
    outputFile.close();

    return 0;
  } catch(const std::invalid_argument & error) {
    std::cerr << currentFile << error.what() << std::endl;
    return 1;
  }
}

int compiler::Writer::compile(const std::string & inputPath) {
  return compile(inputPath, getOutputFileLocation(inputPath));
}