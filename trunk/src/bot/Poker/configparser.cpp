
#include "stdafx.h"
#include "ConfigParser.h"
#include <algorithm>
#include <ctype.h>

/* Constructor / Destructor **************************************************/

ConfigParser::ConfigParser()
{
	memset(this, 0, sizeof(ConfigParser));
}

ConfigParser::ConfigParser(char* file)
{
	cout << "Loading settings... ";
	memset(this, 0, sizeof(ConfigParser));
	parseFile(file);
	cout << "done." << endl;
}

ConfigParser::~ConfigParser()
{
}

/* ConfigOpt operators *******************************************************/

bool operator < (const ConfigOpt& v1, const ConfigOpt& v2)
{
	return (v1.name<v2.name);
}

/* ConfigParser::addVar() ****************************************************/
void ConfigParser::addVar(const char* name, const char* v)
{
	if (string(name)==""||string(v)=="") {
		return;
	}

	ConfigOpt tmp;
	tmp.name=killWhite(name);
	tmp.sVal=killWhite(v);


	char* tmpchar;
	tmp.dVal=strtod(v,&tmpchar);

	if (tmp.sVal=="false")	{ tmp.dVal=0; }
	if (tmp.sVal=="f")		{ tmp.dVal=0; }
	if (tmp.sVal=="true")	{ tmp.dVal=1; }
	if (tmp.sVal=="t")		{ tmp.dVal=1; }

	int varIndex=getVarIndex(tmp.name.c_str());
	if (varIndex!=-1) {
		conf[varIndex]=tmp;
	}
	conf.push_back(tmp);
}

/* ConfigParser::getRealValue() **********************************************/
double ConfigParser::getRealValue(const char* v)
{
	string var=v;
	transform(var.begin(),var.end(),var.begin(),::tolower);

	for (unsigned int i=0; i<conf.size(); i++) {
		if (conf[i].name==var) {
			return conf[i].dVal;
		}
	}

	return 0;
}

/* ConfigParser::getStrValue() ***********************************************/
string ConfigParser::getStrValue(const char* v)
{
	string var=v;
	transform(var.begin(),var.end(),var.begin(),::tolower);

	for (unsigned int i=0; i<conf.size(); i++) {
		if (conf[i].name==var) {
			return conf[i].sVal;
		}
	}

	return "";
}

/* ConfigParser::varExist() **************************************************/
bool ConfigParser::varExist(const char* var)
{
	for (unsigned int i=0; i<conf.size(); i++) {
		if (conf[i].name==var) {
			return true;
		}
	}

	return false;
}

/* ConfigParser::getVarIndex() ***********************************************/
int ConfigParser::getVarIndex(const char* var)
{
	for (unsigned int i=0; i<conf.size(); i++) {
		if (conf[i].name==var) {
			return i;
		}
	}

	return -1;
}

/* ConfigParser::isTermChar() ************************************************/
bool ConfigParser::isTermChar(char c)
{
	switch (c)
	{
	case '\0':
	case '\n':
	case ';':
	case '#':
		return true;
	}

	return false;
}


/* ConfigParser::killWhite() **************************************************/
string ConfigParser::killWhite(string s)
{
	string ret="";

	bool quoted=false;

	for (unsigned int i=0; i<s.length(); i++) {
		if (!quoted) {
			switch (s[i])
			{
			case ' ':
			case '\t':
			case '\n':
				break;

			case '\"':
				quoted=true;
				break;

			default:
				ret+=s[i];
			}
		}
		else {
			if (s[i]=='\"'){
				quoted=false;
			}
			else {
				ret+=s[i];
			}
		}
	}

	transform(ret.begin(),ret.end(),ret.begin(),::tolower);

	return ret;
}

/* ConfigParser::parseCmd() **************************************************/
void ConfigParser::parseCmd(const char *cmd)
{
	string var,val;
	int eq=0;
	int len=(int)strlen(cmd);

	for (int i=0; i<len; i++) {
		if (cmd[i]=='=') {
			var=(string(cmd)).substr(0,i);
			eq=i;
		}
		if (isTermChar(cmd[i+1])) {
			val=(string(cmd)).substr(eq+1,i-eq);
			break;
		}
	}

	addVar(var.c_str(), val.c_str());
}

/* ConfigParser::parseCmdLine() **********************************************/
void ConfigParser::parseCmdLine(int num, char** argv)
{
	for (int i=1; i<num&&argv[i]!=NULL; i++) {
		if (argv[i][0]=='-'&&argv[i][1]=='-') {
			argv[i][0]=' ';
			argv[i][1]=' ';
			parseCmd(argv[i]);
		}
	}
}

/* ConfigParser::parseFile() *************************************************/
void ConfigParser::parseFile(const char *file)
{
	ifstream in(file);
	char buff[512];

	while (!in.eof()) {
		memset(buff, 0, sizeof(buff));
		in.getline(buff, 512);
		parseCmd(buff);
	}
}

/* ConfigParser::dumpTable() *************************************************/
void ConfigParser::dumpTable(ostream& out)
{
	out << "Config Parser: " << endl;

	for (unsigned int i=0; i<conf.size(); i++) {
		out << "  " << conf[i].name << "\t = " << conf[i].sVal << "\t = " << conf[i].dVal << endl;
	}
}