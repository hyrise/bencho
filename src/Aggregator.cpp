#include "Aggregator.h"

Aggregator::Aggregator(vector<long long> &data):
    _data(data)
{

}

Aggregator::~Aggregator() { 

}

void Aggregator::setData(vector<long long> &data) {
    _data = data;
}

//Aggretator-Functions:

long long Aggregator::calculateAverage() {

    long long sum = 0;

    for (size_t i = 0; i < _data.size(); ++i) {
        sum += _data[i];
    }

    return sum / _data.size();
}

long long Aggregator::calculateMedian() {

    vector<long long> vector_temp(_data.size());
    copy(_data.begin(), _data.end(), vector_temp.begin());
    sort(vector_temp.begin(), vector_temp.end());

    if (vector_temp.size() % 2 == 0) {
    
        return (long long)(vector_temp[vector_temp.size()/2] + _data[(_data.size()/2)-1]) / (long long)2;
    
    } else {

        return (long long)vector_temp[(vector_temp.size()-1)/2];
    
    }
}

long long Aggregator::calculateMin() {

    return *(std::min_element( _data.begin(), _data.end() ));
}

long long Aggregator::calculateMax() {

    return *(std::max_element(_data.begin(),_data.end() ));
}

long long Aggregator::calculateDeviation() {

    if (_data.size() <= 1)
        return 0;
    
    long long mittelwert = this->calculateAverage();
    double deviation = 0;

    long long sum = 0;

    for (size_t i = 0; i < _data.size(); ++i) {

        sum += pow(_data[i] - mittelwert, 2);

    }

    deviation = sqrt(sum / (_data.size() - 1));

    return (long long)deviation;
}

long long Aggregator::calculateFunction(AggregationType::Function function) {

    switch (function) {
        case AggregationType::Average:
            return this->calculateAverage();
            break;
        case AggregationType::Median:
            return this->calculateMedian();
            break;
        case AggregationType::Min: 
            return this->calculateMin();
            break;
        case AggregationType::Max:
            return this->calculateMax();
            break;
        default:
            return -1;
            break;
    }

}