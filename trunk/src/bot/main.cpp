
/* #includes ******************************************************************/

#include "stdafx.h"
#include "main.h"

#include "MsgQueue.h"


/* main() *********************************************************************/
int main(int argc, char **argv)
{
	/* initialize general settings */
	conf.parseFile("c:\\poker\\pp.conf");
	conf.parseCmdLine(argc, argv);
	srand(GetTickCount());

	/* select and load application */
	string cmd;
	if (argc>=2) {
		cmd=argv[1];
	}
//	if (cmd!="resetdb") cmd="proc";

	if      (cmd=="proc")		{ CompilePlayerStats(); }
	else if (cmd=="resetdb")	{ ResetPlayerStats(); }
	else if (cmd=="debug")		{  conf.addVar("Observer","true"); StatTracker(); }
//	else if (cmd=="watchold")	{ RecordDaemon(); }
	else {
		if (cmd=="watch") conf.addVar("Observer","true");
		GameController();
	}

	while (1) Sleep(500);
    return 0;
}