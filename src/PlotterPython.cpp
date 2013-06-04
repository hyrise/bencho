#include "PlotterPython.h"


void PlotterPython::callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

    string command1 = "export PYTHONPATH=$PYTHONPATH: " + getSystemScriptDir();
	string command2 = "python " + plotterScript + " -f " + resultFile + " -s " + plotterScript;
    system(command1.c_str());
	system(command2.c_str());
}