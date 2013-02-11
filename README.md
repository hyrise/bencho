# Bencho

Bencho is a C++ benchmarking framework that will help you create and run your own benchmarks.

## Current Status

The Bencho Framework can be used already, but it is still in development. Features can change or will be added and even some basic functionality could change in the next versions. 

The following features are currently implemented:

  * Create benchmarks by using the AbstractBenchmark as template
  * Add Parameters easily
  * Measure different performance counters at the same time
  * Register different test series
  * Supports warm-up runs, calibration runs, chache clearing, parameter versions
  * All common aggregating functions included in an Aggregator
  * Save results in CSV formated files
  * Plot results directly via GnuPlot
  
## Building

Supported operating systems are Linux and Mac OS X, however the PAPI library wont work on the Apple operating systems.

To build the Bencho framework you will only need to execute 

	make

If not specified already, it will automatically ask you for your prefered Configuration of the options production mode, papi use and verbose build. If you want to change these settings later just execute

	make config

The Bencho framework is now ready to use. On how to include this framework in your project best, please take a look at the [Bencho Sample](https://github.com/schwald/benchosample) or at the documentation.

## Documentation

The Bencho project provides a detailed documentation on how to use and set up the framework. The ready build documentation can be found in the docs directory.