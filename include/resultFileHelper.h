
#ifndef RESULTFILEHELPER_H
#define RESULTFILEHELPER_H

#include <stdlib.h>

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string findBenchName(string result_dir);
string findBenchId(string bench_name, string result_dir);

string getResultFile(string bench_name, string id, string result_dir);
string getParameterFile(string bench_name, string id, string result_dir);

bool fileExists(string file_name);

void pdfCropFile(string file_name);


#endif //RESULTFILEHELPER_H