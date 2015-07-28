
#ifndef _ConfigParser_H_
#define _ConfigParser_H_

#include <string.h>
#include <iostream>
using namespace std;

/* structs *******************************************************************/
struct ConfigOpt
{
	string name;

	double dVal;
	string sVal;
};

/* class ConfigParser ********************************************************/
class ConfigParser
{
private:
	vector<ConfigOpt> conf;

public:
	ConfigParser();
	ConfigParser(char* file);
	~ConfigParser();

	void parseCmdLine(int argc, char** argv);
	void parseFile(const char* file);
	void parseCmd(const char* cmd);
	void dumpTable(ostream& out);

	void addVar(const char* name, const char* v);

	double getRealValue(const char* var);
	string getStrValue(const char* var);

	bool varExist(const char* var);
	int getVarIndex(const char* var);

	bool isTermChar(char c);
	string killWhite(string s);
};

#endif