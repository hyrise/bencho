

#include "DirectoryManager.h"


vector<string> DirectoryManager::listDirectory(string dir)
{
    vector<string> files;
    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(dir.c_str())) == NULL)
    {
        throw std::runtime_error("Error opening directory.");
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
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

string DirectoryManager::getId(string bench_name)
{
    if (_id.size() == 0)
    {
        int new_id;
        string line;
        string bench_dir = _main_dir + bench_name + "/";
        string id_filename = bench_dir + "id.txt";
        mkdir(bench_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        ofstream id_file;
        vector<string> files = listDirectory(bench_dir);

        ifstream id_file_in;
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

        id_file.open (id_filename.c_str(), ios::trunc);
        id_file << new_id;
        id_file.close();

        std::ostringstream id_stream;
        id_stream << new_id;

        _id = id_stream.str();
    

        // Creates .txt - file to identifie last used Benchmark.
        string last_used = _main_dir + "last.txt";
        ofstream last_used_in;
        last_used_in.open (last_used.c_str(), ios::trunc);
        last_used_in << bench_name;
        last_used_in.close();
        //
    }
    return _id;
}

string DirectoryManager::getFilename(string bench_name, string filetype)
{
    string run = getId(bench_name);
    return _main_dir + bench_name + "/" + bench_name + "_" + run + filetype;
}

int DirectoryManager::removeFile(string filename)
{
    return remove(filename.c_str());
}
