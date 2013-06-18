#include "FileWriter.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>

#include "DirectoryManager.h"
#include "Parameter.h"
#include "ConfigFile.h"

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

extern char* global_program_invocation_name;

FileWriter::FileWriter(AbstractBenchmark *benchmark)
{
    _benchmark = benchmark;
}

void FileWriter::psToPdf(string filename)
{
    string command = ConfigFile::getExecutable("ps2pdf") + " " + filename + ".ps " + filename + ".pdf";
    _benchmark->getDirectoryManager()->removeFile(filename + ".ps");
    std::cout << "Chart written to " << filename << ".pdf" << std::endl;
}

void FileWriter::saveParameters(AbstractBenchmark *benchmark)
{
    std::ofstream file;
    std::vector<Parameter>::iterator p;


    file.open (benchmark->getDirectoryManager()->getFilename(benchmark->getName(), ".parameter.txt").c_str());
    
    file << benchmark->getCurrentVersion() << std::endl;
    file << std::endl << benchmark->getName() << std::endl;
    file << "################################" << std::endl;

    for (p = benchmark->getParameters()->begin(); p != benchmark->getParameters()->end(); p++)
    {

        if (p->getValues().size() == 1)
        {
            file << p->getName() << ": " << p->getValues().at(0) << std::endl;
        }
        else if (p->getValues().size() > 1)
        {
            file << p->getName() << ": from " << p->getValues().front() << " to " << p->getValues().back() << " in " << p->getValues().size() << " steps." << std::endl;
        }
    }
    
    char hostname[512];
    gethostname(hostname, 512);
    
    file << "Computer: " << hostname << std::endl;
    //file << "Executable: " << global_program_invocation_name << std::endl;  //ersetzten?
    
    // copy setting.conf
    file << "Settings: ";
    std::ifstream settings_file(STR(BENCHO_DIR)"/settings.conf");
    if (settings_file.is_open())
    {
        string settings_line;
        string settings_line_after;
        getline(settings_file, settings_line);
        while (settings_file.good())
        {
            getline(settings_file, settings_line_after);
            file << settings_line;
            if (!settings_line_after.empty()) file << ", ";
            settings_line = settings_line_after;
        }
        settings_file.close();
    }
    
    file << std::endl;
    file.close();

    std::cout << "Parameters written to " << benchmark->getDirectoryManager()->getFilename(benchmark->getName(), ".parameter.txt") << std::endl;
}

void FileWriter::dumpResult(AbstractBenchmark *benchmark)
{
    std::ofstream file;
    std::map<int, string>::iterator p;
    size_t lines = benchmark->getRowCount();
    file.open (benchmark->getDirectoryManager()->getFilename(benchmark->getName(), ".result.csv").c_str());

    

    // write headers

    file << "x ";

    std::vector<std::map<string, int> > &combinations = benchmark->getCombinations();

    std::map<string, int>::iterator it2;
    for (it2 = combinations[0].begin(); it2 != combinations[0].end(); it2++)
        file << "par_" << it2->first << " ";

    std::map<int, string>::iterator it;
    for (it = benchmark->getTestSeries().begin(); it != benchmark->getTestSeries().end(); it++)
    {
        file << it->second << "_incache ";
        for (size_t i = 0; i < benchmark->getPerformanceCounters().size(); ++i)
        {
            file << it->second << "_" << benchmark->getPerformanceCounters()[i] << "_y ";
            file << it->second << "_" << benchmark->getPerformanceCounters()[i] << "_error ";

            if (benchmark->getRawDataOutput() && !benchmark->getFastMode())
            {
                for (size_t j = 0; j < benchmark->getMaxRuns(); ++j)
                {
                    file << it->second << "_" << benchmark->getPerformanceCounters()[i] << "_y_raw_" << j << " ";
                }
            }
        }
    }

    file << std::endl;

    // write data

    for (size_t line = 0; line < lines; ++line)
    {
        // write sequence id
        file << benchmark->getResult_x(0, benchmark->getPerformanceCounters()[0]).at(line) << " ";

        // write paramters
        for (it2 = combinations[line].begin(); it2 != combinations[line].end(); it2++)
            file << it2->second << " ";


        // write results
        for (it = benchmark->getTestSeries().begin(); it != benchmark->getTestSeries().end(); it++)
        {
            file << benchmark->getResult_incache(it->second) << " ";
            for (size_t i = 0; i < benchmark->getPerformanceCounters().size(); ++i)
            {
                file << benchmark->getResult_y(it->first, benchmark->getPerformanceCounters()[i]).at(line) << " ";
                file << benchmark->getResult_error(it->first, benchmark->getPerformanceCounters()[i]).at(line) << " ";

                if (benchmark->getRawDataOutput() && !benchmark->getFastMode()) 
                {
                    for (size_t j = 0; j < benchmark->getMaxRuns(); ++j)
                    {
                        file << benchmark->getResult_y_raw(it->first, benchmark->getPerformanceCounters()[i]).at(line).at(j) << " ";
                    }
                }
            }
        }

        file << std::endl;
    }

    file.close();

    std::cout << "CSV written to " << benchmark->getDirectoryManager()->getFilename(benchmark->getName(), ".result.csv") << std::endl;
}

std::vector<string> FileWriter::getHeaders(AbstractBenchmark *benchmark)
{
    std::vector<string> headers;

    std::map<int, string>::iterator it;
    for (it = benchmark->getTestSeries().begin(); it != benchmark->getTestSeries().end(); it++)
    {
        headers.push_back(it->second + "_incache ");
        for (size_t i = 0; i < benchmark->getPerformanceCounters().size(); ++i)
        {
            headers.push_back(it->second + "_" + benchmark->getPerformanceCounters()[i] + "_y");
            headers.push_back(it->second + "_" + benchmark->getPerformanceCounters()[i] + "_error");
        }
    }

    return headers;
}
