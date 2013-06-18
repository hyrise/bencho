#include "PlotterGnuplot.h"

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "resultFileHelper.h"


void PlotterGnuplot::callPlot(std::string resultDir, std::string plotterScript, std::string systemScript, std::string benchName, std::string benchId)
{
	_terminal = TERMINAL;
	std::string resultFile = getResultFile(benchName, benchId, resultDir);

	std::string finalPlotterScript = createFinalScript(resultFile, plotterScript, systemScript);

	std::cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << std::endl;

	std::string command = "gnuplot " + finalPlotterScript;
	system(command.c_str());

	// Clean up
	if (fileExists(finalPlotterScript))
	{
		remove(finalPlotterScript.c_str());
	}
}

std::string PlotterGnuplot::createFinalScript(std::string resultFile, std::string baseScript, std::string systemScript)
{
	std::string finalPlotterScript = baseScript;
	finalPlotterScript = finalPlotterScript.erase(finalPlotterScript.find(".gp")) + "_final.gp";

	mergeSystemScript(baseScript, systemScript, finalPlotterScript);
	replaceTerminals(finalPlotterScript, resultFile);
	setPerfCounters(finalPlotterScript, resultFile);
	setPlotCommands(finalPlotterScript);

	return finalPlotterScript;
}

void PlotterGnuplot::mergeSystemScript(std::string baseScript, std::string systemScript, std::string mergedScript)
{
	std::string line;
	std::ofstream mergeStream;
	mergeStream.open(mergedScript.c_str(), std::ios::out | std::ios::app);
	mergeStream << "#!/usr/bin/gnuplot" << std::endl << std::endl;

	std::ifstream systemStream(systemScript.c_str());
	if (systemStream.is_open())
	{
		while(getline(systemStream, line))
		{
			mergeStream << line << std::endl;
		}
		systemStream.close();
	} else { std::cerr << "Couldn't open " << systemScript << std::endl; }

	std::ifstream baseStream(baseScript.c_str());
	if (baseStream.is_open())
	{
		while(getline(baseStream, line))
		{
			mergeStream << line << std::endl;
		}
		baseStream.close();
	} else { std::cerr << "Couldn't open " << baseScript << std::endl; }

	mergeStream.close();
}

void PlotterGnuplot::replaceTerminals(std::string baseScript, std::string resultFile)
{
	std::string outputString = resultFile;
	outputString = outputString.erase(outputString.find(".result.csv")) + "_Gp_";

	bufferSearchReplace(baseScript, "DATAFILE", resultFile);
	bufferSearchReplace(baseScript, "TERMINAL_PDF", "# TERMINAL_PDF");
	bufferSearchReplace(baseScript, "TERMINAL_X11", "# TERMINAL_X11");
	bufferSearchReplace(baseScript, "TERMINAL_PS", "# TERMINAL_PS");
	bufferSearchReplace(baseScript, "TERMINAL_TEST", "# TERMINAL_TEST");

	if(_terminal == "x11")
		{ bufferSearchReplace(baseScript, "# TERMINAL_X11: ", ""); }
	else if(_terminal == "pdf")
		{ bufferSearchReplace(baseScript, "# TERMINAL_PDF: ", ""); }
	else if(_terminal == "ps")
		{ bufferSearchReplace(baseScript, "# TERMINAL_PS: ", ""); }
	else if(_terminal == "xps")
		{ bufferSearchReplace(baseScript, "# TERMINAL_PS: ", ""); }
	else
		{ std::cerr << "Unknown Terminal." << std::endl; }

	if (PS2PDF)
	{
		bufferSearchReplace(baseScript, "OUTPUT", "| ps2pdf - OUTPUT");
		_terminal = "pdf";
	}

	bufferSearchReplace(baseScript, "OUTPUT", outputString);
	bufferSearchReplace(baseScript, "$(TERMINAL)", _terminal);
}

void PlotterGnuplot::setPerfCounters(std::string baseScript, std::string resultFile)
{
	std::vector<std::string> counters;
	std::string buffer;

	counters = getCounters(baseScript);
	for (std::size_t i = 0; i < counters.size(); i++)
	{
		int pos = getCounterPosition(counters.at(i), resultFile);
		if (pos != 0)
		{
			std::stringstream ss;
			ss << pos;
			std::string position = ss.str();
			std::string search = "§@" + counters.at(i) + "@§";
			std::string replace = "$" + position;
			bufferSearchReplace(baseScript, search, replace);
		} else {
			std::stringstream ss;
			ss << pos;
			std::string position = ss.str();
			std::string search = "§@" + counters.at(i) + "@§";

			//std::cout << "Missing Counter: " << counters.at(i) << std::endl;

			std::string tmp_file = baseScript + ".tmp";
			std::fstream stream_in(baseScript.c_str(), std::ios::in);
			std::fstream stream_out_tmp(tmp_file.c_str(), std::ios::out | std::ios::app);
			std::string buffer;
			while(getline(stream_in, buffer))
			{
				if(buffer.find(search) != std::string::npos)
				{
					buffer = "#" + buffer;
				}
				stream_out_tmp << buffer << std::endl;
			}
			stream_out_tmp.close();
			stream_in.close();

			remove(baseScript.c_str());
			std::fstream stream_in_tmp(tmp_file.c_str(), std::ios::in);
			std::fstream stream_out(baseScript.c_str(), std::ios::out | std::ios::app);
			while(!stream_in_tmp.eof())
			{
				getline(stream_in_tmp, buffer);
				stream_out << buffer << std::endl;
			}
			stream_out.close();
			stream_in_tmp.close();
			remove(tmp_file.c_str());
		}
	}
}

void PlotterGnuplot::setPlotCommands(std::string baseScript)
{
	std::vector<std::string> lines;
	std::string buffer, next;
	std::string tmp_file = baseScript + ".tmp";

	std::fstream stream_in(baseScript.c_str(), std::ios::in);
	std::fstream stream_out_tmp(tmp_file.c_str(), std::ios::out | std::ios::app);

	bool foundplot = false;
	bool needplot = false;
	std::vector<bool> needed_plots;
	while(getline(stream_in, buffer))
	{
		if(foundplot)
		{
			bool is_empty = true;
			for(std::size_t i = 0; i < buffer.length(); i++)
			{
				if(!isspace(buffer.at(i)))
					is_empty = false;
			}
			if(!is_empty)
			{
				int spot = buffer.find("#");
				if(spot != 0)
				{
					needplot = true;
					lines.push_back(buffer);
				}
			} else {
				lines.push_back(buffer);
				if(!needplot)
				{
					stream_out_tmp << "#";
					needed_plots.push_back(false);
				} else {
					needed_plots.push_back(true);
				}
				for(std::size_t i = 0; i < lines.size(); i++)
				{
					stream_out_tmp << lines.at(i) << std::endl;
				}
				lines.clear();
				needplot = false;
				foundplot = false;
			}
		} else {
			int spot = buffer.find("plot\\");
			if(spot >= 0)
			{
				foundplot = true;
				lines.push_back(buffer);
			} else {
				stream_out_tmp << buffer << std::endl;
			}
		}
	}
	stream_out_tmp.close();
	stream_in.close();

	remove(baseScript.c_str());
	std::fstream stream_in_tmp(tmp_file.c_str(), std::ios::in);
	std::fstream stream_out(baseScript.c_str(), std::ios::out | std::ios::app);
	while(!stream_in_tmp.eof())
	{
		getline(stream_in_tmp, buffer);
		stream_out << buffer << std::endl;
	}
	stream_out.close();
	stream_in_tmp.close();
	remove(tmp_file.c_str());

	std::fstream stream_in2(baseScript.c_str(), std::ios::in);
	std::fstream stream_out_tmp2(tmp_file.c_str(), std::ios::out | std::ios::app);
	while(getline(stream_in2, next))
	{
		bool is_empty = true;
		for(std::size_t i = 0; i < next.length(); i++)
		{
			if(!isspace(next.at(i)))
				is_empty = false;
		}
		if(is_empty)
		{
			unsigned long spot = buffer.find(",\\");
			if(spot == buffer.length()-2)
				buffer = buffer.substr(0,spot);
		}
		stream_out_tmp2 << buffer << std::endl;
		buffer = next;
	}
	stream_out_tmp2 << buffer << std::endl;
	stream_out_tmp2.close();
	stream_in2.close();

	remove(baseScript.c_str());
	std::fstream stream_in_tmp2(tmp_file.c_str(), std::ios::in);
	std::fstream stream_out2(baseScript.c_str(), std::ios::out | std::ios::app);
	while(!stream_in_tmp2.eof())
	{
		getline(stream_in_tmp2, buffer);
		stream_out2 << buffer << std::endl;
	}
	stream_out2.close();
	stream_in_tmp2.close();
	remove(tmp_file.c_str());

	std::fstream stream_in4(baseScript.c_str(), std::ios::in);
	std::fstream stream_out_tmp4(tmp_file.c_str(), std::ios::out | std::ios::app);
	int i = 0;
	while(getline(stream_in4, buffer))
	{
		int spot = buffer.find("set output \"");
		if(spot == 0)
		{
			if(!needed_plots.at(i))
			{
				stream_out_tmp4 << "# ";
			}
			i++;
		}
		stream_out_tmp4 << buffer << std::endl;
	}
	stream_out_tmp4.close();
	stream_in4.close();

	remove(baseScript.c_str());
	std::fstream stream_in_tmp4(tmp_file.c_str(), std::ios::in);
	std::fstream stream_out4(baseScript.c_str(), std::ios::out | std::ios::app);
	while(!stream_in_tmp4.eof())
	{
		getline(stream_in_tmp4, buffer);
		stream_out4 << buffer << std::endl;
	}
	stream_out4.close();
	stream_in_tmp4.close();
	remove(tmp_file.c_str());
}

void PlotterGnuplot::bufferSearchReplace(std::string replaceFile, std::string search, std::string replace)
{
	std::string buffer;
	std::string tmpFile = replaceFile + ".tmp";
	std::fstream stream_in(replaceFile.c_str(), std::ios::in);
	std::fstream stream_out_tmp(tmpFile.c_str(), std::ios::out | std::ios::app);
	while(!stream_in.eof())
	{
		getline(stream_in, buffer);
		int spot = buffer.find(search);
		if(spot >= 0)
		{
			std::string tmpString = buffer.substr(0,spot);
			tmpString += replace;
			tmpString += buffer.substr(spot+search.length(), buffer.length());
			buffer = tmpString;
		}
		stream_out_tmp << buffer << std::endl;
	}
	stream_out_tmp.close();
	stream_in.close();

	remove(replaceFile.c_str());
	std::fstream stream_in_tmp(tmpFile.c_str(), std::ios::in);
	std::fstream stream_out(replaceFile.c_str(), std::ios::out | std::ios::app);
	while(!stream_in_tmp.eof())
	{
		getline(stream_in_tmp, buffer);
		stream_out << buffer << std::endl;
	}
	stream_out.close();
	stream_in_tmp.close();
	remove(tmpFile.c_str());
	return;
}

std::vector<std::string> PlotterGnuplot::getCounters(std::string baseScript)
{
	std::vector<std::string> counters;
	std::string linebuffer;
	std::string begin = "§@";
	std::string end = "@§";
	std::fstream stream_in(baseScript.c_str(), std::ios::in);
	while(!stream_in.eof())
	{
		getline(stream_in, linebuffer);
		int spot_begin = linebuffer.find(begin);
		int spot_end = linebuffer.find(end);
		if(spot_begin >= 0 && spot_end >= 0)
		{
			std::string tmpstring = linebuffer.substr(spot_begin+begin.length(),spot_end-(spot_begin+begin.length()));
			counters.push_back(tmpstring);
		}
	}
	stream_in.close();
	return counters;
}

int PlotterGnuplot::getCounterPosition(std::string counter, std::string resultFile)
{
	std::string linebuffer;
	std::fstream stream_in(resultFile.c_str(), std::ios::in);
	getline(stream_in, linebuffer);
	int spot = linebuffer.find(counter);
	int spaces = 0;
	if(spot >= 0)
	{
		std::string tmpstring = linebuffer.substr(0,spot);
		for(std::size_t i = 0; i < tmpstring.size(); i++)
		{
			spaces += ( tmpstring.at(i) == ' ' );
		}
		stream_in.close();
		return 1+spaces;
	}
	stream_in.close();
	return 0;
}
