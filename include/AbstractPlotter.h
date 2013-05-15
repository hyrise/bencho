/**
 * @file AbstractPlotter.h
 *
 * Contains the class definition of AbstractPlotter
 *
 */

#ifndef ABSTRACTPLOTTER_H
#define ABSTRACTPLOTTER_H

#include <string>
#include <iostream>

#include <dirent.h>
#include <string.h>

#include "resultFileHelper.h"


/**
 * @brief Base plotting class.
 *
 * AbstractPlotter is the base class for the whole plotting in Bencho,
 * providing an interface to access plotting functions and
 * most of the needed functions for inherited plotting classes
 */
class AbstractPlotter
{
public:

	/**
	 * Constructor
	 */
	AbstractPlotter();

	/**
	 * Destructor
	 */
	virtual ~AbstractPlotter();


	/**
	 * @brief The function you call to invoke plotting action.
	 *
	 * Calls the plotting function with the settings specified in setUp()
	 * @see AbstractPlotter#setUp
	 */
	void plot();

	/**
	 * Sets either default settings for plotting or asks for specific ones.
	 * 
	 * @param isDefault If true, setUp will automatically look for the last plotted benchmark
	 * and finds its result file etc.
	 * @param isDefault If false, setUp will ask for the name and id of the benchmark that will be plotted.
	 */
	void setUp(bool isDefault);

	/**
	 * Sets all needed settings for plotting manually.
	 * 
	 * @param resultDir Directory where result files of the benchmark are stored.
	 * @param plotterScriptDir Directory where you find the individual plotting script for the benchmark.
	 * @param systemScriptDir Directory where you find the general plotting script for the plotter.
	 * @param plotterScript Exact (but relative) path to the individual plotting script for the benchmark.
	 * @param systemScript Exact (but relative) path to the general plotting script for the plotter.
	 * @param benchName Name of the benchmark that will be plotted.
	 * @param benchId Id of the benchmark that will be plotted.
	 */
	void setUp(string resultDir, string plotterScriptDir, string systemScriptDir, string plotterScript, string systemScript, string benchName, string benchId);


	/**
	 * Magic function that automatically finds all newly plotted pdfs and calls pdfcrop on them.
	 */
	void pdfcropResult();

	string getResultDir();
	string getPlotterScriptDir();
	string getSystemScriptDir();
	string getPlotterScript();
	string getSystemScript();
	string getBenchName();
	string getBenchId();

	void setResultDir(string resultDir);
	void setPlotterScriptDir(string plotterScriptDir);
	void setSystemScriptDir(string systemScriptDir);
	void setPlotterScript(string plotterScript);
	void setSystemScript(string systemScript);
	void setBenchName(string benchName);
	void setBenchId(string benchId);

private:
	string _resultDir;
	string _plotterScriptDir;
	string _systemScriptDir;
	string _plotterScript;
	string _systemScript;
	string _benchName;
	string _benchId;


	/**
	 * Function that may be needed in derived classes if the plotter script internally has to be modified,
	 * like in the Gnuplot plotter.
	 * 
	 * @param resultFile Exact (but relative) path to the resultFile.
	 * @param baseScript Exact (but relative) path to the plotter script that has to be modified.
	 *
	 * @return Exact (but relative) path to the final script that can be used for plotting
	 */
	virtual string createFinalScript(string resultFile, string baseScript, string systemScript);

	/**
	 * Function that calls the individual plotter, has to be overwritten.
	 */
	virtual void callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId);
};



#endif //ABSTRACTPLOTTER_H