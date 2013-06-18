#include "AbstractBenchmark.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#include "PapiSingleton.h"      //class for performance counters
#include "Parameter.h"          //class for Parameters
#include "Printer.h"            //class for printing final and intermediate results
#include "Aggregator.h"         //class for aggregator-functions
#include "FileWriter.h"         //class to write result files
#include "DirectoryManager.h"   //class to manage directories
#include "AbstractPlotter.h"    //abstract plotter class
#include "PlotterGnuplot.h"     //class for plotting results with gnuplot
#include "PlotterPython.h"      //class for plotting results with python matplotlib
#include "PlotterR.h"           //class for plotting results with R ggplot2
#include "resultFileHelper.h"


////////////////////////////////////// Contructor and Destructor //////////////////////////////////////

AbstractBenchmark::AbstractBenchmark() :
    _max_runs(1)
,   _warm_up_runs(2)
,   _calibration_runs(5)
,   _is_initialized(false)
,   _do_incache_calibration(false)
,   _max_deviation(0)
,   _name("default")
,   _do_papi_manual(false)
,   _current_perf_counter(NULL)
,   _fastMode(false)
,   _silentMode(false)
,   _rawOutput(false)
,   _filewriter(new FileWriter(this))
,   _directorymanager(new DirectoryManager())
{

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.setf(std::ios::showpoint);

#ifdef USE_PAPI_TRACE
    setUnit("CPU Cycles");
#else
    setUnit("Clock Ticks");
#endif

}

AbstractBenchmark::~AbstractBenchmark() {
    delete _filewriter;
    delete _directorymanager;
}


////////////////////////////////////// Methods to initialize Benchmark //////////////////////////////////////

void AbstractBenchmark::init()
{
    if (_is_initialized == false)
    {
        //checkExecutables();
        initialize();
        _is_initialized = true;
    }
}

std::vector<std::map<std::string, int> > &AbstractBenchmark::getCombinations() {
    return _combinations;
}

void AbstractBenchmark::addParameter(Parameter *parameter, std::string version) {
    _parameters[version].push_back(*parameter);
    delete parameter;
}

void AbstractBenchmark::addPerformanceCounter(std::string event_name) {
    _performance_counters.push_back(event_name);
}

void AbstractBenchmark::addTestSeries(int id, std::string name)
{
    _test_series[id] = name;

    for (size_t i = 0; i < _performance_counters.size(); ++i)
    {
        _result_x[_performance_counters[i]][id] = std::vector<long long>();
        _result_y[_performance_counters[i]][id] = std::vector<long long>();
        if (_rawOutput) _result_y_raw[_performance_counters[i]][id] = std::vector<std::vector<long long> >();
    }
}

void AbstractBenchmark::addAllTestSeriesAsGraphs()
{
    std::map<int, std::string>::iterator it;

    for (it = _test_series.begin(); it != _test_series.end(); it++)
    {
        addTestSeriesAsGraph(it->first);
    }
}

void AbstractBenchmark::addTestSeriesAsGraph(int test_series_id)
{
    addGraph(test_series_id, _test_series[test_series_id]);
}

void AbstractBenchmark::addGraph(int id, std::string name)
{
    _graphs[id] = name;
}

////////////////////////////////////// Methods to prepare Benchmark //////////////////////////////////////

void AbstractBenchmark::calcCombinations() {

    std::vector<std::map<std::string, int> > temp;
    std::map<std::string, int> d;
    std::string name;
    std::vector<long long> values;
    std::vector<Parameter> *parameters;

    parameters = &(_parameters[_current_version]);
    
    if (parameters->size() < 1) {
        throw std::runtime_error("No parameters.");
    }

    _combinations.clear();
    _combinations.push_back(std::map<std::string, int>());

    std::vector<Parameter>::iterator it;

    for (it = parameters->begin(); it != parameters->end(); it++) {

        temp.clear();

        std::vector< long long > values = it->getValues();  //get data from parameter-object
        std::string name = it->getName();

        for (size_t j = 0; j < values.size(); ++j) {
            
            for (size_t k = 0; k < _combinations.size(); ++k) {

                d = _combinations[k];
                d[name] = values[j];
                temp.push_back(d);
            }
        }

        _combinations = temp;
    }
}

void AbstractBenchmark::clearCache() {
    
    int sum;
    int cachesize_in_mb = 12;
    if (!_silentMode) std::cout << "Clearing cache. " << cachesize_in_mb << "MB. ";
    int * dummy_array = new int [1024*1024*cachesize_in_mb] ;

    for ( int address = 0; address < 1024*1024*cachesize_in_mb; address++) {
        dummy_array [ address ] = address +1;
    }

    int * dummy_array2 = new int [1024*1024*cachesize_in_mb ] ;
    for ( int address = 0; address < 1024*1024*cachesize_in_mb; address++) {
        dummy_array2 [ address ] = address +1;
    }

    for(int repetition = 0; repetition < 3; repetition++) {
        for ( int address = 0; address < 1024*1024*cachesize_in_mb; address++){ sum += dummy_array[address];
        } 
    }

    if (!_silentMode) std::cout << sum << std::endl;
    delete dummy_array ;
    delete dummy_array2 ;
    
}

void AbstractBenchmark::calibrateInCache() {
    int test_series_id;
    long long result;
    std::vector<long long> results;
    std::map<int, std::string>::iterator it;
    std::vector<Parameter> ::iterator it_par;
    std::string perfctr = "PAPI_TOT_CYC";
    
    if (_test_series.size() < 1) {
        throw std::runtime_error("No test series.");
    }
    
    // make sure in cache parameters are set
    for (it_par = _parameters[_current_version].begin(); it_par != _parameters[_current_version].end(); it_par++) {
        if (_parameters_incache.find(it_par->getName()) == _parameters_incache.end()) {

            std::cout << "Error: Could not find incache parameter " << it_par->getName() << std::endl;
            throw std::runtime_error("Incache parameter not set properly.");
        }
    }
    
    prepareCombination(_parameters_incache, combination_incache);
    
    for (it = _test_series.begin(); it != _test_series.end(); it++) {

        test_series_id = it->first;
        if (!_silentMode) std::cout << "Calibrating test series " << test_series_id << std::endl;
        
        results.clear();
        
        for (size_t run = 0; run<_calibration_runs; ++run) {
            // execute with incache=true
            result = executeRun(_parameters_incache, combination_incache, test_series_id, 0, perfctr, true);
            if (!_silentMode) std::cout << "calibrating result = " << result << std::endl;
            results.push_back(result);
        }
        
        Aggregator *myAggregator = new Aggregator(results);
        long long aggr_val = myAggregator->calculateMedian();
        if (!_silentMode) std::cout << "Final Calibration result for test series " << test_series_id << ": " << aggr_val << std::endl;
        _result_calibration[it->second] = aggr_val;
        delete myAggregator;
    }
    
    finishCombination(_parameters_incache, combination_incache);
}


////////////////////////////////////// Methods to execute Benchmark //////////////////////////////////////


void AbstractBenchmark::execute(int max_runs, double max_deviation) {

    init();
    
    if (_performance_counters.size() == 0)
        throw std::runtime_error("No performance counters added!");    
    
    
    // ask which version of benchark should be run
    if (_parameters.size() > 1) {
       
        std::map<std::string, std::vector<Parameter> >::iterator it_par;   //iterator for Parameter-Map
        std::cout << "Multiple versions of benchmark available:" << std::endl;
        size_t i = 1;
        std::vector<std::string> choice;
        for (it_par = _parameters.begin(); it_par != _parameters.end(); it_par++)
        {
            std::cout << "\t" << i++ << ": " << it_par->first << std::endl; //print out versions and save options in vector
            choice.push_back(it_par->first);
        }
        
        std::string input;       //user selects version
        std::cin >> input;
        
        std::stringstream sstr(input);
        size_t number; 
        sstr >> number;
        
        _current_version = choice[number-1];        
        if (!_silentMode) std::cout << "Running version " << _current_version << std::endl;
        if (!_silentMode) std::cout << "Setting sequence id to " << _current_version << std::endl;
        _sequence_id_parameter = _current_version;
    
    } else {
        
        _current_version = "first";
        if (!_silentMode) std::cout << "Running default version of benchmark." << std::endl;
    
    }
    
    if (!_silentMode) std::cout << std::endl;
    if (!_silentMode) std::cout << "###################################" << std::endl;
    if (!_silentMode) std::cout << "Running Benchmark: " << _name << std::endl;
    if (!_silentMode) std::cout << "###################################" << std::endl;
    if (!_silentMode) std::cout << std::endl;


    // Displaying used perf.-counters and missing ones (relating to GNUplot script) //

    displayHeader();

    //


    if (!_silentMode) std::cout << "Preparing." << std::endl;
    prepareStart();

    calcCombinations();

    if (max_runs >= 1) {

        _max_runs = max_runs;
    }

    if (max_deviation > 0) {

        _max_deviation = max_deviation;
    }
    
    if (_do_incache_calibration) {

        calibrateInCache(); // calibrate in cache
    }

    for (size_t i = 0; i < _combinations.size(); ++i) {

        if (!_silentMode) std::cout << std::endl;
        if (!_silentMode) std::cout << "###################################" << std::endl;
        if (!_silentMode) std::cout << std::setprecision(2) << (float)(i * 100) / _combinations.size() << " percent done." << std::endl;
        if (!_silentMode) std::cout << "###################################" << std::endl;
        if (!_silentMode) std::cout << std::endl;


        if (!_silentMode) std::cout << "Preparing combination " << i << "..." << std::endl;

        prepareCombination(_combinations[i], i);

        executeCombination(_combinations[i], i);

        if (!_silentMode) std::cout << std::endl;
        if (!_silentMode) std::cout << "###################################" << std::endl;
        if (!_silentMode) std::cout << std::setprecision(2) << (float)(i * 100) / _combinations.size() << " percent done." << std::endl;
        if (!_silentMode) std::cout << "###################################" << std::endl;
        if (!_silentMode) std::cout << std::endl;


        if (!_silentMode) std::cout << "Finish combination " << i << "...";

        finishCombination(_combinations[i], i);

        if (!_silentMode) std::cout << std::endl;
    }

    finalize();
}

void AbstractBenchmark::displayHeader()
{
    std::vector<std::string> headers = _filewriter->getHeaders(this);

    std::string gnuplotscript = "./benchmarks/" + this->getName() + ".gp";

    std::ifstream fileexists;
    fileexists.open(gnuplotscript.c_str());
    if(fileexists)
    {
        fileexists.close();
    } else {
        std::cout << "No Gnuplot-Script found." << std::endl;
        return;
    }

    std::vector<std::string> counters;
    std::string linebuffer;
    std::string begin = "§@";
    std::string end = "@§";
    std::fstream stream_in(gnuplotscript.c_str(), std::ios::in);
    while(!stream_in.eof())
    {
        getline(stream_in, linebuffer);
        int spot_begin = linebuffer.find(begin);
        int spot_end = linebuffer.find(end);
        if(spot_begin >= 0 && spot_end >= 0)
        {
            std::string tmpstring = linebuffer.substr(spot_begin+begin.length(),spot_end-(spot_begin+begin.length()));
            counters.push_back(tmpstring);
        }
    }
    stream_in.close();

    
    int pos;
    if (!_fastMode) 
    {
        std::cout << std::endl << "Using PerformanceCounters:" << std::endl;
        for (int i = 0; i < counters.size(); ++i)
        {
            for (int j = 0; j < headers.size(); ++j)
            {
                if (counters.at(i) == headers.at(j))
                    std::cout << "   " << counters.at(i) << std::endl;
            }
        }
    }

    bool counters_missing = false;

    if (!_fastMode) 
    {
        std::cout << std::endl << std::endl << "Missing PerformanceCounters (Gnuplot):" << std::endl;
        for (int i = 0; i < counters.size(); ++i)
        {
            bool not_found = true;
            for (int j = 0; j < headers.size(); ++j)
            {
                if (counters.at(i) == headers.at(j))
                    not_found = false;
            }
            if(not_found){
                std::cout << "   " << counters.at(i) << std::endl;
                counters_missing = true;
            }
        }
    }

    if (!_fastMode && counters_missing)
    {
        std::cout << std::endl << "Do you wish to continue?" << std::endl << "(Press ENTER to continue, type 0 to abort)" << std::endl;
        std::string userinput;
        getline(std::cin, userinput);
        if(!userinput.empty())
        {
            if(userinput == "0")
            {
                std::cout << std::endl << "Process terminated." << std::endl;
                exit(1);
            }
        }
    }
}

void AbstractBenchmark::executeCombination(std::map<std::string, int> parameters, int combination) {

    int test_series_id;
    long long result;
    std::vector<long long> results;
    std::map<int, std::string>::iterator it;

    if (_test_series.size() < 1) {

        throw std::runtime_error("No test series.");
    }

    for (it = _test_series.begin(); it != _test_series.end(); it++) {

        test_series_id = it->first;

        if (!_silentMode) {
            std::cout << std::endl;
            std::cout << "###################################" << std::endl;
            std::cout << std::setprecision(2) << (float)(combination * 100) / _combinations.size() << " percent done." << std::endl;
            std::cout << "###################################" << std::endl;
            std::cout << std::endl;
        }

        for (size_t perf_ctr = 0; perf_ctr < _performance_counters.size(); ++perf_ctr) {

            std::string perf = _performance_counters[perf_ctr];
            
            if (!_silentMode) {
                std::cout << "Measuring " << perf << ". " << std::endl;
                std::cout << "Test Series: " << it->second << ". " << std::endl;
            }

            // print parameters
            if (!_silentMode) std::cout << "Parameters: " << std::endl;
            std::map<std::string, int>::iterator parameter_it;
            for (parameter_it = parameters.begin(); parameter_it != parameters.end(); parameter_it++)
                if (!_silentMode) std::cout << "\t" << parameter_it->first << ": " << parameter_it->second << std::endl;
                
                
            if (!_silentMode) std::cout << "Warm up (" << _warm_up_runs << "): " << std::flush;

            // do the warum up
            for (size_t run = 0; run < _warm_up_runs; ++run) {

                executeRun(parameters, combination, test_series_id, -1, _performance_counters[0]);
                if (!_silentMode) std::cout << run + 1 << "." << std::flush;
            }

            if (_warm_up_runs > 0) {

                if (!_silentMode) std::cout << " " << std::flush;
            }

            if (!_silentMode) std::cout << std::endl;

            size_t max_runs = _max_runs;

            if (_fastMode) max_runs = 1;

            double current_deviation = _max_deviation;
            results.clear();

            if (_max_deviation > 0) {

                std::cout << "Run: " << std::flush;

                while (current_deviation >= _max_deviation) {

                    for (size_t run = 0; (run < max_runs) && (current_deviation >= _max_deviation); ++run) {

                        result = executeRun(parameters, combination, test_series_id, run, perf);
                        results.push_back(result);
                        Aggregator *resultAggregator = new Aggregator(results);
                        current_deviation = resultAggregator->calculateDeviation() / resultAggregator->calculateAverage();
                        delete resultAggregator;
                        std::cout << "+++ Run: " << run << "\tResult: " << results[run] << "\tDeviation: " << current_deviation * 100 << "%" << std::endl;
                    }

                    std::cout << " ";

                    if (current_deviation >= _max_deviation) {

                        max_runs = max_runs * 2;
                        std::cout << "Incrementing max runs to " << max_runs << std::endl;
                        results.clear();
                        std::cout << "Cleared results, starting over..." << std::endl;
                    
                    } else {

                        std::cout << "Deviation is " << current_deviation * 100 << "%. Continue." << std::endl;
                    }
                }
            
            } else {
                
                for (size_t run = 0; run < max_runs; ++run) {

                    result = executeRun(parameters, combination, test_series_id, run, perf);
                    results.push_back(result);
                }
            }

            _result_x[perf][test_series_id].push_back(getSequenceId(parameters, test_series_id));
            
            if (!_fastMode) {
                Aggregator *resultAggregator = new Aggregator(results);
                long long aggr_val = resultAggregator->calculateFunction(getAggregatingFunction());
                long long aggr_err = resultAggregator->calculateDeviation();
                _result_y[perf][test_series_id].push_back(aggr_val);
                _result_error[perf][test_series_id].push_back(aggr_err);
                if (_rawOutput) _result_y_raw[perf][test_series_id].push_back(results);
                delete resultAggregator;
                if (!_silentMode) std::cout << "Final Result: " << aggr_val << std::endl;
                if (!_silentMode) std::cout << "Final Error: " << aggr_err << std::endl;
                if (!_silentMode) std::cout << "Error/Result: " << (double)aggr_err/aggr_val << std::endl;
            } else {
                _result_y[perf][test_series_id].push_back(result);
                _result_error[perf][test_series_id].push_back(0);
            }
        }
    }
}

long long AbstractBenchmark::executeRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run, std::string perf_ctr, bool incache)
{
    long long result;

    if (incache) {

        // execute to load everything in memory
        if (!_silentMode) std::cout << "Dry run..." << std::endl;
        prepareRun(parameters, combination, test_series_id, run);
        prepareRunAfterCacheCleared(parameters, combination, test_series_id, run);
        doTheTest(parameters, combination, test_series_id, run);
        
        // execute 10 times
        PapiSingleton::getInstance().init(perf_ctr.c_str());
        PapiSingleton::getInstance().start();
        
        for (size_t i=0; i<10; ++i) {

            prepareRun(parameters, combination, test_series_id, run);
            prepareRunAfterCacheCleared(parameters, combination, test_series_id, run);
            doTheTest(parameters, combination, test_series_id, run);
        
        }
        
        result = PapiSingleton::getInstance().stop() / 10;
        if (!_silentMode) std::cout << "result: " << result << std::endl;
        finishRun(parameters, combination, test_series_id, run);
    
    } else {

        // no calibration, so just run once
        prepareRun(parameters, combination, test_series_id, run);
        clearCache();
        prepareRunAfterCacheCleared(parameters, combination, test_series_id, run);
        
        if (!_do_papi_manual) {
        
            PapiSingleton::getInstance().init(perf_ctr.c_str());
            PapiSingleton::getInstance().start();
        
        } else {

           _current_perf_counter = perf_ctr.c_str();
        }
        
        doTheTest(parameters, combination, test_series_id, run);
        
        if (!_do_papi_manual)
            result = PapiSingleton::getInstance().stop();
        else
            result = _manual_papi_result;
        
        if (!_silentMode) std::cout << "result: " << result << std::endl;
        finishRun(parameters, combination, test_series_id, run);
    }
    
    return result;
}

int AbstractBenchmark::full(int max_runs, double max_deviation) {

    execute(max_runs, max_deviation);

    _filewriter->dumpResult(this);
    _filewriter->saveParameters(this);

    plotResults();

    return 0;
}


////////////////////////////////////// Methods for result output //////////////////////////////////////


FileWriter *AbstractBenchmark::getFileWriter() {
    return _filewriter;
}

DirectoryManager *AbstractBenchmark::getDirectoryManager() {
    return _directorymanager;
}

void AbstractBenchmark::printResults() {

    Printer* resultPrinter = new Printer(&_combinations, &_graphs, &_performance_counters, &_sequence_id_parameter, &_result_x, &_result_y, &_unit);
    resultPrinter->printResults();
    delete resultPrinter;

}

void AbstractBenchmark::printCombinations() {

    Printer* combinationsPrinter = new Printer(&_combinations);
    combinationsPrinter->printCombinations();
    delete combinationsPrinter;

}

void AbstractBenchmark::plotResults(bool isDefault) {
    AbstractPlotter *settingsPlotter = new AbstractPlotter();
    settingsPlotter->setUp(isDefault);

    callPlotterWithSettings(settingsPlotter);

    settingsPlotter->pdfcropResult();
    delete settingsPlotter;
}

void AbstractBenchmark::callPlotterWithSettings(AbstractPlotter *settingsPlotter)
{
    #ifdef GNUPLOT
    std::cout << std::endl << "Plotting results with Gnuplot" << std::endl;
    AbstractPlotter *plotterGnuplot = new PlotterGnuplot();
    callSpecificPlotter(plotterGnuplot, settingsPlotter, ".gp");
    delete plotterGnuplot;
    #endif

    #ifdef PYPLOT
    std::cout << std::endl << "Plotting results with python matplotlib" << std::endl;
    AbstractPlotter *plotterPython = new PlotterPython();
    callSpecificPlotter(plotterPython, settingsPlotter, ".py");
    delete plotterPython;
    #endif
    
    #ifdef RPLOT
    std::cout << std::endl << "Plotting results with R ggplot2" << std::endl;
    AbstractPlotter *plotterR = new PlotterR();
    callSpecificPlotter(plotterR, settingsPlotter, ".r");
    delete plotterR;
    #endif
}

void AbstractBenchmark::callSpecificPlotter(AbstractPlotter *specificPlotter, AbstractPlotter *settingsPlotter, std::string fileEnding)
{
    std::string systemScript = settingsPlotter->getSystemScriptDir() + "/system" + fileEnding;
    std::string plotterScript = settingsPlotter->getPlotterScriptDir() + "/" + settingsPlotter->getBenchName() + fileEnding;

    if (!fileExists(systemScript))
    {
        std::cerr << "No plotter base script found (in " << settingsPlotter->getSystemScriptDir() << "). Plotting cancelled!" << std::endl;
        return;
    }
    if (!fileExists(plotterScript))
    {
        std::cerr << "No plotter script found (in " << settingsPlotter->getPlotterScriptDir() << "). Plotting cancelled!" << std::endl;
        return;
    }

    specificPlotter->setUp(settingsPlotter->getResultDir(), settingsPlotter->getPlotterScriptDir(), settingsPlotter->getSystemScriptDir(), plotterScript, systemScript, settingsPlotter->getBenchName(), settingsPlotter->getBenchId());
    specificPlotter->plot();
}


////////////////////////////////////// Setters for Options //////////////////////////////////////

void AbstractBenchmark::setName(std::string name) {
    _name = name;
}

void AbstractBenchmark::setSequenceId(std::string id) {
    _sequence_id_parameter = id;
}

void AbstractBenchmark::setUnit(std::string unit) {
    _unit = unit;
}

void AbstractBenchmark::setMaxRuns(size_t max_runs) {
    _max_runs = max_runs;
}

void AbstractBenchmark::setMaxDeviation(double max_deviation) {
    _max_deviation = max_deviation;
}

void AbstractBenchmark::setWarmUpRuns(size_t warm_up_runs) {
    _warm_up_runs = warm_up_runs;
}

void AbstractBenchmark::setFastMode(bool fastMode) {
    _fastMode = fastMode;
    _silentMode = fastMode;
}

void AbstractBenchmark::setSilentMode(bool silentMode) {
    _silentMode = silentMode;
}

void AbstractBenchmark::setRawDataOutput(bool rawOutput) {
    _rawOutput = rawOutput;
}

void AbstractBenchmark::setAggregatingFunction(AggregationType::Function function) {
    _aggregatingFunction = function;
}


////////////////////////////////////// Getters for Options //////////////////////////////////////

std::string AbstractBenchmark::getName() {
    return _name;
}

std::string AbstractBenchmark::getUnit() {
    return _unit;
}

size_t AbstractBenchmark::getMaxRuns() {
    return _max_runs;
}

size_t AbstractBenchmark::getWarmUpRuns() {
    return _warm_up_runs;
}

int AbstractBenchmark::getSequenceId(std::map<std::string, int> parameters, int test_series_id) const {
   
    if (_sequence_id_parameter == "") {
        
        throw std::runtime_error("Either set _sequence_id_parameter or override getSequenceId method.");
    }

    return parameters[_sequence_id_parameter];
}

bool AbstractBenchmark::getFastMode() {
    return _fastMode;
}

bool AbstractBenchmark::getSilentMode() {
    return _silentMode;
}

bool AbstractBenchmark::getRawDataOutput() {
    return _rawOutput;
}

AggregationType::Function AbstractBenchmark::getAggregatingFunction() {

    if (!(_aggregatingFunction < AggregationType::numTypes)) {
        
        throw std::runtime_error("Please specify aggregating function.");
    }

    return _aggregatingFunction;
}


////////////////////////////////////// Getters for actual Version/Values //////////////////////////////////////

std::string AbstractBenchmark::getCurrentVersion()
{
    return _current_version;
}

std::vector<Parameter> *AbstractBenchmark::getParameters()
{
    return &(_parameters[_current_version]);
}

size_t AbstractBenchmark::getRowCount()
{
    return _result_y[_performance_counters[0]][0].size();
}

std::map<int, std::string> &AbstractBenchmark::getTestSeries()
{
    return _test_series;
}

std::vector<std::string> &AbstractBenchmark::getPerformanceCounters()
{
    return _performance_counters;
}

std::vector<long long> &AbstractBenchmark::getResult_x(size_t test_series, std::string performance_counter)
{
    return _result_x[performance_counter][test_series];
}

std::vector<long long> &AbstractBenchmark::getResult_y(size_t test_series, std::string performance_counter)
{
    return _result_y[performance_counter][test_series];
}

std::vector<std::vector<long long> > &AbstractBenchmark::getResult_y_raw(size_t test_series, std::string performance_counter)
{
    return _result_y_raw[performance_counter][test_series];
}

std::vector<long long> &AbstractBenchmark::getResult_error(size_t test_series, std::string performance_counter)
{
    return _result_error[performance_counter][test_series];
}

long long AbstractBenchmark::getResult_incache(std::string test_series)
{
    return _result_calibration[test_series];
}

long long AbstractBenchmark::getValue(size_t graph_id, std::string perf_ctr, size_t pos, std::map<std::string, int> parameters)
{
    return _result_y[perf_ctr][graph_id][pos];
}


////////////////////////////////////// Methods implememnted by actual benchmark //////////////////////////////////////

void AbstractBenchmark::prepareRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

void AbstractBenchmark::prepareRunAfterCacheCleared(std::map<std::string, int> parameters, int combination, int run, int test_series_id)
{
    // filled in actual benchmark
}

void AbstractBenchmark::prepareCombination(std::map<std::string, int> parameters, int combination) {
    // filled in actual benchmark
}

void AbstractBenchmark::finishRun(std::map<std::string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

void AbstractBenchmark::finishCombination(std::map<std::string, int> parameters, int combination) {
    // filled in actual benchmark
}

void AbstractBenchmark::doTheTest(std::map<std::string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

