#include "PlotterPython.h"


void PlotterPython::callPlot(string resultDir, string plotterScriptDir, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);
	string plotterScript = plotterScriptDir + "/" + benchName + ".py";
	string systemScript = getSystemScriptDir() + "/system.py";

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	string command = "python " + systemScript + " -f " + resultFile + " -s " + plotterScript;
	system(command.c_str());
}