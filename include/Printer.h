#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

class Printer {
	
	private:
		map<int, string>* _graphs;
		vector<string>* _performanceCounters;
		string* _sequenceIdParameter;
		vector<map<string, int> >* _combinations;

		map<string, map<int, vector<long long> > >* _resultX;
    	map<string, map<int, vector<long long> > >* _resultY; 

    	string *_unit;

	public:
		Printer(vector<map<string, int> >* combinations, map<int, string>* graphs, vector<string>* performanceCounters, string* sequenceIdParameter, map<string, map<int, vector<long long> > >* resultX, map<string, map<int, vector<long long> > >* resultY, string* unit);
		Printer(vector<map<string, int> >* combinations);
		void printResults();
		void printCombinations();

		long long getValue(size_t graph_id, string perf_ctr, size_t pos, map<string, int> parameters);

		virtual ~Printer();
};

#endif //PRINTER_H