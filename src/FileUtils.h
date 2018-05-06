#if !defined(FILEUTILS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim Planstedt $
   ======================================================================== */

#define FILEUTILS_H
#include <vector>
#include <string>
#include <cstring>

#include "../include/tinydir.h" //TODO: Add path to CMakeLists
namespace pl {
  using std::vector;
  using std::string;

  static std::vector<std::string> getFilesInPath(const std::string &path);
  static vector<string> listFolder(string path);
  static vector<string> listFile(string folder_path);

}

#endif
