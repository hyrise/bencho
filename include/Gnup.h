
#ifndef GNUP_H
#define GNUP_H

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "resultFileHelper.h"

using namespace std;

class Gnup
{
public:
	Gnup();
	Gnup(string result_dir, string bench_script_dir);
	~Gnup();

	void setUp(bool set_default);
	void setUp(string bench_name);
	void setUp(string bench_name, string bench_id);

	///// Setters /////
	void setBenchName(string bench_name);
	void setBenchId(string id);
	//void setTerminal(string terminal);

	///// Getters /////
	string getBenchName();
	string getBenchId();
	//string getTerminal();
	// string getResultFile(string bench_name, string id);
	// string getParameterFile(string bench_name, string id);

	///// Search /////
	// string findBenchName();
	// string findBenchId(string bench_name);

	///// Script merging and finalizing /////
	int createBaseScript(string bench_name);
	int createFinalScript(string bench_name, string id);
	void clearScript(string final_script, string result_file);
	void bufferSearchReplace(string replace_file, string search, string replace);
	// bool fileExists(string file_name);
	vector<string> getCounters(string script);
	int getCounterPosition(string counter, string csv);

	///// Plot /////
	void plot(string bench_name, string id);
	void plot();


private:
	string _result_dir;
	string _system_script_dir;
	string _bench_script_dir;
	string _bench_name;
	string _id;

	string _terminal;
};

#endif //GNUP_H