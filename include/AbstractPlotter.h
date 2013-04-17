#ifndef ABSTRACTPLOTTER_H
#define ABSTRACTPLOTTER_H

#include <string>
#include <stdio>

#include "resultFileHelper.h"


class AbstractPlotter
{
public:
	AbstractPlotter();
	virtual ~AbstractPlotter();

	void plot();
	void setUp(bool isDefault);


private:
	string _resultDir;
	string _plotterScriptDir;
	string _systemScriptDir;
	string _benchName;
	string _benchId;

	void plot(string resultDir, string plotterScriptDir, string benchName, string benchId);

	virtual string createFinalScript(string resultFile, string baseScript);
	virtual void callPlot(string resultFile, string scriptFile) = 0; // pure virtual method

	string getResultDir();
	string getPlotterScriptDir();
	string getSystemScriptDir();
	string getBenchName();
	string getBenchId();

	void setResultDir(string resultDir);
	void setPlotterScriptDir(string plotterScriptDir);
	void setSystemScriptDir(string systemScriptDir);
	void setBenchName(string benchName);
	void setBenchId(string benchId);
};



#endif ABSTRACTPLOTTER_H