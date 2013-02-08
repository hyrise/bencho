#include "Printer.h"

Printer::Printer(vector<map<string, int> >* combinations, map<int, string>* graphs, vector<string>* performanceCounters, string* sequenceIdParameter, map<string, map<int, vector<long long> > >* resultX, map<string, map<int, vector<long long> > >* resultY, string* unit) {

    _combinations = combinations;
    _graphs = graphs;
    _performanceCounters = performanceCounters;
    _sequenceIdParameter = sequenceIdParameter;

    _resultX = resultX;
    _resultY = resultY;

    _unit = unit;

}

Printer::Printer(vector<map<string, int> >* combinations) {

    _combinations = combinations;

}

void Printer::printResults() {

    cout << endl << "Results:" << endl << "#############" << endl;

    int graphId, positions;
    map<int, string>::iterator it;

    map<string, map<int, vector<long long> > > resultX = *_resultX;
    vector<string> performanceCounters = *_performanceCounters;
    vector<map<string, int> > combinations = *_combinations;


    for (size_t i = 0; i < _performanceCounters->size(); ++i) {

        for (it = _graphs->begin(); it != _graphs->end(); it++) {

            graphId = it->first;
            vector<long long> graphData;
            positions = resultX[performanceCounters[i]][graphId].size();
            
            for (int pos = 0; pos < positions; ++pos) {

                cout << resultX[performanceCounters[i]][graphId][pos] << " " << *_sequenceIdParameter << " -> " << this->getValue(graphId, performanceCounters[i], pos, combinations[pos]) << " " << *_unit << endl;
            }
        }
    }

}

long long Printer::getValue(size_t graph_id, string perf_ctr, size_t pos, map<string, int> parameters) {

    map<string, map<int, vector<long long> > > resultY = *_resultY;
    return resultY[perf_ctr][graph_id][pos];

}


void Printer::printCombinations() {

    vector<map<string, int> > combinations = *_combinations;

    map<string, int>::iterator it;

    for (size_t i = 0; i < combinations.size(); ++i) {

        for (it = combinations[i].begin(); it != combinations[i].end(); it++) {

            cout << it->first << " - " << it->second << endl;
        }

        cout << "-------" << endl;
    }

}

Printer::~Printer() { 

}