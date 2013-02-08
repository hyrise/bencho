
#ifndef PAPISINGLETON_H
#define PAPISINGLETON_H

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sys/time.h>
#include <stdlib.h>

//#define USE_PAPI_TRACE

#ifdef USE_PAPI_TRACE
#include <papi.h>
#endif

class PapiSingleton {
#ifdef USE_PAPI_TRACE // Measuring with PAPI
  public:
    void init(const char *papi) {
        _perfCounterName = std::string(papi);

        // init papi
        int retval;
        char errstring[PAPI_MAX_STR_LEN];

        if ((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
            fprintf(stderr, "Error: %d %s\n", retval, errstring);
            exit(1);
        }

        if (_perfCounterName != "walltime") {
            // init for threads
            if (PAPI_thread_init(pthread_self) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI thread initialization failed.");
                exit(1);
            }

            _perfEvents = 0;
            if (PAPI_event_name_to_code((char *) papi,
                                        &_perfEvents) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI PAPI_event_name_to_code failed.");
                exit(1);
            }

            _eventSet = PAPI_NULL;

            if (PAPI_create_eventset(&_eventSet) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_create_eventset failed.");
                exit(1);
            }
            if (PAPI_add_event(_eventSet, _perfEvents) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_add_event failed.");
                exit(1);
            }
        }
    }

    inline void start() {
        if (_perfCounterName == "walltime") {
            _t1 = PAPI_get_real_usec();
        } else {
            if (PAPI_start(_eventSet) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_start failed.");
                exit(1);
            }
        }
    }

    inline long long stop() {
        if (_perfCounterName == "walltime") {
            _t2 = PAPI_get_real_usec();
            return (_t2 - _t1);  // in milliseconds
        } else {
            long long papiResult = 0;
            if (PAPI_stop(_eventSet, &papiResult) != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_stop failed.");
                exit(1);
            }
            return papiResult;
        }
    }

  private:
    int _perfEvents;
    int _eventSet;
    unsigned long _t1, _t2;

#else // Measuring without PAPI available
public:
    void init(const char *papi) {
        _perfCounterName = std::string(papi);
        // std::cout << "No PAPI Counters to measure " << _perfCounterName << ". Measuring clock ticks instead. " << std::cout;
    }

    inline void start() {

        if (_perfCounterName == "walltime")
        {
        	gettimeofday(&_t1, NULL);
        } else {
        	gettimeofday(&_t1, NULL);
        }
    }

    inline long long stop() {

    	if (_perfCounterName == "walltime")
        {
        	gettimeofday(&_t2, NULL);
       	 	return _t2.tv_sec*1000*1000 + _t2.tv_usec - _t1.tv_sec*1000*1000 - _t1.tv_usec;  // in microseconds
        } else {
        	gettimeofday(&_t2, NULL);
        	return _t2.tv_sec*1000*1000 + _t2.tv_usec - _t1.tv_sec*1000*1000 - _t1.tv_usec;  // in microseconds
        }
        
    }

private:
    struct timeval _t1, _t2;

#endif

  private:
    //Konstruktor private, damit man sich keine Instanzen holen kann.
    PapiSingleton() {
    }
    //Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
    PapiSingleton(const PapiSingleton &cc);

    std::string _perfCounterName;

  public:
    static PapiSingleton &getInstance() {
        static PapiSingleton instance;
        return instance;
    }
};

#endif
