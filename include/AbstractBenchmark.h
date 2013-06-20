/** @file AbstractBenchmark.h
 * 
 * Contains the class definition of AbstractBenchmark.
 */

#ifndef ABSTRACTBENCHMARK_H
#define ABSTRACTBENCHMARK_H

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "Aggregator.h"

const int combination_incache = -1;

class Parameter;
class FileWriter;
class DirectoryManager;
class AbstractPlotter;

/**
 * @brief Abstract class for to be subclassed by benchmarks. 
 *
 * The AbstractBenchmark class is the main class of the Bencho Framework, since it is
 * used as the super class for all benchmarks. With it's helper classes it organizes
 * everything from the input parameters via the calculation and execution of the combinations
 * through to the creation of the result files.
 */
class AbstractBenchmark {

	protected:
		size_t _max_runs;
		size_t _warm_up_runs;
		size_t _calibration_runs;
		bool _is_initialized;
		bool _do_incache_calibration;
		double _max_deviation;
		std::string _name;
		std::string _sequence_id_parameter;
		std::string _unit;
		std::string _current_version;
		bool _do_papi_manual;
		const char* _current_perf_counter;
		long long _manual_papi_result;

		AggregationType::Function _aggregatingFunction;

		//parameters, input and other stuff for execution
		std::map<std::string, std::vector<Parameter> > _parameters; //list of parameters for execution
		std::map<std::string, int> _parameters_incache;

		std::vector<std::map<std::string, int> > _combinations; //combinations to calculate in calcCombinations()

		std::vector<std::string> _arguments;

		std::vector<std::string> _performance_counters; //measured performance counters

		std::map<int, std::string> _test_series;
		std::map<int, std::string> _graphs;

		std::vector<std::unique_ptr<AbstractPlotter> > _plotters;

		//results for output
    	std::map<std::string, std::map<int, std::vector<long long> > > _result_x; // result x -> sequence id or xdata
    	std::map<std::string, std::map<int, std::vector<long long> > > _result_y;
    	std::map<std::string, std::map<int, std::vector<std::vector<long long> > > > _result_y_raw; // unaggregated data eg. for boxplots
    	// std error
    	std::map<std::string, std::map<int, std::vector<long long> > > _result_error;
    	// calibration results
    	std::map<std::string, long long> _result_calibration;

		FileWriter *_filewriter;
		DirectoryManager *_directorymanager;

		//modes for supressing output and running fastest possible configuration
  		bool _fastMode;
  		bool _silentMode;

  		bool _rawOutput;

		void init();

		void setUnit(std::string unit); //happens automatically

		virtual void finalize() = 0;
    	virtual void initialize() = 0;

    	void calcCombinations();
    	void executeCombination(std::map<std::string, int> parameters, int combination);
        long long executeRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run, std::string perf_ctr, bool incache = false);

		// Methods to implement in specialized class
	    
	    /// This does the actual test run. execution time is measured. should be overwritten and implemented by specialized class.
	    virtual void doTheTest(std::map<std::string, int> parameters, int combination, int test_series_id, int run) = 0;
	    
	    virtual int getSequenceId(std::map<std::string, int> parameters, int test_series_id) const;

	    /// Prepare one run (executed before do_the_test)
	    virtual void prepareRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run);
	    
	    /// Prepare one run (executed before do_the_test, but after cache cleared)
	    virtual void prepareRunAfterCacheCleared(std::map<std::string, int> parameters, int combination, int test_series_id, int run);

	    /// Prepare one combination (executed once before execute_combination)
	    virtual void prepareCombination(std::map<std::string, int> parameters, int combination);

	    /// Finish one run (executed after do_the_test)
	    virtual void finishRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run);
	    
	    /// Finish one combination (executed after execute_combination)
	    virtual void finishCombination(std::map<std::string, int> parameters, int combination);
	    
	    /// Prepare start (executed once)
	    virtual void prepareStart() { };

	    void calibrateInCache();

	public:

		/**
		 * Constructor
		 */
		AbstractBenchmark();

		/**
		 * Destructor
		 */
    	virtual ~AbstractBenchmark();
 
		/**
		 * @brief Add parameters to the benchmark. 
		 *
		 * Please note, that this function expects a unique pointer to a Parameter object of
		 * Bencho's Parameter class. @see Parameter
		 *
		 * @param parameter A Pointer to a Parameter object.
		 * @param version A String specifying the benchmark version this Parameter belongs to.
		 */
		void addParameter(unique_ptr<Parameter> parameter, string version = "first");

		/**
		 * @brief Add counters for perfomance measurement. 
		 *
		 * This function takes PAPI perfomance counters and adds them to 
		 * the benchmark to be measured. For a complete list of performance counters
		 * take a look at <a href="md_PAPI-Counter.html">PAPI Performance Counters</a>.
		 *
		 * Note: If you do not have the PAPI library installed all performance counters will fall back to simple time measurement.
		 *
		 * @param event_name The name of a PAPI performance counter.
		 */
		void addPerformanceCounter(std::string event_name);

		/**
		 * @brief Register the different Test Series.
		 * 
		 * If you have different test series for your benchmark, like a random series and a sequenced series 
		 * you can add them all to your benchmark class via the addTestSeries() function. The ids should be unique.
		 *
		 * @param id A unique id for your test series.
		 * @param name The name of your test series.
		 */
		void addTestSeries(int id, std::string name);
		void addGraph(int id, std::string name);
		

		void addPerformanceCounter(string event_name);
		void addTestSeries(int id, string name);
		void addGraph(int id, string name);
    	void addAllTestSeriesAsGraphs();
    	void addTestSeriesAsGraph(int test_series_id);

    	/**
    	 * @brief Add a plotter to the benchmark.
    	 *
    	 * @param plotter A unique_ptr to an AbstractPlottrt
    	 */
    	void addPlotter(std::unique_ptr<AbstractPlotter> plotter);

		//output methods
		FileWriter *getFileWriter();
		DirectoryManager *getDirectoryManager();

		/**
		 * @brief Run the complete benchmark.
		 *
		 * This funktion gets called in the main function of each benchmark and 
		 * starts the whole execution process. The function takes a number of runs 
		 * each combination should be executed at maximum or a maximum deviation. If a 
		 * maximum deviation is specified each combination gets executed again and again until this
		 * deviation is reached. However, usually these values get passed via setMaxRuns() and 
		 * setMaxDeviation() in your benchmarks initialize() method, were thay will have the same effects.
		 * 
		 * @params max_runs The maximum times one combination should be executed.
		 * @params max_deviation The maximum deviation of teh results of each combination.
		 */
		int full(int max_runs = -1, double max_deviation = -1);
		void plotOnly();
		
		void printResults();
		void printCombinations();

		/**
		 * @brief Plots the results a benchmark run.
		 *
		 * Call this function to invoke plotting, either after running a benchmark
		 * or if you want to plot something independently.
		 *
		 * @param isDefault true if you want the framework to automatically check the last used benchmark + id, false if you want to set it yourself.
		 */
		void plotResults(bool isDefault = true);

		/**
		 * @brief Calls every defined plotter with specific settings.
		 *
		 * @param settingsPlotter Pointer to an AbstractPlotter which holds the settings for the other plotters.
		 */
		void callPlotterWithSettings(AbstractPlotter *settingsPlotter);

		/**
		 * @brief Calls one specific plotter.
		 *
		 * @param specificPlotter The plotter you want to plot the results.
		 * @param settingsPlotter The plotter that holds the settings.
		 * @param fileEnding The ending with which the plotting script file ends, e.g. ".gp", ".py", ".r".
		 */
		void callSpecificPlotter(AbstractPlotter *specificPlotter, AbstractPlotter *settingsPlotter, std::string fileEnding);
		void callSpecificPlotter(std::unique_ptr<AbstractPlotter> specificPlotter, AbstractPlotter *settingsPlotter, std::string fileEnding);

		/**
		 * @brief Set the benchmark name.
		 *
		 * Use this function to set the benchmark name. This name is used for identification throughout the 
		 * the Bencho framework.
		 *
		 * @params name Name of the specific benchmark.
		 */
		void setName(std::string name);

		/**
		 * @brief Set an input parameter to provide the values on the x-axis.
		 *
		 * This function takes a name of one of the parameters you add to the benchmark,
		 * which will then be used as x values in all plotted graphs.
		 *
		 * @params id Name of a parameter.
		 */
		void setSequenceId(std::string id);

		/**
		 * @brief Set the number of warm up runs.
		 *
		 * Use this function to specify warm up runs for your benchmark. These will be executed
		 * before every combination.
		 *
		 * @params warm_up_runs Number of warm up runs.
		 */
		void setWarmUpRuns(size_t warm_up_runs);

		/**
		 * @brief Set maximum of runs.
		 *
		 * Use this function to set the maximum amount of runs each benchmark should be executed.
		 *
		 * @params max_runs Maximum amount of runs.
		 */
		void setMaxRuns(size_t max_runs);

		/**
		 * @brief Set maximum deviation.
		 *
		 * Use this function to specify a maximum deviation the results of each 
		 * combination of your benchmarks should have. If this value is specified the benchmarks tests
		 * will be exetuted until the deviation of the runs is smaller than the specified max_deviation.
		 * Therefore it exetures the benchmark max_runs times and doubles the runs every iteration until
		 * the deviation decreases under its maximum value.
		 *
		 * @params max_deviation The maximum deviation.
		 */
		void setMaxDeviation(double max_deviation);

		/**
		 * @brief Activate the fast mode.
		 *
		 * In the fast mode the benchmark will be executed as fast as possible. Therefore
		 * all framework output is muted and each combination will run only once. This mode can also 
		 * be activated after compilation by running the benchmark via 
		 * \code
		 * 		> make run fast=1
		 * \endcode
		 *
		 * @params fastMode Flag for fast mode.
		 */
		void setFastMode(bool fastMode);

		/**
		 * @brief Activate the silent mode.
		 *
		 * The silent mode deactivates all output by the framework. Own output of your benchmark and
		 * and some errors will still be displayed. This mode can also be activated after
		 * compilation by running the benchmark via 
		 * \code
		 * 		> make run silent=1
		 * \endcode
		 *
		 * @params silentMode Flag for silent mode.
		 */
		void setSilentMode(bool silentMode);

		/**
		 * @brief Activate raw data outpu to csv.
		 *
		 * This flags enables the saving of all results of all runs for your benchmark,
		 * which then for example can be used for plotting.
		 *
		 * @params rawOutput Flag for the raw data output.
		 */
		void setRawDataOutput(bool rawOutput);

		/**
		 * @brief Set the aggregating function
		 *
		 * This should be used to set the function used to aggreagate the results 
		 * of each combination. Available are Average, Median, Min and Max.
		 * @see AggregationType
		 *
		 * @params function The aggregating function. 
		 */
		void setAggregatingFunction(AggregationType::Function function);

		/// Returns the benchmarks name.
		std::string getName();
		std::string getUnit();

		/// Returns the number of warm up runs.
		size_t getWarmUpRuns();

		/// Returns the maximum number of runs.
		size_t getMaxRuns();
		virtual long long getValue(size_t graph_id, std::string perf_ctr, size_t pos, std::map<std::string, int> parameters);
		
		/// Returns the fast mode flag.
		bool getFastMode();

		/// Returns the silent mode flag.
		bool getSilentMode();

		/// Returns the raw data output flag.
		bool getRawDataOutput();

		/// Returns the aggregating function.
		AggregationType::Function getAggregatingFunction();

		/// Returns all combinations.
    	std::vector<std::map<std::string, int> > &getCombinations();

    	/// Returns the current version of your Parameters.
		std::string getCurrentVersion();

		/// Returns the parameters.
		std::vector<Parameter> *getParameters();
		size_t getRowCount();

		/// Returns the current test series.
		std::map<int, std::string> &getTestSeries();

		/// Returns all specified performance counters.
		std::vector<std::string> &getPerformanceCounters();

		std::vector<long long> &getResult_x(size_t test_series, std::string performance_counter);
    	std::vector<long long> &getResult_y(size_t test_series, std::string performance_counter);
    	std::vector<std::vector<long long> > &getResult_y_raw(size_t test_series, std::string performance_counter);
    	std::vector<long long> &getResult_error(size_t test_series, std::string performance_counter);
    	long long getResult_incache(std::string test_series);

    	/// Executes the benchmark.
        void execute(int max_runs, double max_deviation);

        /// Displays used/missing perf.counters according to the [benchmark].gp - script.
        void displayHeader();

        /// Helper function to clear the cache before benchmark execution.
        void clearCache();
};

#endif //ABSTRACTBENCHMARK_H
