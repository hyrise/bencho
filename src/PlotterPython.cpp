#include "PlotterPython.h"


void PlotterPython::callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	string command = "python " + systemScript + " -f " + resultFile + " -s " + plotterScript;
	system(command.c_str());
}