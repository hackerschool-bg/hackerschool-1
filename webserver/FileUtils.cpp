#include "FileUtils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
using namespace std;

bool isFile(const char* path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

bool isDir(const char* path)
{
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

void dirLs(const char* dir, vector<string>& files)
{
	DIR* dp;
	dirent* dirp;
	if((dp  = opendir(dir)) == NULL)
	{
		perror("Failed to list files in directory");
		exit(1);
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		files.push_back(string(dirp->d_name));
	}
	closedir(dp);
}

bool fileExists(const char* name)
{
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}

string getExtension(const string& fname)
{
	size_t lastDot = fname.find_last_of(".");
	if (lastDot == string::npos) return "";
	if (fname.find("/",lastDot) != string::npos) return "";
	return fname.substr(lastDot+1);
}
