#include "AbstractPlotter.h"

class PlotterPython : public AbstractPlotter
{
private:
	string resultFile;
	string plotterScript;
	string systemScript;
	string command;

public:
	void callPlot(string resultDir, string plotterScriptDir, string benchName, string benchId);
};