#include "PlotterPython.h"


void PlotterPython::callPlot(string resultDir, string plotterScriptDir, string benchName, string benchId)
{
	resultFile = getResultFile(benchName, benchId, resultDir);
	plotterScript = plotterScriptDir + "/" + benchName + ".py";
	systemScript = getSystemScriptDir() + "/system.py";

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	command = "python " + systemScript + " -f " + resultFile + " -s " + plotterScript;
	system(command.c_str());
}