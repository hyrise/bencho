/** @file Aggregator.h
 * 
 * Contains the class definition of Aggregator.
 */

#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <vector>
#include <numeric>
#include "math.h"
#include <algorithm>
#include <iostream>

using namespace std;


/// Struct to define the available aggregation functions.
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


/**
 * @brief Class for aggregating functions. 
 *
 * The Aggregator incapsulates different aggregating functions 
 * to use on the result data of the benchmarks. The preferred function
 * for your benchmark can be set directly in your benchmark class 
 * via the setAggregatingFunction(). 
 */
class Aggregator {

	private:
		vector<long long> _data; 

	public:
		/// The Contructor that takes a data vector used for aggregation.
		Aggregator(vector<long long> data);
		virtual ~Aggregator();

		/**
		 * @brief Function to change the data that will be aggregated. 
		 *
		 * @param data A data vector used for aggragation.
		 */
		void setData(vector<long long> data);

		long long calculateAverage();
		long long calculateMedian();
		long long calculateDeviation();
		long long calculateMin();
		long long calculateMax();
		long long calculateFunction(AggregationType::Function function);

};

#endif  //AGGREGATOR_H