#include "Gnup.h"


Gnup::Gnup()
{
	_result_dir = "./results";
	_bench_script_dir = "./benchmarks";
	_system_script_dir = "./bencho/plotting";
	_bench_name = "";
	_id = "";

	_terminal = "xps";
}

Gnup::Gnup(string result_dir, string bench_script_dir)
{
	_result_dir = result_dir;
	_bench_script_dir = bench_script_dir;
	_system_script_dir = "./bencho/plotting";
	_bench_name = "";
	_id = "";

	_terminal = "xps";
}

Gnup::~Gnup()
{

}


///// Setters /////

void Gnup::setBenchName(string bench_name)
{
	_bench_name = bench_name;
}

void Gnup::setBenchId(string id)
{
	_id = id;
}


///// Getters /////

string Gnup::getBenchName()
{
	return _bench_name;
}

string Gnup::getBenchId()
{
	return _id;
}

// string Gnup::getResultFile(string bench_name, string id)
// {
// 	string _resultFile = _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".result.csv";
// 	return _resultFile;
// }

// string Gnup::getParameterFile(string bench_name, string id)
// {
// 	string _parameterFile = _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + ".parameter.txt";
// 	return _parameterFile;
// }


///// Search /////

// string Gnup::findBenchName()
// {
// 	string last_name = _bench_name;
// 	string last_file = _result_dir + "/last.txt";

// 	ifstream last_name_in;
// 	last_name_in.open (last_file.c_str());

// 	if(last_name_in)
// 	{
// 		last_name_in >> last_name;
// 		last_name_in.close();
// 	} else { cerr << "Couldn't open " << last_file << "." << endl; }
// 	return last_name;
// }

// string Gnup::findBenchId(string bench_name)
// {
// 	string actual_id = _id;
// 	string id_file = _result_dir + "/" + bench_name + "/id.txt";

// 	ifstream actual_id_in;
// 	actual_id_in.open(id_file.c_str());

// 	if(actual_id_in)
// 	{
// 		actual_id_in >> actual_id;
// 		actual_id_in.close();
// 	} else { cerr << "Couldn't open " << id_file << "." << endl; }
// 	return actual_id;
// }


///// Script merging and finalizing /////

int Gnup::createBaseScript(string bench_name)
{
	string temp_script = bench_name + "_temp.gp";
	string system_script = _system_script_dir + "/system.gp";
	string bench_script = _bench_script_dir + "/" + bench_name + ".gp";
	string line;

	if(fileExists(temp_script))
	{
		remove(temp_script.c_str());
	}

	ofstream base_script;
	base_script.open (temp_script.c_str(), ios::out | ios::app);
	base_script << "#!/usr/bin/gnuplot" << endl << endl;


	ifstream read_system (system_script.c_str());
	if(read_system.is_open())
	{
		while(getline(read_system, line))
		{
			base_script << line << endl;
		}
		read_system.close();
	} else { cerr << "Couldn't open " << system_script << endl; return 1; }

	ifstream read_bench_script (bench_script.c_str());
	if(read_bench_script.is_open())
	{
		while(getline(read_bench_script, line))
		{
			base_script << line << endl;
		}
		read_bench_script.close();
	} else { cerr << "Couldn't open " << bench_script << endl; return 1; }

	base_script.close();
	return 0;
}

int Gnup::createFinalScript(string bench_name, string id)
{
	string temp_script = bench_name + "_temp.gp";
	string final_script = bench_name + "_final.gp";
	string result_file = getResultFile(bench_name, id, _result_dir);
	string buffer;

	if(fileExists(final_script))
	{
		remove(final_script.c_str());
	}

	if(!(fileExists(temp_script)))
	{
		if(Gnup::createBaseScript(bench_name))
		{
			remove(temp_script.c_str());
			return 1;
		}
	}

	fstream stream_in(temp_script.c_str(), ios::in);
	fstream stream_out(final_script.c_str(), ios::out | ios::app);
	while(!stream_in.eof())
	{
		getline(stream_in, buffer);
		stream_out << buffer << endl;
	}
	stream_out.close();
	stream_in.close();
	Gnup::bufferSearchReplace(final_script, "DATAFILE", result_file);
	Gnup::bufferSearchReplace(final_script, "TERMINAL_PDF", "# TERMINAL_PDF");
	Gnup::bufferSearchReplace(final_script, "TERMINAL_X11", "# TERMINAL_X11");
	Gnup::bufferSearchReplace(final_script, "TERMINAL_PS", "# TERMINAL_PS");


	// The code below caused to interrupt the flow every single benchmark execution.
	// Therefore I commented it out.
	// Need to solve the Terminal decision in another way.

	// cout << "Terminal?" << endl << "(default: " << _terminal << ")" << endl;
	// string userinput;
	// getline(cin, userinput);
	// if(!userinput.empty())
	// {
	// 	_terminal = userinput;
	// }

	Gnup::bufferSearchReplace(final_script, "$(TERMINAL)", _terminal);

	if(_terminal == "x11")
		{ Gnup::bufferSearchReplace(final_script, "# TERMINAL_X11: ", ""); }
	else if(_terminal == "pdf")
		{ Gnup::bufferSearchReplace(final_script, "# TERMINAL_PDF: ", ""); }
	else if(_terminal == "ps")
		{ Gnup::bufferSearchReplace(final_script, "# TERMINAL_PS: ", ""); }
	else if(_terminal == "xps")
		{ Gnup::bufferSearchReplace(final_script, "# TERMINAL_PS: ", ""); }
	else
		{ cout << "Unknown Terminal." << endl; }


//////////////// Get rid of unused performance-counters in gp-script /////////////////////
	Gnup::clearScript(final_script, result_file);
//////////////////////////////////////////////////////////////////////////////////////////

	//Gnup::plot(bench_name, id, final_script);

	remove(temp_script.c_str());
	//remove(final_script.c_str());
	return 0;
}

void Gnup::clearScript(string final_script, string result_file)
{
	vector<string> counters;
	vector<string> lines;
	string buffer, next;
	string tmp_file = final_script + ".tmp";

	counters = Gnup::getCounters(final_script);
	for (int i = 0; i < counters.size(); i++)
	{
		int pos = Gnup::getCounterPosition(counters.at(i), result_file);
		if (pos != 0)
		{
			stringstream ss;
			ss << pos;
			string position = ss.str();
			string search = "§@" + counters.at(i) + "@§";
			string replace = "$" + position;
			Gnup::bufferSearchReplace(final_script, search, replace);
		} else {
			stringstream ss;
			ss << pos;
			string position = ss.str();
			string search = "§@" + counters.at(i) + "@§";

			//cout << "Missing Counter: " << counters.at(i) << endl;

			string tmp_file = final_script + ".tmp";
			fstream stream_in(final_script.c_str(), ios::in);
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

			remove(final_script.c_str());
			fstream stream_in_tmp(tmp_file.c_str(), ios::in);
			fstream stream_out(final_script.c_str(), ios::out | ios::app);
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

	fstream stream_in2(final_script.c_str(), ios::in);
	fstream stream_out_tmp2(tmp_file.c_str(), ios::out | ios::app);

	bool foundplot = false;
	bool needplot = false;
	vector<bool> needed_plots;
	while(getline(stream_in2, buffer))
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
					stream_out_tmp2 << "#";
					needed_plots.push_back(false);
				} else {
					needed_plots.push_back(true);
				}
				for(int i = 0; i < lines.size(); i++)
				{
					stream_out_tmp2 << lines.at(i) << endl;
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
				stream_out_tmp2 << buffer << endl;
			}
		}
	}
	stream_out_tmp2.close();
	stream_in2.close();

	remove(final_script.c_str());
	fstream stream_in_tmp2(tmp_file.c_str(), ios::in);
	fstream stream_out2(final_script.c_str(), ios::out | ios::app);
	while(!stream_in_tmp2.eof())
	{
		getline(stream_in_tmp2, buffer);
		stream_out2 << buffer << endl;
	}
	stream_out2.close();
	stream_in_tmp2.close();
	remove(tmp_file.c_str());

	fstream stream_in3(final_script.c_str(), ios::in);
	fstream stream_out_tmp3(tmp_file.c_str(), ios::out | ios::app);
	while(getline(stream_in3, next))
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
		stream_out_tmp3 << buffer << endl;
		buffer = next;
	}
	stream_out_tmp3 << buffer << endl;
	stream_out_tmp3.close();
	stream_in3.close();

	remove(final_script.c_str());
	fstream stream_in_tmp3(tmp_file.c_str(), ios::in);
	fstream stream_out3(final_script.c_str(), ios::out | ios::app);
	while(!stream_in_tmp3.eof())
	{
		getline(stream_in_tmp3, buffer);
		stream_out3 << buffer << endl;
	}
	stream_out3.close();
	stream_in_tmp3.close();
	remove(tmp_file.c_str());

	fstream stream_in4(final_script.c_str(), ios::in);
	fstream stream_out_tmp4(tmp_file.c_str(), ios::out | ios::app);
	int i = 0;
	while(getline(stream_in4, buffer))
	{
		int spot = buffer.find("set output \"plot");
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

	remove(final_script.c_str());
	fstream stream_in_tmp4(tmp_file.c_str(), ios::in);
	fstream stream_out4(final_script.c_str(), ios::out | ios::app);
	while(!stream_in_tmp4.eof())
	{
		getline(stream_in_tmp4, buffer);
		stream_out4 << buffer << endl;
	}
	stream_out4.close();
	stream_in_tmp4.close();
	remove(tmp_file.c_str());
}

void Gnup::bufferSearchReplace(string replace_file, string search, string replace)
{
	string buffer;
	string tmp_file = replace_file + ".tmp";
	fstream stream_in(replace_file.c_str(), ios::in);
	fstream stream_out_tmp(tmp_file.c_str(), ios::out | ios::app);
	while(!stream_in.eof())
	{
		getline(stream_in, buffer);
		int spot = buffer.find(search);
		if(spot >= 0)
		{
			string tmpstring = buffer.substr(0,spot);
			tmpstring += replace;
			tmpstring += buffer.substr(spot+search.length(), buffer.length());
			buffer = tmpstring;
		}
		stream_out_tmp << buffer << endl;
	}
	stream_out_tmp.close();
	stream_in.close();

	remove(replace_file.c_str());
	fstream stream_in_tmp(tmp_file.c_str(), ios::in);
	fstream stream_out(replace_file.c_str(), ios::out | ios::app);
	while(!stream_in_tmp.eof())
	{
		getline(stream_in_tmp, buffer);
		stream_out << buffer << endl;
	}
	stream_out.close();
	stream_in_tmp.close();
	remove(tmp_file.c_str());
	return;
}

// bool Gnup::fileExists(string file_name)
// {
// 	ifstream filestream;
// 	filestream.open(file_name.c_str());
// 	if(filestream)
// 	{
// 		filestream.close();
// 		return true;
// 	} else {
// 		filestream.close();
// 		return false;
// 	}
// }

vector<string> Gnup::getCounters(string script)
{
	vector<string> counters;
	string linebuffer;
	string begin = "§@";
	string end = "@§";
	fstream stream_in(script.c_str(), ios::in);
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

int Gnup::getCounterPosition(string counter, string csv)
{
	string linebuffer;
	fstream stream_in(csv.c_str(), ios::in);
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

void Gnup::plot(string bench_name, string id)
{
	cout << "Benchmark: \"" + bench_name + "\", ID: " + id << endl;

	string bench_script = bench_name + "_final.gp";
	string system_script = "gnuplot " + bench_script;

	if(fileExists(bench_script))
		remove(bench_script.c_str());

	if(Gnup::createFinalScript(bench_name, id))
	{
		remove(bench_script.c_str());
		return;
	}
	
	system(system_script.c_str());

	string move;

	if(fileExists("plot1.ps"))
	{
		if(_terminal == "ps")
		{
			move = "mv plot1.ps " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_1.ps";
		} else if(_terminal != "x11") {
			system("ps2pdf -dEPSCrop plot1.ps");
			system("pdfcrop plot1.pdf plot1-crop.pdf > /dev/null");
			move = "mv plot1-crop.pdf " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_1.pdf";
		}
		system(move.c_str());
		remove("plot1.ps");
		remove("plot1.pdf");
	}

	if(fileExists("plot2.ps"))
	{
		if(_terminal == "ps")
		{
			move = "mv plot2.ps " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_2.ps";
		} else if(_terminal != "x11") {
			system("ps2pdf -dEPSCrop plot2.ps");
			system("pdfcrop plot2.pdf plot2-crop.pdf > /dev/null");
			move = "mv plot2-crop.pdf " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_2.pdf";
		}
		system(move.c_str());
		remove("plot2.ps");
		remove("plot2.pdf");
	}

	if(fileExists("plot3.ps"))
	{
		if(_terminal == "ps")
		{
			move = "mv plot3.ps " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_3.ps";
		} else if(_terminal != "x11") {
			system("ps2pdf -dEPSCrop plot3.ps");
			system("pdfcrop plot3.pdf plot3-crop.pdf > /dev/null");
			move = "mv plot3-crop.pdf " + _result_dir + "/" + bench_name + "/" + bench_name + "_" + id + "_Gp" + "_3.pdf";
		}
		system(move.c_str());
		remove("plot3.ps");
		remove("plot3.pdf");
	}

	remove(bench_script.c_str());

	return;
}

void Gnup::plot()
{
	Gnup::plot(Gnup::getBenchName(),Gnup::getBenchId());
}

void Gnup::setUp(bool set_default)
{
	Gnup::setBenchName(findBenchName(_result_dir));
	Gnup::setBenchId(findBenchId(Gnup::getBenchName(), _result_dir));

	if(!set_default)
	{
		string userinput;
		cout << "Name of Benchmark?" << endl << "(default: " << Gnup::getBenchName() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Gnup::setBenchName(userinput);
			Gnup::setBenchId(findBenchId(Gnup::getBenchName(), _result_dir));
		}
		cout << "ID?" << endl << "(default: " << Gnup::getBenchId() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Gnup::setBenchId(userinput);
		}
	}
}

void Gnup::setUp(string bench_name)
{
	Gnup::setBenchName(bench_name);
	Gnup::setBenchId(findBenchId(Gnup::getBenchName(), _result_dir));
}

void Gnup::setUp(string bench_name, string id)
{
	Gnup::setBenchName(bench_name);
	Gnup::setBenchId(id);
}
