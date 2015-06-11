#ifndef FILEUTILS_H_
#define FILEUTILS_H_
#include <vector>
#include <string>

bool isFile(const char* path);

bool isDir(const char* path);

void dirLs(const char* dir, std::vector<std::string>& files);

bool fileExists(const char* name);

std::string getExtension(const std::string& fname);

#endif /* FILEUTILS_H_ */
