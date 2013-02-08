#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <vector>
#include <stdexcept>


using namespace std;

struct ParameterType{

	enum Operation {
		Add,
		Multiply
	};
    
};


class Parameter {
	
	private:
		string _name;
		vector<long long> _values;

	public:
		Parameter(string name, vector<long long> values);
		Parameter(string name, long long start, long long stop, long long step, ParameterType::Operation operation);
		Parameter(string name, long long value);
		virtual ~Parameter();

		string getName();
		vector<long long> getValues();
};

#endif //PARAMETER_H
