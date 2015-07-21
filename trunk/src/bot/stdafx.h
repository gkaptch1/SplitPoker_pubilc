// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _DLL_CPPLIB

#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <winsock.h>
#include <mysql.h>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

#include "../poker/stdafx.h"


/*
#include "../poker/poker.h"
#include "../poker/Card.h"
#include "../poker/utils.h"
#include "../poker/PokerHand.h"

#include "MsgQueue.h"
#include "TournamentData.h"
#include "HoldemTournamentAI.h"
#include "PlayerHoleCards.h"
#include "PlayerStats.h"
#include "main.h"
*/
//__import int __stdcall GetWindowTextA(struct HWND__ *,char *,int)


// TODO: reference additional headers your program requires here
