#ifndef PLOTTERR_H
#define PLOTTERR_H


#include "AbstractPlotter.h"

class PlotterR : public AbstractPlotter
{
public:
	void callPlot(string resultDir, string plotterScriptDir, string benchName, string benchId);
};


#endif //PLOTTERR_H