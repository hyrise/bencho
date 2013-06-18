#include "Parameter.h"

#include <string>
#include <vector>
#include <stdexcept>

Parameter::Parameter(std::string name, std::vector<long long> values) {

    _name = name;
    _values = values;
}

Parameter::Parameter(std::string name, long long start, long long stop, long long step, ParameterType::Operation operation) {
    
    _name = name;

    if (operation == ParameterType::Add) { //with step for addition

        if (start >= stop) {
            throw std::runtime_error("Start is greater or equal than stop.");
        }

        std::vector<long long> v;

        for (long long i = start; i < stop; i += step) {
            v.push_back(i);
        }

        _values = v;

    } else {        //use step as a factor
        
        if (start >= stop) {
            throw std::runtime_error("Start is greater or equal than stop.");
        }

        if (step <= 1) {
            throw std::runtime_error("Factor is not greater 1.");
        }
        
        std::vector<long long> v;
        double val = start;
        
        long long i = -1;
        while ( i <= stop ) {
            if (i != (long long) val) {

                i = (long long) val;
                if (i <= stop)
                    v.push_back(i);
            }
            val *= step;
        }
        
        _values = v;
    }

}

Parameter::Parameter(std::string name, long long value) {
   
    _name = name;

    std::vector<long long> v;
    v.push_back(value);

    _values = v;
}

//Getters:

std::string Parameter::getName() {
    return _name;
}

std::vector<long long> Parameter::getValues() {
    return _values;
}

Parameter::~Parameter() { 

}