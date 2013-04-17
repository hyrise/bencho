#ifndef ABSTRACTBENCHMARK_H
#define ABSTRACTBENCHMARK_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>

#include "PapiSingleton.h"		//class for performance counters
#include "Parameter.h"    		//class for Parameters
#include "Printer.h"			//class for printing final and intermediate results
#include "Aggregator.h" 		//class for aggregator-functions
#include "FileWriter.h"			//class to write result files
#include "DirectoryManager.h"	//class to manage directories
#include "Gnup.h"				//class for plotting results with gnuplot
#include "Pyp.h"				//class for plotting results with python matplotlib
#include "Rp.h"					//class for plotting results with R ggplot2

using namespace std;

const int combination_incache = -1;

class FileWriter;

class AbstractBenchmark {

	protected:
		size_t _max_runs;
		size_t _warm_up_runs;
		size_t _calibration_runs;
		bool _is_initialized;
		bool _do_incache_calibration;
		double _max_deviation;
		string _name;
		string _sequence_id_parameter;
		string _unit;
		string _current_version;
		bool _do_papi_manual;
		const char* _current_perf_counter;
		long long _manual_papi_result;

		AggregationType::Function _aggregatingFunction;

		//parameters, input and other stuff for execution
		map<string, vector<Parameter> > _parameters; //list of parameters for execution
		map<string, int> _parameters_incache;

		vector<map<string, int> > _combinations; //combinations to calculate in calcCombinations()

		vector<string> _arguments;

		vector<string> _performance_counters; //measured performance counters

		map<int, string> _test_series;
		map<int, string> _graphs;

		//results for output
    	map<string, map<int, vector<long long> > > _result_x; // result x -> sequence id or xdata
    	map<string, map<int, vector<long long> > > _result_y;
    	// std error
    	map<string, map<int, vector<long long> > > _result_error;
    	// calibration results
    	map<string, long long> _result_calibration;

		FileWriter *_filewriter;
		DirectoryManager _directorymanager;
  		//Plotter *_plotter;		//später

		//modes for supressing output and running fastest possible configuration
  		bool _fastMode;
  		bool _silentMode;

		void init();

		void setUnit(string unit); //happens automatically

		virtual void finalize() = 0;
    	virtual void initialize() = 0;

    	void calcCombinations();
    	void executeCombination(map<string, int> parameters, int combination);
        long long executeRun(map<string, int> parameters, int combination, int test_series_id, int run, string perf_ctr, bool incache = false);

		// Methods to implement in specialized class
	    
	    // does the actual test run. execution time is measured. should be overwritten and implemented by specialized class.
	    virtual void doTheTest(map<string, int> parameters, int combination, int test_series_id, int run) = 0;
	    
	    virtual int getSequenceId(map<string, int> parameters, int test_series_id) const;

	    // prepare one run (executed before do_the_test)
	    virtual void prepareRun(map<string, int> parameters, int combination, int test_series_id, int run);
	    
	    // prepare one run (executed before do_the_test, but after cache cleared)
	    virtual void prepareRunAfterCacheCleared(map<string, int> parameters, int combination, int test_series_id, int run);

	    // prepare one combination (executed once before execute_combination)
	    virtual void prepareCombination(map<string, int> parameters, int combination);

	    // finish one run (executed after do_the_test)
	    virtual void finishRun(map<string, int> parameters, int combination, int test_series_id, int run);
	    
	    // finish one combination (executed after execute_combination)
	    virtual void finishCombination(map<string, int> parameters, int combination);
	    
	    // prepare start (executed once)
	    virtual void prepareStart() { };

	    void calibrateInCache();

	public:
		AbstractBenchmark();
    	virtual ~AbstractBenchmark();
 
		//add a parameter for execution
		void addParameter(Parameter *parameter, string version = "first");

		void addPerformanceCounter(string event_name);
		void addTestSeries(int id, string name);
		void addGraph(int id, string name);
    	void addAllTestSeriesAsGraphs();
    	void addTestSeriesAsGraph(int test_series_id);

		//output methods
		FileWriter *getFileWriter();
		DirectoryManager *getDirectoryManager();

		int full(int max_runs = -1, double max_deviation = -1);
		void printResults();
		void printCombinations();
		void plotResults(bool isDefault = true);
		void plotResultsWithGnuplot(bool isDefault = true);
		void plotResultsWithPython(bool isDefault = true);
		void plotResultsWithR(bool isDefault = true);

		void setName(string name);
		void setSequenceId(string id);
		void setWarmUpRuns(size_t warm_up_runs);
		void setMaxRuns(size_t max_runs);
		void setFastMode(bool fastMode);
		void setSilentMode(bool silentMode);
		void setAggregatingFunction(AggregationType::Function function);

		string getName();
		string getUnit();
		size_t getWarmUpRuns();
		size_t getMaxRuns();
		virtual long long getValue(size_t graph_id, string perf_ctr, size_t pos, map<string, int> parameters);
		bool getFastMode();
		bool getSilentMode();
		AggregationType::Function getAggregatingFunction();
    	vector<map<string, int> > &getCombinations();

		string getCurrentVersion();
		vector<Parameter> *getParameters();
		size_t getRowCount();
		map<int, string> &getTestSeries();
		vector<string> &getPerformanceCounters();

		vector<long long> &getResult_x(size_t test_series, string perf_ctr);
    	vector<long long> &getResult_y(size_t test_series, string perf_ctr);
    	vector<long long> &getResult_error(size_t test_series, string perf_ctr);
    	long long getResult_incache(string test_series);

        void execute(int max_runs, double max_deviation);

        void displayHeader();
        void clearCache();
};

#endif //ABSTRACTBENCHMARK_H