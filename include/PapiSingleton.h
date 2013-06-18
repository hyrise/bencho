
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
            retval = PAPI_thread_init(pthread_self);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI thread initialization failed. ");
                papi_handle_error(retval);
                exit(1);
            }

            _perfEvents = 0;

            retval = PAPI_event_name_to_code((char *) papi, &_perfEvents);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI PAPI_event_name_to_code failed. ");
                papi_handle_error(retval);
                exit(1);
            }

            _eventSet = PAPI_NULL;

            retval = PAPI_create_eventset(&_eventSet);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_create_eventset failed. ");
                papi_handle_error(retval);
                exit(1);
            }
            retval = PAPI_add_event(_eventSet, _perfEvents);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_add_event failed. ");
                papi_handle_error(retval);
                exit(1);
            }
        }
    }

    inline void start() {
        if (_perfCounterName == "walltime") {
            _t1 = PAPI_get_real_usec();
        } else {
            int retval;
            retval = PAPI_start(_eventSet);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_start failed. ");
                papi_handle_error(retval);
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
            int retval;
            retval = PAPI_stop(_eventSet, &papiResult);
            if (retval != PAPI_OK) {
                fprintf(stderr, "Error: PAPI_stop failed. ");
                papi_handle_error(retval);
                exit(1);
            }
            return papiResult;
        }
    }

  private:
    int _perfEvents;
    int _eventSet;
    unsigned long _t1, _t2;

    void papi_handle_error(int errcode)
    {
        switch (errcode)
        {
            case 0:
                break;
            case -1:
                fprintf(stderr, "Invalid argument.\n");
                break;
            case -2:
                fprintf(stderr, "Insufficient memory.\n");
                break;
            case -3:
                fprintf(stderr, "A System/C library call failed.\n");
                break;
            case -4:
                fprintf(stderr, "Not supported by component.\n");
                break;
            case -5:
                fprintf(stderr, "Access to the counters was lost or interrupted.\n");
                break;
            case -6:
                fprintf(stderr, "Internal error, please send mail to the developers.\n");
                break;
            case -7:
                fprintf(stderr, "Event does not exist.\n");
                break;
            case -8:
                fprintf(stderr, "Event exists, but cannot be counted due to counter resource limitations.\n");
                break;
            case -9:
                fprintf(stderr, "EventSet is currently not running.\n");
                break;
            case -10:
                fprintf(stderr, "EventSet is currently counting.\n");
                break;
            case -11:
                fprintf(stderr, "No such EventSet Available.\n");
                break;
            case -12:
                fprintf(stderr, "Event in argument is not a valid preset.\n");
                break;
            case -13:
                fprintf(stderr, "Hardware does not support performance counters.\n");
                break;
            case -14:
                fprintf(stderr, "Unknown error code.\n");
                break;
            case -15:
                fprintf(stderr, "Permission level does not permit operation.\n");
                break;
            case -16:
                fprintf(stderr, "PAPI hasn't been initialized yet.\n");
                break;
            case -17:
                fprintf(stderr, "Component Index isn't set.\n");
                break;
            case -18:
                fprintf(stderr, "Not supported.\n");
                break;
            case -19:
                fprintf(stderr, "Not implemented.\n");
                break;
            case -20:
                fprintf(stderr, "Buffer size exceeded.\n");
                break;
            case -21:
                fprintf(stderr, "EventSet domain is not supported for the operation.\n");
                break;
            case -22:
                fprintf(stderr, "Invalid or missing event attributes.\n");
                break;
            case -23:
                fprintf(stderr, "Too many events or attributes.\n");
                break;
            case -24:
                fprintf(stderr, "Bad combination of features.\n");
                break;
        }
    }

#else // Measuring without PAPI available
public:
    void init(const char *papi) {
        _perfCounterName = std::string(papi);
        if (_perfCounterName != "walltime")
        {
            std::cout << "No PAPI Counters to measure " << _perfCounterName << ". Measuring clock ticks instead. " << std::endl;
        }
    }

    inline void start() {

    	gettimeofday(&_t1, NULL);
    }

    inline long long stop() {

       	gettimeofday(&_t2, NULL);
     	return _t2.tv_sec*1000*1000 + _t2.tv_usec - _t1.tv_sec*1000*1000 - _t1.tv_usec;  // in microseconds
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
