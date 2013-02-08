#ifndef PDFCREATOR_H
#define PDFCREATOR_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include "AbstractBenchmark.h"

using namespace std;

class AbstractBenchmark;

class FileWriter
{

private:
    AbstractBenchmark *_benchmark;

public:
    FileWriter(AbstractBenchmark *benchmark);
    void addDiagram(string ps_path, map<string, vector<int> > parameters);
    void save();
    void dumpResult(AbstractBenchmark *benchmark);
    void psToPdf(string filename);
    void saveParameters(AbstractBenchmark *benchmark);

    vector<string> getHeaders(AbstractBenchmark *benchmark);
};

#endif
