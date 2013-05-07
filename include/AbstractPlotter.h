#ifndef ABSTRACTPLOTTER_H
#define ABSTRACTPLOTTER_H

#include <string>
#include <iostream>

#include <dirent.h>
#include <string.h>

#include "resultFileHelper.h"


class AbstractPlotter
{
public:
	AbstractPlotter();
	virtual ~AbstractPlotter();

	void plot();
	void setUp(bool isDefault);
	void setUp(string resultDir, string plotterScriptDir, string systemScriptDir, string plotterScript, string systemScript, string benchName, string benchId);

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

	virtual string createFinalScript(string resultFile, string baseScript);
	virtual void callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId);
};



#endif //ABSTRACTPLOTTER_H