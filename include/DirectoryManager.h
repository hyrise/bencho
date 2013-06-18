/** @file DirectoryManager.h
 *
 * Contains the class definition of DirectoryManager
 *
 */

#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <string>
#include <vector>

/**
 * @brief Helper class for file/directory management
 *
 * Directory Manager provides you with helper functions to e.g.
 * get the Id/Filename of a benchmark or remove files.
 */
class DirectoryManager
{
private:
    std::string _main_dir;
    std::string _id;

    /**
     * @param dir The directory which is to be read.
     *
     * @return A vector containing strings of files in the directory @a dir.
     */
    std::vector<std::string> listDirectory(std::string dir);

public:
    DirectoryManager();

    /**
     * Reads in the id of a benchmark.
     *
     * @param bench_name The name of the benchmark.
     *
     * @return The id of the benchmark @a bench_name as @c string.
     */
    std::string getId(std::string bench_name);

    /**
     * @param bench_name The name of the benchmark.
     * @param filetype The type of the file.
     *
     * @return The relative path to the file.
     */
    std::string getFilename(std::string bench_name, std::string filetype);

    /**
     * Removes the file @a filename.
     *
     * @param filename The file to remove.
     *
     * @return Integer value whether the removal succeeded or not.
     */
    int removeFile(std::string filename);
};

#endif //DIRECTORYMANAGER_H
