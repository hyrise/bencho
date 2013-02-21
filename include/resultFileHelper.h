
#ifndef RESULTFILEHELPER_H
#define RESULTFILEHELPER_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string findBenchName(string result_dir);
string findBenchId(string bench_name, string result_dir);

string getResultFile(string bench_name, string id, string result_dir);
string getParameterFile(string bench_name, string id, string result_dir);




#endif //RESULTFILEHELPER_H