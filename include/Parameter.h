/** @file Parameter.h
 *
 * Contains the class definition of Parameter.
 */

#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <vector>


/// Struct to define the parameter type and thereby the operator used to calculate the steps.
struct ParameterType{

	enum Operation {
		Add,
		Multiply
	};
    
};

/**
 * @brief Class for parameters added to a benchmark.
 *
 * The Parameter class is the class used to provide parameters to your 
 * benchmark. They always consist of a name as identifier and a vector of values
 * used as input parameters for in your benchmark, in which the test will be executed 
 * for every combination of the values provided. The constructors can take a 
 * vector of values, a single value or a start, stop and step value, with which it will 
 * calculate the vector of input parameters itself.
 */
class Parameter {
	
	private:
		std::string _name;
		std::vector<long long> _values;

	public:
		/**
		 * @brief Constructor that takes a premade vector of values.
		 *
		 * @param values A vector of integer values (long long).
		 */
		Parameter(std::string name, std::vector<long long> values);

		/**
		 * @brief Constructor that takes a start, stop and step value to calculate the set of values.
		 *
		 * @param start The value where your data set will start. 
		 * @param stop The value for the end of your data set.
		 * @param step The step between the values.
		 * @param operation The operation used to calculate the next value.
		 */
		Parameter(std::string name, long long start, long long stop, long long step, ParameterType::Operation operation);

		/**
		 * @brief Constructor that takes a single value
		 *
		 * @param value The value of your Parameter 
		 */
		Parameter(std::string name, long long value);
		virtual ~Parameter();

		std::string getName();
		std::vector<long long> getValues();
};

#endif //PARAMETER_H
