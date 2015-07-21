
#include "stdafx.h"

#include "Main.h"
#include "MsgQueue.h"
#include "HoldemTournamentAI.h"

ConfigParser conf("C:\\poker\\pp.conf");
MsgQueue tables[MAX_TABLES];
int numtables=0;

HWND rvp;

#define _CRT_SECURE_NO_DEPRECATE

#define click_killMOTD				MouseClick(GetSystemMetrics(SM_CXSCREEN)/2,GetSystemMetrics(SM_CYSCREEN)/2+260,true);
#define click_buyin					click_killMOTD; Sleep(100); MouseClick(485,230);
#define click_scrollup				click_killMOTD; Sleep(100); MouseClick(577,225);
#define click_scrolldown			click_killMOTD; Sleep(100); MouseClick(577,520);
#define click_scrollreset			click_killMOTD; Sleep(100); for (int i=0; i<20; i++) { MouseClick(577,236); Sleep(10); } for (int i=0; i<20; i++) { click_scrollup; Sleep(10); }

#define click_opentable(x)			click_killMOTD; Sleep(100); MouseClick(240,240+18*x); Sleep(10); MouseClick(240,240+18*x);

/* MouseClick() ***************************************************************/
void MouseClick(int x, int y, bool raw=false)
{	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	SetForegroundWindow(rvp);

	if (!raw) {
		x+=2;
		y+=29;
		WINDOWINFO inf;
		memset(&inf,0,sizeof(inf));
		GetWindowInfo(rvp,&inf);
		x+=inf.rcWindow.left;
		y+=inf.rcWindow.top;
	}

    INPUT in;
    memset(&in,0,sizeof(in));
    in.type=INPUT_MOUSE;
    in.mi.dx=int(x*(65535.0/cx));
    in.mi.dy=int(y*(65535.0/cy));
    in.mi.dwFlags=MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;

	SendInput(1,&in,sizeof(in));
    in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP;
    SendInput(1,&in,sizeof(in));
}

/* EnumWindowsProc_LoadTables() ***********************************************/
BOOL CALLBACK EnumWindowsProc_LoadTables(HWND hwnd,LPARAM lParam)
{
    char buff[255]; memset(buff,0,sizeof(buff));
    GetWindowTextA(hwnd, buff, sizeof(buff));

//    if (strncmp(buff, "Royal Vegas Poker", 10)==0)
//    if (strncmp(buff, "PokerTime", 10)==0)
    if (strncmp(buff, "Poker Time", 10)==0)					// modified 26Feb2010
    {
		cout << buff << endl;

// don't continue if it's the lobby

//        if (strncmp(buff, "Royal Vegas Poker - (", 21)!=0)
//        if (strncmp(buff, "PokerTime - (", 21)!=0)
        if (strncmp(buff, "Poker Time - Hold'em (", 21)!=0)					// modified 26Feb2010
        {
			rvp=hwnd;
            return TRUE;
        }
		else
		{
			cout << "                 awesome!!!" << endl;
		}

		char buff[255]; 
		memset(buff,0,sizeof(buff));
		GetWindowTextA(hwnd, buff, sizeof(buff));
//		void* loc=(void*)((int)(buff)+48+tables[0].getAI()->getAIMoniker().length());
		
		void* loc=(void*)((int)(buff)+39+3+conf.getStrValue("Moniker").length());

		int i=0;
		char b[50]; memset(b,0,sizeof(b));
		char t[50]; memset(t,0,sizeof(t));

		for (i=0; ((char*)loc)[i]!=' '; i++)
		{
			b[i]=((char*)loc)[i];
		}
		b[i+1]=0;
		loc=(void*)((int)loc+i+1);
		for (i=0; ((char*)loc)[i]!='['; i++)
		{
			t[i]=((char*)loc)[i];
		}
		t[i-1]=0;

		cout << "b: " << b << "      t: " << t << endl;

		tables[numtables].LoadTable("tmp",b,t);
while(1);
		numtables++;
    }

    return TRUE;
}

/* GameController() ***********************************************************/
void GameController()
{
	/* Setup **************************/
	int starttime	= (int)conf.getRealValue("timestart"),
		endtime		= (int)conf.getRealValue("timeend"),
		nowtime;

    srand(GetTickCount());
	numtables=0;

	/* Start AI */
    EnumWindows(EnumWindowsProc_LoadTables, 0);

	/* Open tables if necessary */
	if (numtables==0) 
	{
		if (rvp==0) {
			WinExec("\"C:\\program files\\royalvegasMPP\\mppoker.exe\"",SW_SHOW);
		}

		for (int i=0; i<20; i++) {
			cur(0,0);
			cout << i << endl;
			Sleep(1000);
		}

		if (rvp==0) {
		    EnumWindows(EnumWindowsProc_LoadTables, 0);
		}

		cout << endl << "hwnd: " << (void*)rvp << endl;

		click_killMOTD;
		Sleep(1000);
		click_buyin;
		click_scrollreset;

		click_opentable(0); Sleep(5000);
		click_opentable(1); Sleep(5000);
		click_opentable(2); Sleep(5000);
		click_opentable(3); Sleep(5000);

		EnumWindows(EnumWindowsProc_LoadTables, 0);
	}

	for (int i=0; i<numtables; i++) {
		tables[i].setPrintStyle(VIEW_LIGHT);
//		tables[i].setPrintStyle(VIEW_PLAYER);
	}

	/* Clear Screen */
	cur(0,0);
	for (int i=0; i<10000; i++) cout << " ";
	cout << endl; 
	cur(0,0);

	/* Record Program Load Time */
	char buff[40];
	_strtime(buff);

	int loadtime = 
		(buff[0]-'0')*1000+
		(buff[1]-'0')*100+
		(buff[3]-'0')*10+
		(buff[4]-'0')*1;

	/* AI Loop ************************/
	while (1)
	{
		int starttime=timeGetTime();
		cur(0,0);

		/* AllowPlay? */
		char buff[40];
		_strtime(buff);

		nowtime = 
			(buff[0]-'0')*1000+
			(buff[1]-'0')*100+
			(buff[3]-'0')*10+
			(buff[4]-'0')*1;

		bool allow=true;

		if (conf.getRealValue("checktime")) {
			allow=false;
			int starttime=(int)conf.getRealValue("timestart");
			int endtime=(int)conf.getRealValue("timeend");
			if (starttime<endtime) {
				if (starttime<=nowtime&&endtime>=nowtime) {
					allow=true;
				}
			}
			else {
				if (starttime>=nowtime&&endtime<=nowtime) {
					allow=true;
				}
			}
		}

		if (conf.getRealValue("playfortime")!=0) {
			int stoptime=(int)conf.getRealValue("playfortime")+loadtime;
			if ((stoptime%100)/60==1) {
				stoptime-=60;
				stoptime+=100;
			}
			if (stoptime<=nowtime) {
				allow=false;
			}
		}

		if (conf.getRealValue("observer")) {
			allow=false;
		}

		cout << "Allow new games (" << nowtime << "): " << allow << endl << endl;

		tables[0].printLightHeader();

		/* Update Tables */
		for (int i=0; i<MAX_TABLES&&i<numtables; i++) {
			cout << i << endl;
			tables[i].getAI()->setWatchingOnly(false);
			tables[i].toggleAllowPlay(allow);
			if (!tables[i].tableGood()) {
				continue;
			}

			/* bring the table to the foreground; start a timer to see how long AI engine takes */
			tables[i].loadTable();

			/* read the table to see if anything has changed */
			tables[i].update();

			/* print the table's status; display type ("play"/"debug"/etc) controlled by tables[i].setPrintType() above */
			cur (0, 4+i);
			tables[i].print();

			/* ask the AI for its move; will actually make the move if tables[i].allowPlay==true */
			tables[i].getAI()->makePlay();

			/* stop AI engine timer */
			tables[i].closeTable();

			tables[i].printPlayers(40,23);
/*			cur(0,12) << "isEveryPlayerAllin(): ";
			if (tables[i].isEveryPlayerAllin()) {
				cout << "true  " << endl;
			}
			else {
				cout << "false " << endl;
			}
*/		}

		cout << endl;
		cur(0,18);
		cout << "TIME: " << timeGetTime()-starttime << "\t" << endl;

		Sleep(200);
	}
}