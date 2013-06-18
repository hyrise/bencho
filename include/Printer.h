/** @file Printer.h
 *
 * Contains the class definiton of Printer.
 */

#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <map>
#include <vector>


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
		std::map<int, std::string>* _graphs;
		std::vector<std::string>* _performanceCounters;
		std::string* _sequenceIdParameter;
		std::vector<std::map<std::string, int> >* _combinations;

		std::map<std::string, std::map<int, std::vector<long long> > >* _resultX;
    	std::map<std::string, std::map<int, std::vector<long long> > >* _resultY; 

    	std::string *_unit;

	public:
		Printer(std::vector<std::map<std::string, int> >* combinations, std::map<int, std::string>* graphs, std::vector<std::string>* performanceCounters, std::string* sequenceIdParameter, std::map<std::string, std::map<int, std::vector<long long> > >* resultX, std::map<std::string, std::map<int, std::vector<long long> > >* resultY, std::string* unit);
		Printer(std::vector<std::map<std::string, int> >* combinations);
		void printResults();
		void printCombinations();

		long long getValue(size_t test_series_id, std::string perf_ctr, size_t pos);

		virtual ~Printer();
};

#endif //PRINTER_H
