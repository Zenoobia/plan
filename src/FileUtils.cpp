/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joaqim $
   ======================================================================== */
#include "FileUtils.h"

namespace pl {
  static std::vector<std::string> getFilesInPath(const std::string &path) {

    const std::vector<std::string> supported_image_formats = {
      "png",
      "bmp",
      "tif",
      "jpeg"
    };


    std::vector<std::string> filesPaths;
    tinydir_dir dir;
    tinydir_open_sorted(&dir, "/path/to/dir");

    for (size_t i = 0; i < dir.n_files; i++) {
      tinydir_file file;
      tinydir_readfile_n(&dir, &file, i);

      if (!file.is_dir) {
	const auto str = file.name;
	for(std::string ext : supported_image_formats) {
	  if(std::memcmp( str, ext.c_str(), ext.size() ) == 0 )
	    {
	      filesPaths.emplace_back(path + file.name);
	    }
	}
      }
    }
    tinydir_close(&dir);
    return filesPaths;
  };

  static vector<string> listFolder(string path)
  {
    vector<string> folders;
    DIR *dir = opendir(path.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
	string folder_path = path + "/" + string(entry->d_name);
	folders.push_back(folder_path);
      }
    }
    closedir(dir);
    return folders;

  }
  static vector<string> listFile(string folder_path) {
    vector<string> files;
    DIR *dir = opendir(folder_path.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
	string file_path = folder_path + "/" + string(entry->d_name);
	files.push_back(file_path);
      }
    }
    closedir(dir);
    return files;
  }

}
