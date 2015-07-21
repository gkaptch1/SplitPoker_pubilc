
#include "stdafx.h"
#include "MsgQueue.h"
#include "HoldemTournamentAI.h"
#include "Main.h"

/* Globals ********************************************************************/

char t[50];
char b[50];

MsgQueue table;

/* StatTracker() **************************************************************/
int StatTracker()
{
    srand(GetTickCount());

    // Prepare and Load Table
    HWND hwnd;
    EnumWindows(EnumWindowsProc_GetFirstWindow, (LPARAM)&hwnd);
    char buff[255]; memset(buff,0,sizeof(buff));
    GetWindowText(hwnd, buff, sizeof(buff));
	void* loc=(void*)((int)(buff)+39+3+conf.getStrValue("Moniker").length());

	cout << "---" << endl << buff << "---" << endl;
	cout << "---" << endl << (char*)(loc) << "---" << endl;

    int i=0;
    memset(b,0,50);
    memset(t,0,50);
    for (i=0; ((char*)loc)[i]!=' '; i++)
    {
        b[i]=((char*)loc)[i];
    }
    loc=(void*)((int)loc+i+1);
    for (i=0; ((char*)loc)[i]!='['; i++)
    {
        t[i]=((char*)loc)[i];
    }
    t[i-1]=0;

	cout << "b: " << b << "      t: " << t << endl;
    table.LoadTable("tmp",b,t);
	
	table.update();
	table.setPrintStyle(VIEW_DEFAULT);

	/* Clear Screen */
	cur(0,0);
	for (int i=0; i<10000; i++) cout << " ";
	cout << endl; 
	cur(0,0);

	/* play game */
	while (1)
    {
		table.update();
		table.print();
		table.getAI()->makePlay();

		Sleep(200);
    }

    return 0;
}