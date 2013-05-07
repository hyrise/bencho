#include "PlotterGnuplot.h"


void PlotterGnuplot::callPlot(string resultDir, string plotterScript, string systemScript, string benchName, string benchId)
{
	_terminal = TERMINAL;
	string resultFile = getResultFile(benchName, benchId, resultDir);
	// string plotterScript = plotterScriptDir + "/" + benchName + ".gp";
	// string systemScript = getSystemScriptDir() + "/system.gp";

	string finalPlotterScript = createFinalScript(resultFile, plotterScript, systemScript);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	string command = "gnuplot " + finalPlotterScript;
	system(command.c_str());

	// Clean up
	if (fileExists(finalPlotterScript))
	{
		remove(finalPlotterScript.c_str());
	}
}

string PlotterGnuplot::createFinalScript(string resultFile, string baseScript, string systemScript)
{
	string finalPlotterScript = baseScript;
	finalPlotterScript = finalPlotterScript.erase(finalPlotterScript.find(".gp")) + "_final.gp";

	mergeSystemScript(baseScript, systemScript, finalPlotterScript);
	replaceTerminals(finalPlotterScript, resultFile);
	setPerfCounters(finalPlotterScript, resultFile);
	setPlotCommands(finalPlotterScript);

	return finalPlotterScript;
}

void PlotterGnuplot::mergeSystemScript(string baseScript, string systemScript, string mergedScript)
{
	string line;
	ofstream mergeStream;
	mergeStream.open(mergedScript.c_str(), ios::out | ios::app);
	mergeStream << "#!/usr/bin/gnuplot" << endl << endl;

	ifstream systemStream(systemScript.c_str());
	if (systemStream.is_open())
	{
		while(getline(systemStream, line))
		{
			mergeStream << line << endl;
		}
		systemStream.close();
	} else { cerr << "Couldn't open " << systemScript << endl; }

	ifstream baseStream(baseScript.c_str());
	if (baseStream.is_open())
	{
		while(getline(baseStream, line))
		{
			mergeStream << line << endl;
		}
		baseStream.close();
	} else { cerr << "Couldn't open " << baseScript << endl; }

	mergeStream.close();
}

void PlotterGnuplot::replaceTerminals(string baseScript, string resultFile)
{
	string outputString = resultFile;
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
		{ cerr << "Unknown Terminal." << endl; }

	if (PS2PDF)
	{
		bufferSearchReplace(baseScript, "OUTPUT", "| ps2pdf - OUTPUT");
		_terminal = "pdf";
	}

	bufferSearchReplace(baseScript, "OUTPUT", outputString);
	bufferSearchReplace(baseScript, "$(TERMINAL)", _terminal);
}

void PlotterGnuplot::setPerfCounters(string baseScript, string resultFile)
{
	vector<string> counters;
	string buffer;

	counters = getCounters(baseScript);
	for (int i = 0; i < counters.size(); i++)
	{
		int pos = getCounterPosition(counters.at(i), resultFile);
		if (pos != 0)
		{
			stringstream ss;
			ss << pos;
			string position = ss.str();
			string search = "§@" + counters.at(i) + "@§";
			string replace = "$" + position;
			bufferSearchReplace(baseScript, search, replace);
		} else {
			stringstream ss;
			ss << pos;
			string position = ss.str();
			string search = "§@" + counters.at(i) + "@§";

			//cout << "Missing Counter: " << counters.at(i) << endl;

			string tmp_file = baseScript + ".tmp";
			fstream stream_in(baseScript.c_str(), ios::in);
			fstream stream_out_tmp(tmp_file.c_str(), ios::out | ios::app);
			string buffer;
			while(getline(stream_in, buffer))
			{
				if(buffer.find(search) != -1)
				{
					buffer = "#" + buffer;
				}
				stream_out_tmp << buffer << endl;
			}
			stream_out_tmp.close();
			stream_in.close();

			remove(baseScript.c_str());
			fstream stream_in_tmp(tmp_file.c_str(), ios::in);
			fstream stream_out(baseScript.c_str(), ios::out | ios::app);
			while(!stream_in_tmp.eof())
			{
				getline(stream_in_tmp, buffer);
				stream_out << buffer << endl;
			}
			stream_out.close();
			stream_in_tmp.close();
			remove(tmp_file.c_str());
		}
	}
}

void PlotterGnuplot::setPlotCommands(string baseScript)
{
	vector<string> lines;
	string buffer, next;
	string tmp_file = baseScript + ".tmp";

	fstream stream_in(baseScript.c_str(), ios::in);
	fstream stream_out_tmp(tmp_file.c_str(), ios::out | ios::app);

	bool foundplot = false;
	bool needplot = false;
	vector<bool> needed_plots;
	while(getline(stream_in, buffer))
	{
		if(foundplot)
		{
			bool is_empty = true;
			for(int i = 0; i < buffer.length(); i++)
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
				for(int i = 0; i < lines.size(); i++)
				{
					stream_out_tmp << lines.at(i) << endl;
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
				stream_out_tmp << buffer << endl;
			}
		}
	}
	stream_out_tmp.close();
	stream_in.close();

	remove(baseScript.c_str());
	fstream stream_in_tmp(tmp_file.c_str(), ios::in);
	fstream stream_out(baseScript.c_str(), ios::out | ios::app);
	while(!stream_in_tmp.eof())
	{
		getline(stream_in_tmp, buffer);
		stream_out << buffer << endl;
	}
	stream_out.close();
	stream_in_tmp.close();
	remove(tmp_file.c_str());

	fstream stream_in2(baseScript.c_str(), ios::in);
	fstream stream_out_tmp2(tmp_file.c_str(), ios::out | ios::app);
	while(getline(stream_in2, next))
	{
		bool is_empty = true;
		for(int i = 0; i < next.length(); i++)
		{
			if(!isspace(next.at(i)))
				is_empty = false;
		}
		if(is_empty)
		{
			int spot = buffer.find(",\\");
			if(spot == buffer.length()-2)
				buffer = buffer.substr(0,spot);
		}
		stream_out_tmp2 << buffer << endl;
		buffer = next;
	}
	stream_out_tmp2 << buffer << endl;
	stream_out_tmp2.close();
	stream_in2.close();

	remove(baseScript.c_str());
	fstream stream_in_tmp2(tmp_file.c_str(), ios::in);
	fstream stream_out2(baseScript.c_str(), ios::out | ios::app);
	while(!stream_in_tmp2.eof())
	{
		getline(stream_in_tmp2, buffer);
		stream_out2 << buffer << endl;
	}
	stream_out2.close();
	stream_in_tmp2.close();
	remove(tmp_file.c_str());

	fstream stream_in4(baseScript.c_str(), ios::in);
	fstream stream_out_tmp4(tmp_file.c_str(), ios::out | ios::app);
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
		stream_out_tmp4 << buffer << endl;
	}
	stream_out_tmp4.close();
	stream_in4.close();

	remove(baseScript.c_str());
	fstream stream_in_tmp4(tmp_file.c_str(), ios::in);
	fstream stream_out4(baseScript.c_str(), ios::out | ios::app);
	while(!stream_in_tmp4.eof())
	{
		getline(stream_in_tmp4, buffer);
		stream_out4 << buffer << endl;
	}
	stream_out4.close();
	stream_in_tmp4.close();
	remove(tmp_file.c_str());
}

void PlotterGnuplot::bufferSearchReplace(string replaceFile, string search, string replace)
{
	string buffer;
	string tmpFile = replaceFile + ".tmp";
	fstream stream_in(replaceFile.c_str(), ios::in);
	fstream stream_out_tmp(tmpFile.c_str(), ios::out | ios::app);
	while(!stream_in.eof())
	{
		getline(stream_in, buffer);
		int spot = buffer.find(search);
		if(spot >= 0)
		{
			string tmpString = buffer.substr(0,spot);
			tmpString += replace;
			tmpString += buffer.substr(spot+search.length(), buffer.length());
			buffer = tmpString;
		}
		stream_out_tmp << buffer << endl;
	}
	stream_out_tmp.close();
	stream_in.close();

	remove(replaceFile.c_str());
	fstream stream_in_tmp(tmpFile.c_str(), ios::in);
	fstream stream_out(replaceFile.c_str(), ios::out | ios::app);
	while(!stream_in_tmp.eof())
	{
		getline(stream_in_tmp, buffer);
		stream_out << buffer << endl;
	}
	stream_out.close();
	stream_in_tmp.close();
	remove(tmpFile.c_str());
	return;
}

vector<string> PlotterGnuplot::getCounters(string baseScript)
{
	vector<string> counters;
	string linebuffer;
	string begin = "§@";
	string end = "@§";
	fstream stream_in(baseScript.c_str(), ios::in);
	while(!stream_in.eof())
	{
		getline(stream_in, linebuffer);
		int spot_begin = linebuffer.find(begin);
		int spot_end = linebuffer.find(end);
		if(spot_begin >= 0 && spot_end >= 0)
		{
			string tmpstring = linebuffer.substr(spot_begin+begin.length(),spot_end-(spot_begin+begin.length()));
			counters.push_back(tmpstring);
		}
	}
	stream_in.close();
	return counters;
}

int PlotterGnuplot::getCounterPosition(string counter, string resultFile)
{
	string linebuffer;
	fstream stream_in(resultFile.c_str(), ios::in);
	getline(stream_in, linebuffer);
	int spot = linebuffer.find(counter);
	int spaces = 0;
	if(spot >= 0)
	{
		string tmpstring = linebuffer.substr(0,spot);
		for(int i = 0; i < tmpstring.size(); i++)
		{
			spaces += ( tmpstring.at(i) == ' ' );
		}
		stream_in.close();
		return 1+spaces;
	}
	stream_in.close();
	return 0;
}