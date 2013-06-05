#include "PlotterPython.h"


void PlotterPython::callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

    string command = "export PYTHONPATH=$PYTHONPATH:" + getSystemScriptDir() + " && python " + plotterScript + " -f " + resultFile + " -s " + plotterScript;
    system(command.c_str());
	// system(command2.c_str());
}