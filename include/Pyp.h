
#ifndef PYP_H
#define PYP_H

#include <stdlib.h>

#include <string>

#include "resultFileHelper.h"


class Pyp
{
public:
	Pyp();
	~Pyp();

	void plot();
	void plot(string resultDir, string pyScriptDir, string benchName, string id);

	void setUp(bool setDefault);

	string createFinalPyScript(string pyScriptBase, string resultFile);

	string getResultDir();
	string getPyScriptDir();
	string getBenchName();
	string getBenchId();

	void setResultDir(string resultDir);
	void setPyScriptDir(string pyScriptDir);
	void setBenchName(string benchName);
	void setBenchId(string id);

	
private:
	string _resultDir;
	string _pyScriptDir;

	string _benchName;
	string _id;
};



#endif //PYP_H