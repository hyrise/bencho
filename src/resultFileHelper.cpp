
#include "resultFileHelper.h"


string findBenchName(string result_dir)
{
	string last_name = "";
	string last_file = result_dir + "/last.txt";

	ifstream last_name_in;
	last_name_in.open (last_file.c_str());

	if(last_name_in)
	{
		last_name_in >> last_name;
		last_name_in.close();
	} else { cerr << "Couldn't open " << last_file << "." << endl; }
	return last_name;
}

string findBenchId(string bench_name, string result_dir)
{
	string actual_id = "";
	string id_file = result_dir + "/" + bench_name + "/id.txt";

	ifstream actual_id_in;
	actual_id_in.open(id_file.c_str());

	if(actual_id_in)
	{
		actual_id_in >> actual_id;
		actual_id_in.close();
	} else { cerr << "Couldn't open " << id_file << "." << endl; }
	return actual_id;
}

string getResultFile(string bench_name, string id, string result_dir)
{
	string resultFile = result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".result.csv";
	return resultFile;
}

string getParameterFile(string bench_name, string id, string result_dir)
{
	string parameterFile = result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".parameter.txt";
	return parameterFile;
}