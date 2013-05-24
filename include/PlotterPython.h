#ifndef PLOTTERPYTHON_H
#define PLOTTERPYTHON_H


#include "AbstractPlotter.h"

class PlotterPython : public AbstractPlotter
{
private:
	void callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId);
};


#endif //PLOTTERPYTHON_H