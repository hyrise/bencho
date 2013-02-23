.. _PAPI: http://icl.cs.utk.edu/papi/software/index.html

============
Introduction
============

This is the Documentation of the Bencho Benchmark Framework. Here you will find all nescessary information for using this framework with its library and tools to create your own benchmarks as easy as possible.


What's Bencho?
--------------

Bencho is a C++ framework that helps you create and run benchmarks on your own computer.

**Bencho's features at a glance:**

- Create benchmarks by using the AbstractBenchmark as template
- Add Parameters easily
- Measure different Performance Counters at the same time
- Register different test series
- Supports warm-up runs, calibration runs, chache clearing, parameter versions and much more
- All common aggregating functions included in an Aggregator
- Save results in CSV formated files
- Plot results directly via GnuPlot


Prerequesites
-------------

- Supportet Operating Systems: Linux and Mac OS
- The PAPI_ library needs to be installed in order to use the whole functionality of the Bencho Performance Counters (not available for Mac)


===========
Quick Start
===========

- check out the sample project at git@epic.plan.io:epic-benchosample.benchosample.git
- follow the instructons of the README


======
Set Up
======

This Section describes how to set up the Bencho Framework on your computer. If you are reading this documentation you probably already have the project's source code, but in case you do not and still want to use Bencho, ask David Schwalb or one of his research assistants for further information.


Config
------

In order to provide you with a mostly error-free environment, run

::

	> make config

or open the *settings.conf* file and change the flags manually.

**Debug/Production mode**

By default, the Bencho library gets build in Production Mode. If you are interested in developing this framework further you might want to change this to the Development or Debug Mode.

**Use PAPI**

If you want to use all the perfomance counters provided by the Bencho Framework the PAPI_ library has to be installed. However, since PAPI does not support any Mac OS yet, there is a fallback to simple time measurements implemented in the Bencho code. So if you are using Bencho on a Mac OS or do not have PAPI installed on your Linux System you will have to set the PAPI flag in the *settings.conf* file (or by using *make config*) to 0 or while compiling the library an error will be thrown.

**Verbose build**

If you also want to see what is going on while the Makefile is running, set the VERBOSE_BUILD flag to 1. Then all the commands executed while running the makefile will be printed to your command line.


File placement
--------------

The recommended structure of the Bencho project directory contains the following files in the working directory:

- Makefile which calls the Bencho Makefile as well as compiles and links your benchmarks
- benchmarks/ directory which contains your benchmarks (and GNUplot files)
- bencho/ directoy which contains the complete Bencho Framework
- include/ directory which contains eventually external include files for your benchmarks seperated in subdirectories named after the benchmarks

+-----------------------------------------+
|Structure of the Bencho project directory|
+-----------------------------------------+

::
	
				working directory
		/		|		\		\
	bencho/ 	    Makefile		benchmarks/	  include/

To get familiar with this check out the sample project at git@epic.plan.io:epic-benchosample.benchosample.git.
It contains exactly what is mentioned above, a sample benchmark (including GNUplot script), the Bencho framework as git-submodule and a sample Makefile.

=====
Usage
=====

To compile your benchmarks (see `Create a Benchmark`_) linked with the Bencho library type

::
	
	> make

For execution and afterwards result printing type

::
	
	> make run


To let the Makefile ignore one or more benchmarks just add the prefix '_' to it. Then it won't be compiled.

+----------+
| **Hint** |
+----------+

The integrated plotting funciton of the Bencho Tool uses GNUplot. Maybe later on we will implement other plotting options.
In order to provide you with a plot according to your benchmark results, you have to (if you want to use the included plot function) put a GNUplot script with your benchmark source files.
The script has to lay in the *benchmarks/* directory and has to have the same name as the benchmark itself (with .gp extension), like in the sample:

Benchmark: Stride.cpp

gp-script: Stride.gp

In the *benchmarks/* directory there also lays the *system.gp* script, which is the base script and links with the specific benchmark script automatically.

For the explicit structure the GNUplot script **has to** have, please take a look at the section `Structure of the GNUplot script`_.


Create a Benchmark
------------------

This section describes how to create the actual benchmarks with the Bencho Framework. It assumes that you will use the Bencho Tool. If you are planning to not use it you still would create your benchmarks the same way, but depending on what your project setup will be then, you might have to add some more code like a main function or the include line for the bencho header in order to get executable benchmarks (see `Independent Projects`_ for further information).

Creating benchmarks with the Bencho Framework will always start with creating a subclass of the AbstractBenchmark class using *class YourBenchmarkName: public AbstractBenchmark* and then continue with implementing the benchmarks functions as you need. In the following paragraphs all the functions you can override to create your benchmark will be explained to be used as a step by step guide for creating a benchmark, with code from the Stride benchmark as example. In addition to that you will find a detailed list of helper functions you can use under `Benchmark Functions`_. For a deeper understanding please make also sure to check out the code of the example benchmark provided with the framework.

*void initialize()*

	At first you will have to initialize your Benchmark. This means you will set a name for identification of the benchmark and you will set a Sequence Id, which has to be the name of one parameter (that varies), because this will be the values for the x axis of your graph. Then you can set all the other options like warm up runs or the maximum amount of runs. Furthermore you will add your performance counters here and set all the parameters you need for your benchmark (see `Parameter Class`_). As last step you will add all the test series you want to run and set the aggregating function that will be used to aggregate the singel runs. The example benchmark Stride for instance uses the following initialize() function:

	.. code-block:: c++
		:linenos:

		void initialize()
	        {
	            setName("Stride");
	            setSequenceId("stride");
	            setWarmUpRuns(0);
	            setMaxRuns(1);
	            
	            addPerformanceCounter("PAPI_TOT_CYC");
	            addPerformanceCounter("PAPI_L1_DCM");
	            addPerformanceCounter("PAPI_L2_DCM");

	            Parameter *stride = new Parameter("stride", 1, 262144+1, 4, true);
	            Parameter *jumps = new Parameter("jumps", 4096);
	            addParameter(*stride);
	            addParameter(*jumps);
	            
	            addTestSeries(0, "random");
	            addTestSeries(1, "sequential_forwards");
	            addTestSeries(2, "sequential_backwards");

	            setAggregatingFunction(AggregationType::Average);
	        }

*void prepareCombination(map<string, int> parameters, int combination)*

	In prepareCombination() you will implement everything yo need to specify one combination of parameters, which then is executed with the specified options like maximum and warm up runs. 

	.. code-block:: c++
		:linenos:

		void prepareCombination(map<string, int> parameters, int combination)
	        {
	            stride = parameters["stride"];
	            jumps = parameters["jumps"];
	            size = (size_t)parameters["stride"] * (size_t)(parameters["jumps"]+1);
	            
	            srand(time(NULL));
	            
	            posix_memalign((void**)&array_seq_f, 4096, sizeof(int*) * size);
	            posix_memalign((void**)&array_seq_b, 4096, sizeof(int*) * size);
	            posix_memalign((void**)&array_ran, 4096, sizeof(int*) * size);
	            
	            vector<int> v;
	            v.reserve(size);
	            
	            for(unsigned long long i=1; i<jumps;i++)
	                v.push_back(i*stride);
	            
	            // random
	            size_t i = 0; size_t r;
	            for(size_t m = jumps-1; m>1; --m)
	            {
	                do {
	                    r = rand() % m;
	                } while (v[r] ==  i);
	                array_ran[i] = (int*)&(array_ran[v[r]]);
	                i = v[r];
	                swap(v[r], v[m-1]);
	            }
	            array_ran[i] = (int*)&(array_ran[v[0]]);
	            array_ran[v[0]] = NULL;
	            
	            // sequential
	            for(unsigned long long k=0; k<jumps;k++)
	            {
	                array_seq_f[k*stride] = (int*)&(array_seq_f[(k+1)*stride]);
	            }
	            array_seq_f[jumps*stride] = NULL;
	            
	            // sequential
	            for(unsigned long long k=jumps; k>0;k--)
	            {
	                array_seq_b[k*stride] = (int*)&(array_seq_b[(k-1)*stride]);
	            }
	            array_seq_b[0] = NULL;

	        }


*void finishCombination(map<string, int> parameters, int combination)*

	This function will be called after executeCombination() and so can be used to finalize a complete combination. The example benchmark Stride for instance releases its arrays used in the combination:

	.. code-block:: c++
		:linenos:

		void finishCombination(map<string, int> parameters, int combination)
		{
		    delete array_seq_f;
		    delete array_seq_b;
		    delete array_ran;
		} 

*void prepareRun(map<string, int> parameters, int combination, int test_series_id, int run)*

	Every combination usually has some warm up runs and then it will be executed several times to get significant result by aggregating the single results. To prepare every single run you can use this function. The Stride benchmark for example uses this function to differentiate between the single test series:

	.. code-block:: c++
		:linenos:

		void prepareRun(map<string, int> parameters, int combination, int test_series_id, int run)
		{
		    clear();
		    sum = 0;
		    
		    switch (test_series_id)
		    {
		        case 0:
		            p = array_ran[0];  //for random test series
		            break;
		        case 1:
		            p = array_seq_f[0]; //for sequential forward test series
		            break;
		        case 2:
		            p = array_seq_b[parameters["stride"]*parameters["jumps"]]; //for sequential backward test series
		            break;
		   }

		}

*void prepareRunAfterCacheCleared(map<string, int> parameters, int combination, int test_series_id, int run)*

	Just like the prepareRun() function this gets called right before doTheTest(), so before running one test of the benchmark but unlike prepareRun() before calling prepareRumAfterCacheCleared() the cache gets cleared via clearCache().

*void finishRun(map<string, int> parameters, int combination, int test_series_id, int run)*

	This function gets called after each run of doTheTest(). Here you could print results, free objects or clear some variables. The example benchmark Stride for instance prints some intermediate results here:

	.. code-block:: c++
		:linenos:

		void finishRun(map<string, int> parameters, int combination, int test_series_id, int run)
		{
			cout << "sum: " << sum << p << endl;
		}

*void doTheTest(map<string, int> parameters, int combination, int test_series_id, int run)*

	In this function the actual test should be implemented. It is the function in which the time is measured and can be prepared in prepareRun() and finalized in finishRun(). For the Stride benchmark, the test looks like this:

	.. code-block:: c++
		:linenos:

		void doTheTest(map<string, int> parameters, int combination, int test_series_id, int run)
		{   
		    for (size_t i=0; i<jumps-1; ++i) {
		        p = *((int**)p);
		    }
		}

*Additional Functions*
	
	Not used in the sample benchmark, but also there for usage in your benchmarks, there are the additional functions *void prepareStart()* and *void finalize()*. Both functions will be called once during the whole benchmark run, prepareStart() before the benchmark starts and finalize() after it finished all runs.


Structure of the GNUplot script
-------------------------------

The *system.gp* that comes with the Bencho Tool contains all standard definitions for the gp script that you don't need to specify in every script for new benchmarks.
What you have to do (in case you want to use the GNUplot tool) is writing a *[benchmark].gp* script that contains some information of how you would like to have the plot.

For the *[benchmark].gp* itself you just have to set the important options for the specific benchmark like format, range and labeling. Therefore check the sample *Stride.gp*::

	# plot cycles
	###########################
	set log x 2
	set log x2 2
	set format x "2^{%L}"
	set xlabel "Stride in Bytes"
	set ylabel "CPU Cycles per Element"
	set xrange [1:262144]

	set x2tics ('[Cache Linesize]' 64, '[Pagesize]' 4096)
	set grid noxtics x2tics
	JUMPS = 4096

Afterwards, the plot command/commands follow in a little uncommon way. To make it possible for the tool to check, which performance counters are used and which are not, you have to create a gp script with all counters included the way we did in the sample::

	plot\
	    "DATAFILE" using (($1-1)*8):(§@random_PAPI_TOT_CYC_y@§/JUMPS) title "Random" ls 1 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@sequential_forwards_PAPI_TOT_CYC_y@§/JUMPS) title "Sequential" ls 2 with linespoints


	# plot random cache misses
	###########################
	set output "plot2.ps"
	set ylabel "Misses per Element"
	set format y "%.1f"


	plot\
	    "DATAFILE" using (($1-1)*8):(§@random_PAPI_L3_TCM_y@§/JUMPS) ls 3 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@random_PAPI_L1_DCM_y@§/JUMPS) ls 1 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@random_PAPI_L2_DCM_y@§/JUMPS) ls 2 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@random_PAPI_TLB_DM_y@§/JUMPS) ls 4 with linespoints
	    
	    
	# plot sequential cache misses
	###########################
	set output "plot3.ps"
	set ylabel "Misses per Element"
	set format y "%.1f"

	plot\
	    "DATAFILE" using (($1-1)*8):(§@sequential_forwards_PAPI_L1_DCM_y@§/JUMPS) ls 1 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@sequential_forwards_PAPI_L2_DCM_y@§/JUMPS) ls 2 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@sequential_forwards_PAPI_L3_TCM_y@§/JUMPS) ls 3 with linespoints,\
	    "DATAFILE" using (($1-1)*8):(§@sequential_forwards_PAPI_TLB_DM_y@§/JUMPS) ls 4 with linespoints

The filename you just have to name *"DATAFILE"*, it will be replaced automatically. For the row/column information of the single performance counters you have to write down the explicit name of the perf.-counter (which has to be the same as shown in the result file as column names later on), included by *§@* and *@§* (see the above example). They will be replaced automatically with the right row/column number dependent on which perf.-counters you use/how the result file looks like.

Execution Modes
---------------

The Bencho framework offers modes to specify how the benchmark should be run. The modes can either be set permanetly in the benchmark class with the according setMode(bool mode) function or added to the make run statement as make arguments. For example an execution in fast mode can be invoked by executing

::

	> make run fast=1

This will translate into the corresponding commandline arguments for your benchmark, making it possible to change running modes without recompiling your project.

The currently implemented modes are:

*Fast Mode*

	The Fast Mode executes the benchmark in the fastest possible configuration. It opresses the output, sets the maximum runs to 1 and does not use aggregation. To opress the output the Silent Mode is activated (and deactivated) automatically with the Fast Mode.

	.. code-block:: c++

		fastMode(true);

*Silent Mode*

	The Silent Mode executes the benchmark without the frameworks output (except important errors). So if you don't need its output and want to have the benchmarks output be represented clearly activate the silentMode with


	.. code-block:: c++

		silentMode(true);


=========
Reference
=========

The reference section contains the classes and functions you can or maybe have to use in order to create a benchmark with the Bencho Framework. Furthermore it includes a complete list of the PAPI_ performance counters you can measure during your test runs.

Parameter Class
---------------

The Parameter Class is a helper class of the Bencho Framework, that is used to register parameters for your benchmark. This is accomlished throug the overloaded constructors, which all create a standarized Parameter Object with a name for identification and a vector of long integer values. The following contructors are available at the moment: 

*Parameter::Parameter(string name, vector<long long> values)*
	
	This is the standard contructor, that transfers name and values directly to the object. No calculations will be done here.

*Parameter::Parameter(string name, long long start, long long stop, long long step, operation operation)*

	This contructor can be used to automatically calculate a vector of integer values by setting a start value, an end value and a step. Finally you can use values of the enum operation to determine, wether the steps should be added or multiplied.

	.. code-block:: c++

		enum operation {
		    add,
		    multiply
		};

*Parameter::Parameter(string name, long long value)*

	Of course you can also use only a single value as a parameter. This contructor will wrap your single integer value into the vector, that will be used by the frameworks functions.


Aggregator Class
----------------

The Aggregator Class is a helper class of the Bencho Framework, that is used to condition the test results via calculating averages, medians or deviations. To use this class, declare a new Aggregator object either with directly giving your data to the constructor or setting it afterwards via *void Aggregator::setData(vector<long long> &data)*. Once an object it created and initialized with its data you can call the following methods:

*long long Aggregator::calculateAverage()*

	This function will calculate the average of the given data and return it as a rounded integer value.

*long long Aggregator::calculateMedian()*

	This function will calculate the median of the data and return it as a rounded integer value.

*long long Aggregator::calculateDeviation()*

	This function will calculate the deviation of the given data for you and return it as a rounded integer value.

*long long Aggregator::calculateMin()*

	This function will return the minimum value of the given data.

*long long Aggregator::calculateMax()*

	This function will return the maximum value of the given data.

*long long Aggregator::calculateFunction(AggregationType::Function)*
	
	This function will return the result of whatever function type you pass as parameter. This can be Average, Median, Min or Max, which are all declared in the AggregationType struct coming with the Aggregator class.


Benchmark Functions
-------------------

In this Section all public or protected functions of the Abstract Benchmark class will be listed and explained in order to use them in your own benchmarks class. The functions are ordered alphabetically by their names.

*void addParameter(Parameter *parameter, string version = "first")*

	Each benchmark has some parameters that compose the single combinations, that will be tested. For adding these parameters this function can be used. It expects an pointer to an object of the `Parameter Class`_, that helps you add complex parameters simple with its overloaded contructors and a version string, that could be used for registering different versions of parameters. The version is set to 'first' by default. And don't worry about the Paramter object you used to add the Parameter to the benchmark. The addParameter() function will automatically take care of the now useless object and delete it.

*void addPerformanceCounter(string event_name)*

	Of course you don not only want to measure the time your benchmarks needs for a run, but you also want to measure cache misses or CPU cycles. Therefore you can add the PAPI performance counters. A complete list of these counters can be found under `PAPI Performance Counters`_.

	Note: If you do not have the PAPI library installed all performance counters will fall back to simple time measurement.

*void addTestSeries(int id, string name)*

	If you have different test series for your benchmark, like a random series and a sequenced series you can add them all to your benchmark class via the addTestSeries() function. The ids should be unique.

*void addAllTestSeriesAsGraphs()*

	If you want to have all test series registered for the printing output by printResults() then you can use this function. It is a shortcut, that can be used instead of calling the addTestSeriesAsGraph() function for every test series.

*void addTestSeriesAsGraph(int test_series_id)*

	This function will register a specific test series as graph, that then can be outputed by the printing function printResults().

*void clearCache()*

	This function can be used to clear the whole cache for an unaffected test.

*void full(int max_runs = -1, double max_deviation = -1)*

	This function can be ignored when using the Bencho Tool. However, if you write your own main functions for your benchmarks you will have to use the full() function to start the whole benchmark test. A standard main function for a benchmark could look like this:

	.. code-block:: c++
		:linenos:

		int main(int argc, const char *argv[])
		{
		    hyrise::benchmark::Stride *s = new hyrise::benchmark::Stride;

		    s->full();
		    delete s;

		    return 0;
		}

*string getCurrentVersion()*

	The getCurrentVersion() function will return the current version of the parameters the run uses.

*bool getFastMode()*

	The getFastMode() function will return boolean whether the Fast Mode is activated or not.

*size_t getMaxRuns()*
	
	This function returns the maximum runs set for the benchmark.

*string getName()*

	The getName() function will return the name of the benchmark currently running.

*vector<Parameter> \*getParameters()*

	The getParameters() function will return the complete vector of parameters.

*vector<string> &getPerformanceCounters()*

	The getPerformanceCounters() will return an pointer on the vector of performance counters used by this benchmark.

*vector<long long> &getResult_error(size_t test_series, string perf_ctr)*

	This function returns an pointer on the result errors for a specific test_series and a performance counter.

*long long getResult_incache(string test_series)*

	This function returns the result of the given test_series, that is currently in the cache.

*vector<long long> &getResult_x(size_t test_series, string perf_ctr)*

	This function returns the x result of the given test series and performance counter.

*vector<long long> &getResult_y(size_t test_series, string perf_ctr)*

	This function returns the y result of the given test series and performance counter.

*size_t getRowCount()*

	This function returns the size of the y result of the current performance counter.

*bool getSilentMode()*

	The getSilentMode() function will return boolean whether the Silent Mode is activated or not.

*map<int, string> &getTestSeries()*

	This function returns a pointer to the current test series.

*string getUnit()*

	This function returns the unit of the benchmark. This determines, if it uses the performance counters or the fall back clock ticks.
	
*size_t getWarmUpRuns()*
		
	This function will return the specified warm up runs for the benchmark.

*void printCombinations()*

	This function will prin t the combiantions calculated for the current parameters.

*void printResults()*

	This function will print out the results of the benchmark test for all test series added as graphs. For adding a test series as a graph use the addTestSeriesAsGraph() function or the addAllTestSeriesAsGraphs() function, that will add all the registered test series to the printing output.

*void setAggregatingFunction(AggregationType::Function)*

	Setter for the aggregating function, that will be used to aggregate the results of the single runs. You can pass all values of the Function enum in the AggregationType struct as parameter. At the moment these are Average, Median, Min and Max.

*void setFastMode(bool fastMode)*

	Setter to activate or deactivate the include Fast Mode. This will run your benchmarks in the fastest possible configuration and opresses the output of the Bencho framework. NOTE: in order to opress the output this function will automatically activate and deactivate the Silent Mode. 

*void setMaxRuns(size_t max_runs)*

	Setter for the maximum amount of runs used for the benchmark tests.

*void setName(string name)*

	Setter for the Benchmarks name. The name is used for identification in the Bencho Tool.

*void setSequenceId(string id)*

	Setter for the Sequence Id. This is the name of the parameter, whose values will be used for the x axis in the graph.

*void setSilentMode(bool silentMode)*

	Setter to activate or deactivate the included Silent Mode, that opresses all output generated by the Bencho framework.

*void setWarmUpRuns(size_t warmUpRuns)*

	Setter for the warm up runs, executed before running the actual benchmark test runs.


PAPI Performance Counters
-------------------------

This Section contains a complete table of the PAPI_ performance counters that can be used with the Bencho Framework. Please note, that if the PAPI library is not installed on your system, all performance counters will fall back to simple time measurement.

	+---------------+-------------------------------------------------------+
	|PAPI_EVENT     |Event measured                                         |
	+===============+=======================================================+
	|PAPI_L1_DCM	|Level 1 data cache misses                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_ICM	|Level 1 instruction cache misses                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_DCM	|Level 2 data cache misses                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_ICM	|Level 2 instruction cache misses                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_DCM	|Level 3 data cache misses                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_ICM	|Level 3 instruction cache misses                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_TCM	|Level 1 total cache misses                             |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_TCM	|Level 2 total cache misses                             |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_TCM	|Level 3 total cache misses                             |
	+---------------+-------------------------------------------------------+
	|PAPI_CA_SNP	|Snoops                                                 |
	+---------------+-------------------------------------------------------+
	|PAPI_CA_SHR	|Request for access to shared cache line (SMP)          |
	+---------------+-------------------------------------------------------+
	|PAPI_CA_CLN	|Request for access to clean cache line (SMP)           |
	+---------------+-------------------------------------------------------+
	|PAPI_CA_INV	|Cache Line Invalidation (SMP)                          |
	+---------------+-------------------------------------------------------+
	|PAPI_CA_ITV	|Cache Line Intervention (SMP)                          |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_LDM    |Level 3 load misses                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_STM	|Level 3 store misses                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_BRU_IDL   |Cycles branch units are idle                           |
	+---------------+-------------------------------------------------------+
	|PAPI_FXU_IDL   |Cycles integer units are idle                          |
	+---------------+-------------------------------------------------------+
	|PAPI_FPU_IDL   |Cycles floating point units are idle                   |
	+---------------+-------------------------------------------------------+
	|PAPI_LSU_IDL   |Cycles load/store units are idle                       |
	+---------------+-------------------------------------------------------+
	|PAPI_TLB_DM    |Data translation lookaside buffer misses               |
	+---------------+-------------------------------------------------------+
	|PAPI_TLB_IM    |Instruction translation lookaside buffer misses        |
	+---------------+-------------------------------------------------------+
	|PAPI_TLB_TL    |Total translation lookaside buffer misses              |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_LDM    |Level 1 load misses                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_STM    |Level 1 store misses                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_LDM    |Level 2 load misses                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_STM    |Level 2 store misses                                   |
	+---------------+-------------------------------------------------------+ 
	|PAPI_BTAC_M    |BTAC miss                                              |
	+---------------+-------------------------------------------------------+
	|PAPI_PRF_DM    |Prefetch data instruction caused a miss                |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_DCH    |Level 3 Data Cache Hit                                 |
	+---------------+-------------------------------------------------------+
	|PAPI_TLB_SD    |Translation lookaside buffer shootdowns (SMP)          |
	+---------------+-------------------------------------------------------+
	|PAPI_CSR_FAL   |Failed store conditional instructions                  |
	+---------------+-------------------------------------------------------+
	|PAPI_CSR_SUC   |Successful store conditional instructions              |
	+---------------+-------------------------------------------------------+
	|PAPI_CSR_TOT   |Total store conditional instructions                   |
	+---------------+-------------------------------------------------------+
	|PAPI_MEM_SCY   |Cycles Stalled Waiting for Memory Access               |
	+---------------+-------------------------------------------------------+
	|PAPI_MEM_RCY   |Cycles Stalled Waiting for Memory Read                 |
	+---------------+-------------------------------------------------------+ 
	|PAPI_MEM_WCY   |Cycles Stalled Waiting for Memory Write                |
	+---------------+-------------------------------------------------------+
	|PAPI_STL_ICY   |Cycles with No Instruction Issue                       |
	+---------------+-------------------------------------------------------+
	|PAPI_FUL_ICY   |Cycles with Maximum Instruction Issue                  |
	+---------------+-------------------------------------------------------+
	|PAPI_STL_CCY   |Cycles with No Instruction Completion                  |
	+---------------+-------------------------------------------------------+
	|PAPI_FUL_CCY   |Cycles with Maximum Instruction Completion             |
	+---------------+-------------------------------------------------------+
	|PAPI_HW_INT    |Hardware interrupts                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_UCN    |Unconditional branch instructions executed             |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_CN     |Conditional branch instructions executed               |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_TKN    |Conditional branch instructions taken                  |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_NTK    |Conditional branch instructions not taken              |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_MSP    |Conditional branch instructions mispredicted           |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_PRC    |Conditional branch instructions correctly predicted    |
	+---------------+-------------------------------------------------------+
	|PAPI_FMA_INS   |FMA instructions completed                             |
	+---------------+-------------------------------------------------------+
	|PAPI_TOT_IIS   |Total instructions issued                              |
	+---------------+-------------------------------------------------------+
	|PAPI_TOT_INS   |Total instructions executed                            |
	+---------------+-------------------------------------------------------+
	|PAPI_INT_INS   |Integer instructions executed                          |
	+---------------+-------------------------------------------------------+
	|PAPI_FP_INS    |Floating point instructions executed                   |
	+---------------+-------------------------------------------------------+
	|PAPI_LD_INS    |Load instructions executed                             |
	+---------------+-------------------------------------------------------+
	|PAPI_SR_INS    |Store instructions executed                            |
	+---------------+-------------------------------------------------------+
	|PAPI_BR_INS    |Total branch instructions executed                     |
	+---------------+-------------------------------------------------------+
	|PAPI_VEC_INS   |Vector/SIMD instructions executed                      |
	+---------------+-------------------------------------------------------+
	|PAPI_FLOPS     |Floating Point Instructions executed per second        |
	+---------------+-------------------------------------------------------+
	|PAPI_RES_STL   |Cycles processor is stalled on resource                |
	+---------------+-------------------------------------------------------+
	|PAPI_FP_STAL   |FP units are stalled                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_TOT_CYC   |Total cycles                                           |
	+---------------+-------------------------------------------------------+
	|PAPI_IPS       |Instructions executed per second                       |
	+---------------+-------------------------------------------------------+
	|PAPI_LST_INS   |Total load/store instructions executed                 |
	+---------------+-------------------------------------------------------+
	|PAPI_SYC_INS   |Synchronization instructions executed                  |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_DCH    |L1 D Cache Hit                                         |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_DCH    |L2 D Cache Hit                                         |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_DCA    |L1 D Cache Access                                      |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_DCA    |L2 D Cache Access                                      |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_DCA    |L3 D Cache Access                                      |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_DCR    |L1 D Cache Read                                        |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_DCR    |L2 D Cache Read                                        |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_DCR    |L3 D Cache Read                                        |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_DCW    |L1 D Cache Write                                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_DCW    |L2 D Cache Write                                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_DCW    |L3 D Cache Write                                       |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_ICH    |L1 instruction cache hits                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_ICH    |L2 instruction cache hits                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_ICH    |L3 instruction cache hits                              |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_ICA    |L1 instruction cache accesses                          |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_ICA    |L2 instruction cache accesses                          |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_ICA    |L3 instruction cache accesses                          |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_ICR    |L1 instruction cache reads                             |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_ICR    |L2 instruction cache reads                             |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_ICR    |L3 instruction cache reads                             |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_ICW    |L1 instruction cache writes                            |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_ICW    |L2 instruction cache writes                            |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_ICW    |L3 instruction cache writes                            |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_TCH    |L1 total cache hits                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_TCH    |L2 total cache hits                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_TCH    |L3 total cache hits                                    |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_TCA    |L1 total cache accesses                                |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_TCA    |L2 total cache accesses                                |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_TCA    |L3 total cache accesses                                |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_TCR    |L1 total cache reads                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_TCR    |L2 total cache reads                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_TCR    |L3 total cache reads                                   |
	+---------------+-------------------------------------------------------+
	|PAPI_L1_TCW    |L1 total cache writes                                  |
	+---------------+-------------------------------------------------------+
	|PAPI_L2_TCW    |L2 total cache writes                                  |
	+---------------+-------------------------------------------------------+
	|PAPI_L3_TCW    |L3 total cache writes                                  |
	+---------------+-------------------------------------------------------+
	|PAPI_FML_INS   |FM ins                                                 |
	+---------------+-------------------------------------------------------+
	|PAPI_FAD_INS   |FA ins                                                 |
	+---------------+-------------------------------------------------------+
	|PAPI_FDV_INS   |FD ins                                                 |
	+---------------+-------------------------------------------------------+
	|PAPI_FSQ_INS   |FSq ins                                                |
	+---------------+-------------------------------------------------------+
	|PAPI_FNV_INS   |Finv ins                                               |
	+---------------+-------------------------------------------------------+
