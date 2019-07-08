#include <boost/filesystem.hpp>
#include "compiler.h"

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
