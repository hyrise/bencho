/** @file DirectoryManager.h
 *
 * Contains the class definition of DirectoryManager
 *
 */

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

/**
 * @brief Helper class for file/directory management
 *
 * Directory Manager provides you with helper functions to e.g.
 * get the Id/Filename of a benchmark or remove files.
 */
class DirectoryManager
{
private:
    string _main_dir;
    string _id;

    /**
     * @param dir The directory which is to be read.
     *
     * @return A vector containing strings of files in the directory @a dir.
     */
    vector<string> listDirectory(string dir);

public:
    DirectoryManager();

    /**
     * Reads in the id of a benchmark.
     *
     * @param bench_name The name of the benchmark.
     *
     * @return The id of the benchmark @a bench_name as @c string.
     */
    string getId(string bench_name);

    /**
     * @param bench_name The name of the benchmark.
     * @param filetype The type of the file.
     *
     * @return The relative path to the file.
     */
    string getFilename(string bench_name, string filetype);

    /**
     * Removes the file @a filename.
     *
     * @param filename The file to remove.
     *
     * @return Integer value whether the removal succeeded or not.
     */
    int removeFile(string filename);
};

#endif //DIRECTORYMANAGER_H
