#include "resultFileHelper.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


std::string findBenchName(std::string result_dir)
{
	std::string last_name = "";
	std::string last_file = result_dir + "/last.txt";

	std::ifstream last_name_in;
	last_name_in.open (last_file.c_str());

	if(last_name_in)
	{
		last_name_in >> last_name;
		last_name_in.close();
	} else { std::cerr << "Couldn't open " << last_file << "." << std::endl; }
	return last_name;
}

std::string findBenchId(std::string bench_name, std::string result_dir)
{
	std::string actual_id = "";
	std::string id_file = result_dir + "/" + bench_name + "/id.txt";

	std::ifstream actual_id_in;
	actual_id_in.open(id_file.c_str());

	if(actual_id_in)
	{
		actual_id_in >> actual_id;
		actual_id_in.close();
	} else { std::cerr << "Couldn't open " << id_file << "." << std::endl; }
	return actual_id;
}

std::string getResultFile(std::string bench_name, std::string id, std::string result_dir)
{
	std::string resultFile = result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".result.csv";
	return resultFile;
}

std::string getParameterFile(std::string bench_name, std::string id, std::string result_dir)
{
	std::string parameterFile = result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".parameter.txt";
	return parameterFile;
}

std::string convertInt(int number)
{
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

bool fileExists(std::string file_name)
{
	std::ifstream filestream;
	filestream.open(file_name.c_str());
	if(filestream)
	{
		filestream.close();
		return true;
	} else {
		filestream.close();
		return false;
	}
}

void pdfCropFile(std::string file_name)
{
	std::string cropCommand = "pdfcrop " + file_name + " " + file_name + " > /dev/null";

	if (int err = std::system(cropCommand.c_str()))
    {
        std::cerr << "Error during system call: " << err << std::endl;
    }
}

