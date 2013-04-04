
#ifndef RP_H
#define RP_H

#include <stdlib.h>

#include <string>

#include "resultFileHelper.h"


class Rp
{
public:
	Rp();
	~Rp();

	void plot();
	void plot(string resultDir, string rScriptDir, string benchName, string id);
	void callRPlot(string resultFile, string scriptFile);

	void setUp(bool setDefault);

	string createFinalRScript(string rScriptBase, string resultFile);

	string getResultDir();
	string getRScriptDir();
	string getSystemScriptDir();
	string getBenchName();
	string getBenchId();

	void setResultDir(string resultDir);
	void setRScriptDir(string rScriptDir);
	void setSystemScriptDir(string systemScriptDir);
	void setBenchName(string benchName);
	void setBenchId(string id);

	
private:
	string _resultDir;
	string _rScriptDir;
	string _systemScriptDir;

	string _benchName;
	string _id;

};



#endif //RP_H