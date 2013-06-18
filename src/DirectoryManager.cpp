#include "DirectoryManager.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>


std::vector<std::string> DirectoryManager::listDirectory(std::string dir)
{
    std::vector<std::string> files;
    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(dir.c_str())) == NULL)
    {
        throw std::runtime_error("Error opening directory.");
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(std::string(dirp->d_name));
    }

    closedir(dp);
    return files;
}


DirectoryManager::DirectoryManager()
{
    _id = "";
    _main_dir = "./results/";
    mkdir(_main_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
}

std::string DirectoryManager::getId(std::string bench_name)
{
    if (_id.size() == 0)
    {
        int new_id;
        std::string line;
        std::string bench_dir = _main_dir + bench_name + "/";
        std::string id_filename = bench_dir + "id.txt";
        mkdir(bench_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        std::ofstream id_file;
        std::vector<std::string> files = listDirectory(bench_dir);

        std::ifstream id_file_in;
        id_file_in.open (id_filename.c_str());

        if (id_file_in)
        {
            id_file_in >> new_id;
            id_file_in.close();
            ++new_id;
        }
        else
        {
            new_id = 1;
        }

        id_file.open (id_filename.c_str(), std::ios::trunc);
        id_file << new_id;
        id_file.close();

        std::ostringstream id_stream;
        id_stream << new_id;

        _id = id_stream.str();
    

        // Creates .txt - file to identifie last used Benchmark.
        std::string last_used = _main_dir + "last.txt";
        std::ofstream last_used_in;
        last_used_in.open (last_used.c_str(), std::ios::trunc);
        last_used_in << bench_name;
        last_used_in.close();
        //
    }
    return _id;
}

std::string DirectoryManager::getFilename(std::string bench_name, std::string filetype)
{
    std::string run = getId(bench_name);
    return _main_dir + bench_name + "/" + bench_name + "_" + run + filetype;
}

int DirectoryManager::removeFile(std::string filename)
{
    return remove(filename.c_str());
}
