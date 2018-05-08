#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <string>
#include <list>

class Directory {
public:
    Directory(const char* path, const char* extension);

    std::list<std::string> list();
    std::string fullpath(const std::string filename);

    bool hasExtension(const char* name, const char* ext) const;
private:

    std::string _path;
    std::string _extension;
};

#endif /* DIRECTORY_H_ */
