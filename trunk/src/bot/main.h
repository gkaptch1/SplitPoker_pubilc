#ifndef _main_h_
#define _main_h_

/* #defines *******************************************************************/

#define MAX_TABLES						10

#define basedir							((string)"c:\\poker\\")

/* Globals ********************************************************************/

//extern MsgQueue table[MAX_TABLES];
//extern int num_tables;

class MsgQueue;
class MsgQueueBase;

extern ConfigParser conf;

/* Function Declarations ******************************************************/
             
int RecordDaemon();   
int CompilePlayerStats();
int ResetPlayerStats();
int StatTracker();
void GameController();
BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam);
BOOL CALLBACK EnumWindowsProc_GetFirstWindow(HWND hwnd,LPARAM lParam);
BOOL CALLBACK EnumWindowsProc_GetMainWindow(HWND hwnd,LPARAM lParam);

#endif