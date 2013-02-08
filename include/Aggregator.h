#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <vector>
#include <numeric>
#include "math.h"
#include <algorithm>
#include <iostream>

using namespace std;

struct AggregationType
{
	enum Function {
		Average,
		Median,
		Min,
		Max,
		numTypes
	};
};

class Aggregator {

	private:
		vector<long long> _data; 

	public:
		Aggregator(vector<long long> &data);
		virtual ~Aggregator();

		void setData(vector<long long> &data);

		long long calculateAverage();
		long long calculateMedian();
		long long calculateDeviation();
		long long calculateMin();
		long long calculateMax();
		long long calculateFunction(AggregationType::Function function);

};

#endif  //AGGREGATOR_H