#include "AbstractBenchmark.h"


////////////////////////////////////// Contructor and Destructor //////////////////////////////////////

AbstractBenchmark::AbstractBenchmark() :
    _max_runs(0),
    _warm_up_runs(2),
    _calibration_runs(5),
    _is_initialized(false),
    _do_incache_calibration(false),
    _max_deviation(0.05),
	_name("default"),
    _do_papi_manual(false),
    _current_perf_counter(NULL),
    _fastMode(false),
    _silentMode(false)
{

    _filewriter = new FileWriter(this);
    cout.setf(ios::fixed, ios::floatfield);
    cout.setf(ios::showpoint);

#ifdef USE_PAPI_TRACE
    setUnit("CPU Cycles");
#else
    setUnit("Clock Ticks");
#endif

}

AbstractBenchmark::~AbstractBenchmark() {
    delete _filewriter;
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

vector<map<string, int> > &AbstractBenchmark::getCombinations() {
    return _combinations;
}

void AbstractBenchmark::addParameter(Parameter *parameter, string version) {
    _parameters[version].push_back(*parameter);
    delete parameter;
}

void AbstractBenchmark::addPerformanceCounter(string event_name) {
    _performance_counters.push_back(event_name);
}

void AbstractBenchmark::addTestSeries(int id, string name)
{
    _test_series[id] = name;

    for (size_t i = 0; i < _performance_counters.size(); ++i)
    {
        _result_x[_performance_counters[i]][id] = vector<long long>();
        _result_y[_performance_counters[i]][id] = vector<long long>();
    }
}

void AbstractBenchmark::addAllTestSeriesAsGraphs()
{
    map<int, string>::iterator it;

    for (it = _test_series.begin(); it != _test_series.end(); it++)
    {
        addTestSeriesAsGraph(it->first);
    }
}

void AbstractBenchmark::addTestSeriesAsGraph(int test_series_id)
{
    addGraph(test_series_id, _test_series[test_series_id]);
}

void AbstractBenchmark::addGraph(int id, string name)
{
    _graphs[id] = name;
}

////////////////////////////////////// Methods to prepare Benchmark //////////////////////////////////////

void AbstractBenchmark::calcCombinations() {

    vector<map<string, int> > temp;
    map<string, int> d;
    string name;
    vector<long long> values;
    vector<Parameter> *parameters;

    parameters = &(_parameters[_current_version]);
    
    if (parameters->size() < 1) {
        throw std::runtime_error("No parameters.");
    }

    _combinations.clear();
    _combinations.push_back(map<string, int>());

    vector<Parameter>::iterator it;

    for (it = parameters->begin(); it != parameters->end(); it++) {

        temp.clear();

        vector< long long > values = it->getValues();  //get data from parameter-object
        string name = it->getName();

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
    if (!_silentMode) cout << "Clearing cache. " << cachesize_in_mb << "MB. ";
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

    if (!_silentMode) cout << sum << endl;
    delete dummy_array ;
    delete dummy_array2 ;
    
}

void AbstractBenchmark::calibrateInCache() {
    int test_series_id;
    long long result;
    vector<long long> results;
    map<int, string>::iterator it;
    vector<Parameter> ::iterator it_par;
    string perfctr = "PAPI_TOT_CYC";
    
    if (_test_series.size() < 1) {
        throw std::runtime_error("No test series.");
    }
    
    // make sure in cache parameters are set
    for (it_par = _parameters[_current_version].begin(); it_par != _parameters[_current_version].end(); it_par++) {
        if (_parameters_incache.find(it_par->getName()) == _parameters_incache.end()) {

            cout << "Error: Could not find incache parameter " << it_par->getName() << endl;
            throw std::runtime_error("Incache parameter not set properly.");
        }
    }
    
    prepareCombination(_parameters_incache, combination_incache);
    
    for (it = _test_series.begin(); it != _test_series.end(); it++) {

        test_series_id = it->first;
        if (!_silentMode) cout << "Calibrating test series " << test_series_id << endl;
        
        results.clear();
        
        for (size_t run = 0; run<_calibration_runs; ++run) {
            // execute with incache=true
            result = executeRun(_parameters_incache, combination_incache, test_series_id, 0, perfctr, true);
            if (!_silentMode) cout << "calibrating result = " << result << endl;
            results.push_back(result);
        }
        
        Aggregator *myAggregator = new Aggregator(results);
        long long aggr_val = myAggregator->calculateMedian();
        if (!_silentMode) cout << "Final Calibration result for test series " << test_series_id << ": " << aggr_val << endl;
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
       
        map<string, vector<Parameter> >::iterator it_par;   //iterator for Parameter-Map
        cout << "Multiple versions of benchmark available:" << endl;
        size_t i = 1;
        vector<string> choice;
        for (it_par = _parameters.begin(); it_par != _parameters.end(); it_par++)
        {
            cout << "\t" << i++ << ": " << it_par->first << endl; //print out versions and save options in vector
            choice.push_back(it_par->first);
        }
        
        string input;       //user selects version
        cin >> input;
        
        std::stringstream sstr(input);
        size_t number; 
        sstr >> number;
        
        _current_version = choice[number-1];        
        if (!_silentMode) cout << "Running version " << _current_version << endl;
        if (!_silentMode) cout << "Setting sequence id to " << _current_version << endl;
        _sequence_id_parameter = _current_version;
    
    } else {
        
        _current_version = "first";
        if (!_silentMode) cout << "Running default version of benchmark." << endl;
    
    }
    
    if (!_silentMode) cout << endl;
    if (!_silentMode) cout << "###################################" << endl;
    if (!_silentMode) cout << "Running Benchmark: " << _name << endl;
    if (!_silentMode) cout << "###################################" << endl;
    if (!_silentMode) cout << endl;


    // Displaying used perf.-counters and missing ones (relating to GNUplot script) //

    displayHeader();

    //


    if (!_silentMode) cout << "Preparing." << endl;
    prepareStart();

    calcCombinations();

    if (max_runs >= 0) {

        _max_runs = max_runs;
    }

    if (max_deviation > 0) {

        _max_deviation = max_deviation;
    }
    
    if (_do_incache_calibration) {

        calibrateInCache(); // calibrate in cache
    }

    for (size_t i = 0; i < _combinations.size(); ++i) {

        if (!_silentMode) cout << endl;
        if (!_silentMode) cout << "###################################" << endl;
        if (!_silentMode) cout << setprecision(2) << (float)(i * 100) / _combinations.size() << " percent done." << endl;
        if (!_silentMode) cout << "###################################" << endl;
        if (!_silentMode) cout << endl;


        if (!_silentMode) cout << "Preparing combination " << i << "..." << endl;

        prepareCombination(_combinations[i], i);

        executeCombination(_combinations[i], i);

        if (!_silentMode) cout << endl;
        if (!_silentMode) cout << "###################################" << endl;
        if (!_silentMode) cout << setprecision(2) << (float)(i * 100) / _combinations.size() << " percent done." << endl;
        if (!_silentMode) cout << "###################################" << endl;
        if (!_silentMode) cout << endl;


        if (!_silentMode) cout << "Finish combination " << i << "...";

        finishCombination(_combinations[i], i);

        if (!_silentMode) cout << endl;
    }

    finalize();
}

void AbstractBenchmark::displayHeader()
{
    vector<string> headers = _filewriter->getHeaders(this);

    string gnuplotscript = "./benchmarks/" + this->getName() + ".gp";

    ifstream fileexists;
    fileexists.open(gnuplotscript.c_str());
    if(fileexists)
    {
        fileexists.close();
    } else {
        cout << "No Gnuplot-Script found." << endl;
        return;
    }

    vector<string> counters;
    string linebuffer;
    string begin = "§@";
    string end = "@§";
    fstream stream_in(gnuplotscript.c_str(), ios::in);
    while(!stream_in.eof())
    {
        getline(stream_in, linebuffer);
        int spot_begin = linebuffer.find(begin);
        int spot_end = linebuffer.find(end);
        if(spot_begin >= 0 && spot_end >= 0)
        {
            string tmpstring = linebuffer.substr(spot_begin+begin.length(),spot_end-(spot_begin+begin.length()));
            counters.push_back(tmpstring);
        }
    }
    stream_in.close();

    
    int pos;
    if (!_fastMode) 
    {
        cout << endl << "Using PerformanceCounters:" << endl;
        for (int i = 0; i < counters.size(); ++i)
        {
            for (int j = 0; j < headers.size(); ++j)
            {
                if (counters.at(i) == headers.at(j))
                    cout << "   " << counters.at(i) << endl;
            }
        }
    }

    if (!_fastMode) 
    {
        cout << endl << endl << "Missing PerformanceCounters (Gnuplot):" << endl;
        for (int i = 0; i < counters.size(); ++i)
        {
            bool not_found = true;
            for (int j = 0; j < headers.size(); ++j)
            {
                if (counters.at(i) == headers.at(j))
                    not_found = false;
            }
            if(not_found)
                cout << "   " << counters.at(i) << endl;
        }
    }

    if (!_fastMode)
    {
        cout << endl << "Do you wish to continue?" << endl << "(Press ENTER to continue, type 0 to abort)" << endl;
        string userinput;
        getline(cin, userinput);
        if(!userinput.empty())
        {
            if(userinput == "0")
            {
                cout << endl << "Process terminated." << endl;
                exit(1);
            }
        }
    }
}

void AbstractBenchmark::executeCombination(map<string, int> parameters, int combination) {

    int test_series_id;
    long long result;
    vector<long long> results;
    map<int, string>::iterator it;

    if (_test_series.size() < 1) {

        throw std::runtime_error("No test series.");
    }

    for (it = _test_series.begin(); it != _test_series.end(); it++) {

        test_series_id = it->first;

        if (!_silentMode) {
            cout << endl;
            cout << "###################################" << endl;
            cout << setprecision(2) << (float)(combination * 100) / _combinations.size() << " percent done." << endl;
            cout << "###################################" << endl;
            cout << endl;
        }

        for (size_t perf_ctr = 0; perf_ctr < _performance_counters.size(); ++perf_ctr) {

            string perf = _performance_counters[perf_ctr];
            
            if (!_silentMode) {
                cout << "Measuring " << perf << ". " << endl;
                cout << "Test Series: " << it->second << ". " << endl;
            }

            // print parameters
            if (!_silentMode) cout << "Parameters: " << endl;
            map<string, int>::iterator parameter_it;
            for (parameter_it = parameters.begin(); parameter_it != parameters.end(); parameter_it++)
                if (!_silentMode) cout << "\t" << parameter_it->first << ": " << parameter_it->second << endl;
                
                
            if (!_silentMode) cout << "Warm up (" << _warm_up_runs << "): " << flush;

            // do the warum up
            for (size_t run = 0; run < _warm_up_runs; ++run) {

                executeRun(parameters, combination, test_series_id, -1, _performance_counters[0]);
                if (!_silentMode) cout << run + 1 << "." << flush;
            }

            if (_warm_up_runs > 0) {

                if (!_silentMode) cout << " " << flush;
            }

            if (!_silentMode) cout << endl;

            size_t max_runs = _max_runs;

            if (_fastMode) max_runs = 1;

            double current_deviation = _max_deviation;
            results.clear();

            if (max_runs == 0) {

                max_runs = 10;
                cout << "Run: " << flush;

                while (current_deviation >= _max_deviation) {

                    for (size_t run = 0; (run < max_runs) && (current_deviation >= _max_deviation); ++run) {

                        result = executeRun(parameters, combination, test_series_id, run, perf);
                        results.push_back(result);
                        Aggregator *resultAggregator = new Aggregator(results);
                        current_deviation = resultAggregator->calculateDeviation() / resultAggregator->calculateAverage();
                        delete resultAggregator;
                        cout << "+++ Run: " << run << "\tResult: " << results[run] << "\tDeviation: " << current_deviation * 100 << "%" << endl;
                    }

                    cout << " ";

                    if (current_deviation >= _max_deviation) {

                        max_runs = max_runs * 2;
                        cout << "Incrementing max runs to " << max_runs << endl;
                        results.clear();
                        cout << "Cleared results, starting over..." << endl;
                    
                    } else {

                        cout << "Deviation is " << current_deviation * 100 << "%. Continue." << endl;
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
                long long aggr_val = resultAggregator->calculateFunction(getAggregatingFunction());      //same funktions as in old implementation
                long long aggr_err = resultAggregator->calculateDeviation();
                _result_y[perf][test_series_id].push_back(aggr_val);
                _result_error[perf][test_series_id].push_back(aggr_err);
                delete resultAggregator;
                if (!_silentMode) cout << "Final Result: " << aggr_val << endl;
                if (!_silentMode) cout << "Final Error: " << aggr_err << endl;
                if (!_silentMode) cout << "Error/Result: " << (double)aggr_err/aggr_val << endl;
            } else {
                _result_y[perf][test_series_id].push_back(result);
                _result_error[perf][test_series_id].push_back(0);
            }
        }
    }
}

long long AbstractBenchmark::executeRun(map<string, int> parameters, int combination, int test_series_id, int run, string perf_ctr, bool incache)
{
    long long result;

    if (incache) {

        // execute to load everything in memory
        if (!_silentMode) cout << "Dry run..." << endl;
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
        if (!_silentMode) cout << "result: " << result << endl;
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
        
        if (!_silentMode) cout << "result: " << result << endl;
        finishRun(parameters, combination, test_series_id, run);
    }
    
    return result;
}

int AbstractBenchmark::full(int max_runs, double max_deviation) {

    execute(max_runs, max_deviation);

    _filewriter->dumpResult(this);
    _filewriter->saveParameters(this);

    plotResultsWithGnuplot();

    return 0;
}


////////////////////////////////////// Methods for result output //////////////////////////////////////


FileWriter *AbstractBenchmark::getFileWriter() {
    return _filewriter;
}

DirectoryManager *AbstractBenchmark::getDirectoryManager() {
    return &_directorymanager;
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

void AbstractBenchmark::plotResultsWithGnuplot() {
    Gnup* plotGnuplot = new Gnup();
    plotGnuplot->plot();
    delete plotGnuplot;
}


////////////////////////////////////// Setters for Options //////////////////////////////////////

void AbstractBenchmark::setName(string name) {
    _name = name;
}

void AbstractBenchmark::setSequenceId(string id) {
    _sequence_id_parameter = id;
}

void AbstractBenchmark::setUnit(string unit) {
    _unit = unit;
}

void AbstractBenchmark::setMaxRuns(size_t max_runs) {
    _max_runs = max_runs;
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

void AbstractBenchmark::setAggregatingFunction(AggregationType::Function function) {
    _aggregatingFunction = function;
}


////////////////////////////////////// Getters for Options //////////////////////////////////////

string AbstractBenchmark::getName() {
    return _name;
}

string AbstractBenchmark::getUnit() {
    return _unit;
}

size_t AbstractBenchmark::getMaxRuns() {
    return _max_runs;
}

size_t AbstractBenchmark::getWarmUpRuns() {
    return _warm_up_runs;
}

int AbstractBenchmark::getSequenceId(map<string, int> parameters, int test_series_id) const {
   
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

AggregationType::Function AbstractBenchmark::getAggregatingFunction() {

    if (!(_aggregatingFunction < AggregationType::numTypes)) {
        
        throw std::runtime_error("Please specify aggregating function.");
    }

    return _aggregatingFunction;
}


////////////////////////////////////// Getters for actual Version/Values //////////////////////////////////////

string AbstractBenchmark::getCurrentVersion()
{
    return _current_version;
}

vector<Parameter> *AbstractBenchmark::getParameters()
{
    return &(_parameters[_current_version]);
}

size_t AbstractBenchmark::getRowCount()
{
    return _result_y[_performance_counters[0]][0].size();
}

map<int, string> &AbstractBenchmark::getTestSeries()
{
    return _test_series;
}

vector<string> &AbstractBenchmark::getPerformanceCounters()
{
    return _performance_counters;
}

vector<long long> &AbstractBenchmark::getResult_x(size_t test_series, string perf_ctr)
{
    return _result_x[perf_ctr][test_series];
}

vector<long long> &AbstractBenchmark::getResult_y(size_t test_series, string perf_ctr)
{
    return _result_y[perf_ctr][test_series];
}

vector<long long> &AbstractBenchmark::getResult_error(size_t test_series, string perf_ctr)
{
    return _result_error[perf_ctr][test_series];
}

long long AbstractBenchmark::getResult_incache(string test_series)
{
    return _result_calibration[test_series];
}

long long AbstractBenchmark::getValue(size_t graph_id, string perf_ctr, size_t pos, map<string, int> parameters)
{
    return _result_y[perf_ctr][graph_id][pos];
}


////////////////////////////////////// Methods implememnted by actual benchmark //////////////////////////////////////

void AbstractBenchmark::prepareRun(map<string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

void AbstractBenchmark::prepareRunAfterCacheCleared(map<string, int> parameters, int combination, int run, int test_series_id)
{
    // filled in actual benchmark
}

void AbstractBenchmark::prepareCombination(map<string, int> parameters, int combination) {
    // filled in actual benchmark
}

void AbstractBenchmark::finishRun(map<string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

void AbstractBenchmark::finishCombination(map<string, int> parameters, int combination) {
    // filled in actual benchmark
}

void AbstractBenchmark::doTheTest(map<string, int> parameters, int combination, int test_series_id, int run) {
    // filled in actual benchmark
}

