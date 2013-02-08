
#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <sys/stat.h>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>

using namespace std;

class DirectoryManager
{
private:
    string _main_dir;
    string _id;

    vector<string> listDirectory(string dir);

public:
    DirectoryManager();
    string getId(string bench_name);
    string getFilename(string bench_name, string filetype);
    int removeFile(string);
};

#endif //DIRECTORYMANAGER_H
