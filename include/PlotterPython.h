/** @file PlotterPython.h
 *
 * Contains the class definition of PlotterPython.
 * For any undocumented method see AbstractPlotter.
 * @see AbstractPlotter
 *
 */

#ifndef PLOTTERPYTHON_H
#define PLOTTERPYTHON_H


#include "AbstractPlotter.h"


/**
 * @brief Class for plotting results with Python matplotlib.
 *
 * PlotterPython is derived from AbstractPlotter
 * and serves as a plotter class for plotting results with Pyhton matplotlib.
 */
class PlotterPython : public AbstractPlotter
{
private:

	/**
	 * Calls the command for executing the plotting with Python.
	 *
	 * @param resultDir Directory where result files of the benchmark are stored.
	 * @param plotterScript Relative path to the individual plotting script for the benchmark.
	 * @param systemScript Relative path to the general plotting script for the plotter.
	 * @param benchName Name of the benchmark that will be plotted.
	 * @param benchId Id of the benchmark that will be plotted.
	 */
	void callPlot(std::string resultDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId);
};


#endif //PLOTTERPYTHON_H
