/** @file AbstractPlotter.h
 *
 * Contains the class definition of AbstractPlotter
 *
 */

#ifndef ABSTRACTPLOTTER_H
#define ABSTRACTPLOTTER_H

#include <string>


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
	void setUp(std::string resultDir, std::string plotterScriptDir, std::string systemScriptDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId);


	/**
	 * Magic function that automatically finds all newly plotted pdfs and calls pdfcrop on them.
	 */
	void pdfcropResult();

	std::string getResultDir();
	std::string getPlotterScriptDir();
	std::string getSystemScriptDir();
	std::string getPlotterScript();
	std::string getSystemScript();
	std::string getBenchName();
	std::string getBenchId();

	void setResultDir(std::string resultDir);
	void setPlotterScriptDir(std::string plotterScriptDir);
	void setSystemScriptDir(std::string systemScriptDir);
	void setPlotterScript(std::string plotterScript);
	void setSystemScript(std::string systemScript);
	void setBenchName(std::string benchName);
	void setBenchId(std::string benchId);

private:
	std::string _resultDir;
	std::string _plotterScriptDir;
	std::string _systemScriptDir;
	std::string _plotterScript;
	std::string _systemScript;
	std::string _benchName;
	std::string _benchId;


	/**
	 * Function that may be needed in derived classes if the plotter script internally has to be modified,
	 * like in the Gnuplot plotter.
	 * 
	 * @param resultFile Exact (but relative) path to the resultFile.
	 * @param baseScript Exact (but relative) path to the plotter script that has to be modified.
	 * @param systemScript Exact (but relative) path to the general plotting script for the plotter.
	 *
	 * @return Exact (but relative) path to the final script that can be used for plotting.
	 */
	virtual std::string createFinalScript(std::string resultFile, std::string baseScript, std::string systemScript);

	/**
	 * Function that calls the individual plotter, has to be overwritten.
	 * 
	 * @param resultDir Directory where result files of the benchmark are stored.
	 * @param plotterScript Exact (but relative) path to the individual plotting script for the benchmark.
	 * @param systemScript Exact (but relative) path to the general plotting script for the plotter.
	 * @param benchName Name of the benchmark that will be plotted.
	 * @param benchId Id of the benchmark that will be plotted.
	 */
	virtual void callPlot(std::string resultDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId);
};



#endif //ABSTRACTPLOTTER_H
