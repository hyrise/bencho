#include "PlotterR.h"


void PlotterR::callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);
	// string plotterScript = plotterScriptDir + "/" + benchName + ".r";
	// string systemScript = getSystemScriptDir() + "/system.r";

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	string command = "R CMD BATCH --no-save '--args " + resultFile + " " + plotterScript + "' " + systemScript + " /dev/null"; // redirect console output to /dev/null
	system(command.c_str());
}