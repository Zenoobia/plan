#include "Directory.h"

#include <string>
#include <list>
//#include <dirent_portable.h>
//#include "../include/dirent.h"

#define _UNICODE
#define UNICODE
#include <tinydir.h>
#include <cstring>

/*
#if ((defined _WIN32) && (defined _UNICODE))
	_wfopen(
#else
	fopen(
#endif
		TINYDIR_STRING("/file/to/output"), TINYDIR_STRING("wb"));

#if ((defined _WIN32) && (defined _UNICODE))
  fwrite(bom, 1, 2, fp);
#endif
  */

Directory::Directory(const char* path, const char* extension) :
        _path(path), _extension(extension) {
}

std::list<std::string> Directory::list() {
    std::list<std::string> files;
    tinydir_dir dir;

    if (tinydir_open(&dir, _path.c_str()) == -1)
    {
	    perror("Error opening file");
	    goto bail;
    }

    while(dir.has_next) {
	    tinydir_file file;
	    if(tinydir_readfile(&dir, &file) == -1)
	    {
		    perror("Error getting file");
		    goto bail;
	    }

	    if(!file.is_dir && hasExtension(file.name, _extension.c_str())) {
		    files.push_back(std::string(file.name));
	    }


	    if (tinydir_next(&dir) == -1)
	    {
		    perror("Error getting next file");
		    goto bail;
	    }
    }

bail:
    tinydir_close(&dir);
    return files;
}

std::string Directory::fullpath(const std::string filename) {
	std::string path(_path);
	path += "/";
	path += filename;
	return path;
}

bool Directory::hasExtension(const char* name, const char* ext) const {
	if (NULL == name || NULL == ext) {
		return false;
	}
	size_t blen = strlen(name);
	size_t slen = strlen(ext);
	return (blen >= slen) && (0 == strcmp(name + blen - slen, ext));
}
