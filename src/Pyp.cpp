#include <Python.h> // need to include this here and not in the .h file to avoid some macro overwriting warnings
#include "Pyp.h"
#include <dirent.h>


Pyp::Pyp()
{
	_resultDir = "./results";
	_pyScriptDir = "./benchmarks";
	_systemScriptDir = "./bencho/plotting";

	_benchName = "";
	_id = "";
}

Pyp::~Pyp()
{

}


void Pyp::plot()
{
	Pyp::plot(Pyp::getResultDir(), Pyp::getPyScriptDir(), Pyp::getBenchName(), Pyp::getBenchId());
}

void Pyp::plot(string resultDir, string pyScriptDir, string benchName, string benchId)
{
	vector<string> resultPlots;
	string resultFile = getResultFile(benchName, benchId, resultDir);
	string pyScript = pyScriptDir + "/" + benchName + ".py";

	// check if we actually need to "work" in the script
	string pyScriptFinal = Pyp::createFinalPyScript(pyScript, resultFile);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	// ToDo: Check first if file exists
	Pyp::callPythonPlot(resultFile, pyScriptFinal);

	//get plotted files and crop them
	struct dirent *de=NULL;
  	DIR *d=NULL;
	string benchmarksResultDir = resultDir + "/" + benchName;
	string resultPrefix = benchName + "_" + benchId + "_Py_";
	d = opendir(benchmarksResultDir.c_str());
	while((de = readdir(d))) {
		if (strncmp(resultPrefix.c_str(), de->d_name, resultPrefix.size()) == 0) {
			pdfCropFile(benchmarksResultDir + "/" + de->d_name);
		}
    }
    closedir(d);
}

void Pyp::callPythonPlot(string resultFile, string scriptFile)
{
	vector<string> resultPlots;
	string systemScript = Pyp::getSystemScriptDir() + "/system.py";

	string command = "python " + systemScript + " -f " + resultFile + " -s " + scriptFile;

	system(command.c_str());
}

void Pyp::setUp(bool setDefault)
{
	Pyp::setBenchName(findBenchName(Pyp::getResultDir()));
	Pyp::setBenchId(findBenchId(Pyp::getBenchName(), Pyp::getResultDir()));

	if(!setDefault)
	{
		string userinput;
		cout << "Name of Benchmark?" << endl << "(default: " << Pyp::getBenchName() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Pyp::setBenchName(userinput);
			Pyp::setBenchId(findBenchId(Pyp::getBenchName(), Pyp::getResultDir()));
		}
		cout << "ID?" << endl << "(default: " << Pyp::getBenchId() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Pyp::setBenchId(userinput);
		}
	}
}

string Pyp::createFinalPyScript(string pyScriptBase, string ResultFile)
{
	string pyScriptFinal = pyScriptBase;
	return pyScriptFinal;
}


string Pyp::getResultDir()
{
	return _resultDir;
}

string Pyp::getPyScriptDir()
{
	return _pyScriptDir;
}

string Pyp::getSystemScriptDir()
{
	return _systemScriptDir;
}

string Pyp::getBenchName()
{
	return _benchName;
}

string Pyp::getBenchId()
{
	return _id;
}

void Pyp::setResultDir(string resultDir)
{
	_resultDir = resultDir;
}

void Pyp::setPyScriptDir(string pyScriptDir)
{
	_pyScriptDir = pyScriptDir;
}

void Pyp::setSystemScriptDir(string systemScriptDir)
{
	_systemScriptDir = systemScriptDir;
}

void Pyp::setBenchName(string benchName)
{
	_benchName = benchName;
}

void Pyp::setBenchId(string id)
{
	_id = id;
}