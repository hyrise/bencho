/** @file PlotterGnuplot.h
 *
 * Contains the class definition of PlotterGnuplot
 * For any undocumented method see AbstractPlotter.
 * @see AbstractPlotter
 *
 */

#ifndef PLOTTERGNUPLOT_H
#define PLOTTERGNUPLOT_H

#define TERMINAL "ps"
#define PS2PDF true

#include "AbstractPlotter.h"

#include <vector>


/**
 * @brief Class for plotting results with Gnuplot.
 *
 * PlotterGnuplot is derived from AbstractPlotter
 * and serves as a plotter class for plotting results with Gnuplot.
 */
class PlotterGnuplot : public AbstractPlotter
{
private:
	std::string _terminal;


	/**
	 * Calls the command for executing the plotting with Gnuplot.
	 * Beforehand it calls createFinalScript() to temporarily customize the .gp script
	 *
	 * @param resultDir Directory where result files of the benchmark are stored.
	 * @param plotterScript Exact (but relative) path to the individual plotting script for the benchmark.
	 * @param systemScript Exact (but relative) path to the general plotting script for the plotter.
	 * @param benchName Name of the benchmark that will be plotted.
	 * @param benchId Id of the benchmark that will be plotted.
	 */
	void callPlot(std::string resultDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId);

	/**
	 * Magic function that creates a Gnuplot-readable .gp script from a generic @a baseScript
	 * and the given @a resultFile.
	 *
	 * @param resultFile Exact (but relative) path to the result file of the benchmark run that will be plotted.
	 * @param baseScript Exact (but relative) path to the plotter script that has to be modified.
	 * @param systemScript Exact (but relative) path to the general plotting script for the plotter.
	 *
	 * @return Exact (but relative) path to the final script that can be used for plotting.
	 */
	virtual std::string createFinalScript(std::string resultFile, std::string baseScript, std::string systemScript);


	/**
	 * Appends the individual generic script to the general @a systemScript for the plotter.
	 *
	 * @param baseScript Relative path to the plotter script that will be appended.
	 * @param systemScript Relative path to the base system script for the plotter.
	 * @param mergedScript Relative path to the script the above scripts will be merged into.
	 */
	void mergeSystemScript(std::string baseScript, std::string systemScript, std::string mergedScript);

	/**
	 * Replaces the placeholders in @a baseScript for the decision which terminal is used
	 * and replaces the @c DATAFILE placeholder with the path to the @a resultFile.
	 *
	 * @param baseScript Relative path to the script which placeholders should be replaced.
	 * @param resultFile Relative path to the result file of the benchmark run that will be plotted.
	 */
	void replaceTerminals(std::string baseScript, std::string resultFile);

	/**
	 * Replaces the placeholders in @a baseScript for the used performance counters.
	 * Determines them by reading the given @a resultFile.
	 *
	 * @param baseScript Relative path to the script which placeholders should be replaced.
	 * @param resultFile Relative path to the result file of the benchmark run that will be plotted.
	 */
	void setPerfCounters(std::string baseScript, std::string resultFile);

	/**
	 * Determines which plot-commands in the given Gnuplot @a baseScript are actual used
	 * and removes unneeded ones to make the Gnuplot script executable.
	 *
	 * @param baseScript Relative path to the script that should be modified.
	 */
	void setPlotCommands(std::string baseScript);


	/**
	 * Helper function that replaces all occurences of @a search with @a replace in the file @a replaceFile.
	 *
	 * @param replaceFile File in which the replacement should take place.
	 * @param search String which should be replaced.
	 * @param replace String which replaces @a search.
	 */
	void bufferSearchReplace(std::string replaceFile, std::string search, std::string replace);


	/**
	 * Finds the potentially used performance counters in the script file @a baseScript.
	 *
	 * @param baseScript The Gnuplot script which contains placeholders for potentially used performance counters.
	 *
	 * @return A vector which contains the potentially used performance counters as strings.
	 */
	std::vector<std::string> getCounters(std::string baseScript);

	/**
	 * Determines the columns of a performance @a counter in a given @a resultFile.
	 *
	 * @param counter The name of the performance counter.
	 * @param resultFile Relative path to the result file which will be used for plotting.
	 *
	 * @return The number in which column the performance @a counter appears in the @a resultFile.
	 */
	int getCounterPosition(std::string counter, std::string resultFile);
};


#endif //PLOTTERGNUPLOT_H