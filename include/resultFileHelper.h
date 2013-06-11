
#ifndef RESULTFILEHELPER_H
#define RESULTFILEHELPER_H

#include <string>

std::string findBenchName(std::string result_dir);
std::string findBenchId(std::string bench_name, std::string result_dir);

std::string getResultFile(std::string bench_name, std::string id, std::string result_dir);
std::string getParameterFile(std::string bench_name, std::string id, std::string result_dir);

std::string convertInt(int number);

bool fileExists(std::string file_name);

void pdfCropFile(std::string file_name);


#endif //RESULTFILEHELPER_H