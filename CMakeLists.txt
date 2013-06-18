cmake_minimum_required(VERSION 2.8)
 
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR})
 
project(bencho C CXX)
 
find_library(PAPI NAMES papi)
 
if ((NOT ${PAPI}) AND (${CMAKE_BUILD_TYPE} MATCHES "RELEASE.*"))
MESSAGE(FATAL_ERROR "cannot set up release build without PAPI!")
endif ()
 
set(bencho_INCLUDE_DIRS ${bencho_SOURCE_DIR}/include
${bencho_BINARY_DIR}/include CACHE STRING "bencho include dirs")
 
include_directories(${bencho_INCLUDE_DIRS})
 
set(BENCHO_HEADERS
include/AbstractBenchmark.h
include/AbstractPlotter.h
include/Aggregator.h
include/bencho.h
include/ConfigFile.h
include/DirectoryManager.h
include/FileWriter.h
include/helper.h
include/main.h
include/PapiSingleton.h
include/Parameter.h
include/PlotterGnuplot.h
include/PlotterPython.h
include/PlotterR.h
include/Printer.h
include/resultFileHelper.h)
 
add_library(bencho STATIC
${BENCHO_HEADERS}
src/AbstractBenchmark.cpp
src/AbstractPlotter.cpp
src/Aggregator.cpp
src/ConfigFile.cpp
src/DirectoryManager.cpp
src/FileWriter.cpp
src/helper.cpp
src/Parameter.cpp
src/PlotterGnuplot.cpp
src/PlotterPython.cpp
src/PlotterR.cpp
src/Printer.cpp
src/resultFileHelper.cpp)
 
set(bencho_CXX_FLAGS "-std=c++0x")

if (${PAPI})
set(bencho_CXX_FLAGS "${bencho_CXX_FLAGS} -D USE_PAPI_TRACE" CACHE_STRING "bencho
compile flags")
add_definitions(USE_PAPI_TRACE)
target_link_libraries(bencho ${PAPI})
endif()

set(CMAKE_CXX_FLAGS ${bencho_CXX_FLAGS})