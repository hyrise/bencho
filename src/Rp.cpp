#include "Rp.h"


Rp::Rp()
{
	_resultDir = "./results";
	_rScriptDir = "./benchmarks";
	_systemScriptDir = "./bencho/plotting";

	_benchName = "";
	_id = "";
}

Rp::~Rp()
{

}


void Rp::plot()
{
	Rp::plot(Rp::getResultDir(), Rp::getRScriptDir(), Rp::getBenchName(), Rp::getBenchId());
}

void Rp::plot(string resultDir, string rScriptDir, string benchName, string benchId)
{
	string resultFile = getResultFile(benchName, benchId, resultDir);
	string rScript = rScriptDir + "/" + benchName + ".r";

	// check if we actually need to "work" in the script
	string rScriptFinal = Rp::createFinalRScript(rScript, resultFile);

	cout << "Benchmark: \"" + benchName + "\", ID: " + benchId << endl;

	// ToDo: Check first if file exists
	Rp::callRPlot(resultFile, rScriptFinal);

	for(int i = 1; i <= 5; ++i)
	{
		string plotFile = resultDir + "/" + benchName + "/" + benchName + "_" + benchId + "_R_" + convertInt(i) + ".pdf";
		if(fileExists(plotFile))
			pdfCropFile(plotFile);
	}
}

void Rp::callRPlot(string resultFile, string scriptFile)
{
	string systemScript = Rp::getSystemScriptDir() + "/system.r";

	string command = "R CMD BATCH --no-save '--args " + resultFile + " " + scriptFile + "' " + systemScript + " /dev/null"; // redirect console output to /dev/null

	system(command.c_str());
}

void Rp::setUp(bool setDefault)
{
	Rp::setBenchName(findBenchName(Rp::getResultDir()));
	Rp::setBenchId(findBenchId(Rp::getBenchName(), Rp::getResultDir()));

	if(!setDefault)
	{
		string userinput;
		cout << "Name of Benchmark?" << endl << "(default: " << Rp::getBenchName() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Rp::setBenchName(userinput);
			Rp::setBenchId(findBenchId(Rp::getBenchName(), Rp::getResultDir()));
		}
		cout << "ID?" << endl << "(default: " << Rp::getBenchId() << ")" << endl;
		getline(cin, userinput);
		if(!userinput.empty())
		{
			Rp::setBenchId(userinput);
		}
	}
}


string Rp::createFinalRScript(string rScriptBase, string ResultFile)
{
	string rScriptFinal = rScriptBase;
	return rScriptFinal;
}


string Rp::getResultDir()
{
	return _resultDir;
}

string Rp::getRScriptDir()
{
	return _rScriptDir;
}

string Rp::getSystemScriptDir()
{
	return _systemScriptDir;
}

string Rp::getBenchName()
{
	return _benchName;
}

string Rp::getBenchId()
{
	return _id;
}

void Rp::setResultDir(string resultDir)
{
	_resultDir = resultDir;
}

void Rp::setRScriptDir(string rScriptDir)
{
	_rScriptDir = rScriptDir;
}

void Rp::setSystemScriptDir(string systemScriptDir)
{
	_systemScriptDir = systemScriptDir;
}

void Rp::setBenchName(string benchName)
{
	_benchName = benchName;
}

void Rp::setBenchId(string id)
{
	_id = id;
}