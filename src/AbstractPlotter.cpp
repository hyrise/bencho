#include "AbstractPlotter.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <dirent.h>
#include <string.h>

#include "resultFileHelper.h"


AbstractPlotter::AbstractPlotter()
{
	_resultDir = "./results";
	_plotterScriptDir = "./benchmarks";
	_systemScriptDir = "./bencho/plotting";

	_plotterScript = "";
	_systemScript = "";

	_benchName = "";
	_benchId = "";
}

AbstractPlotter::~AbstractPlotter()
{

}


void AbstractPlotter::plot()
{
	std::string resultFile = getResultFile(getBenchName(), getBenchId(), getResultDir());
	if (fileExists(resultFile))
	{
		callPlot(getResultDir(), getPlotterScript(), getSystemScript(), getBenchName(), getBenchId());
	} else {
		std::cerr << "No result file of benchmark: " << getBenchName() << ", Id: " << getBenchId() << " available." << std::endl;
	}
}

void AbstractPlotter::setUp(bool isDefault)
{
	setBenchName(findBenchName(getResultDir()));
	setBenchId(findBenchId(getBenchName(), getResultDir()));

	if(!isDefault)
	{
		std::string userinput;
		std::cout << "Name of Benchmark?" << std::endl << "(default: " << getBenchName() << ")" << std::endl;
		getline(std::cin, userinput);
		if(!userinput.empty())
		{
			setBenchName(userinput);
			setBenchId(findBenchId(getBenchName(), getResultDir()));
		}
		std::cout << "ID?" << std::endl << "(default: " << getBenchId() << ")" << std::endl;
		getline(std::cin, userinput);
		if(!userinput.empty())
		{
			setBenchId(userinput);
		}
	}
}

void AbstractPlotter::setUp(std::string resultDir, std::string plotterScriptDir, std::string systemScriptDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId)
{
	setResultDir(resultDir);
	setPlotterScriptDir(plotterScriptDir);
	setSystemScriptDir(systemScriptDir);
	setPlotterScript(plotterScript);
	setSystemScript(systemScript);
	setBenchName(benchName);
	setBenchId(benchId);
}

void AbstractPlotter::pdfcropResult()
{
	std::string resultDir = getResultDir();
	std::string benchName = getBenchName();
	std::string benchId = getBenchId();

	struct dirent *de=NULL;
  	DIR *d=NULL;
	std::string benchmarksResultDir = resultDir + "/" + benchName;
	std::string resultPrefix = benchName + "_" + benchId + "_";
	d = opendir(benchmarksResultDir.c_str());
	if (d)
	{
		while((de = readdir(d))) {
		if (strncmp(resultPrefix.c_str(), de->d_name, resultPrefix.size()) == 0) {
			pdfCropFile(benchmarksResultDir + "/" + de->d_name);
		}
    }
    closedir(d);
	} else { std::perror(benchmarksResultDir.c_str()); }
}


std::string AbstractPlotter::createFinalScript(std::string /*resultFile*/, std::string baseScript, std::string /*systemScript*/)
{
	// overwrite this in actual plotter
	return baseScript;
}

void AbstractPlotter::callPlot(std::string /*resultDir*/, std::string /*plotterScript*/, std::string /*systemScript*/, std::string /*benchName*/, std::string /*benchId*/)
{
	// overwrite this in actual plotter
}


std::string AbstractPlotter::getResultDir()
{
	return _resultDir;
}

std::string AbstractPlotter::getPlotterScriptDir()
{
	return _plotterScriptDir;
}

std::string AbstractPlotter::getSystemScriptDir()
{
	return _systemScriptDir;
}

std::string AbstractPlotter::getPlotterScript()
{
	return _plotterScript;
}

std::string AbstractPlotter::getSystemScript()
{
	return _systemScript;
}

std::string AbstractPlotter::getBenchName()
{
	return _benchName;
}

std::string AbstractPlotter::getBenchId()
{
	return _benchId;
}

void AbstractPlotter::setResultDir(std::string resultDir)
{
	_resultDir = resultDir;
}

void AbstractPlotter::setPlotterScriptDir(std::string plotterScriptDir)
{
	_plotterScriptDir = plotterScriptDir;
}

void AbstractPlotter::setSystemScriptDir(std::string systemScriptDir)
{
	_systemScriptDir = systemScriptDir;
}

void AbstractPlotter::setPlotterScript(std::string plotterScript)
{
	_plotterScript = plotterScript;
}

void AbstractPlotter::setSystemScript(std::string systemScript)
{
	_systemScript = systemScript;
}

void AbstractPlotter::setBenchName(std::string benchName)
{
	_benchName = benchName;
}

void AbstractPlotter::setBenchId(std::string benchId)
{
	_benchId = benchId;
}
