#include "Pyp.h"


Pyp::Pyp()
{
	_resultDir = "./results";
	_pyScriptDir = "./benchmarks";

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
	string resultFile = getResultFile(benchName, benchId, resultDir);
	string pyScript = pyScriptDir + "/" + benchName + ".py";
	string pyScriptFinal = Pyp::createFinalPyScript(pyScript, resultFile);


	// ToDo: Check first if file exists
	string pyScriptExecutionCommand = "python " + pyScriptFinal + " -f " + resultFile + " -n " + benchName;
	system(pyScriptExecutionCommand.c_str());
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

void Pyp::setBenchName(string benchName)
{
	_benchName = benchName;
}

void Pyp::setBenchId(string id)
{
	_id = id;
}