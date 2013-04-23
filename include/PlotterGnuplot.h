#ifndef PLOTTERGNUPLOT_H
#define PLOTTERGNUPLOT_H


#include "AbstractPlotter.h"

class PlotterGnuplot : public AbstractPlotter
{
public:
	void callPlot(string resultDir, string plotterScriptDir, string benchName, string benchId);
};


#endif //PLOTTERGNUPLOT_H