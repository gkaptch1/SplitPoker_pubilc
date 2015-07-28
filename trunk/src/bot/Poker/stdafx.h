// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <winsock.h>
#include <time.h>
//#include <mysql.h>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

#include "poker.h"
#include "Card.h"
#include "PokerHand.h"
#include "Deck.h"
#include "utils.h"
#include "ConfigParser.h"

// TODO: reference additional headers your program requires here
