/** @file Printer.h
 *
 * Contains the class definiton of Printer.
 */

#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

/**
 * @brief Class for printing final and intermediate results.
 * 
 * Usually the Bencho Framework will provide you with a lot of output concerning
 * the status and the results of your running benchmark. However, if you want to get
 * some extra output or the standard output is deactivated (e.g. because you run in Silent Mode)
 * you can use this Printer. Therefore it should be sufficient to use the printResults()
 * and printCombinatinations() methods of the AbstractBenchmark class.
 */
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