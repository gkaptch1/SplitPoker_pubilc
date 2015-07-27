
/* #includes ******************************************************************/
#include "stdafx.h"

#include "MsgQueue.h"
#include "HoldemTournamentAI.h"
#include "TournamentData.h"

/* #defines *******************************************************************/

//#define PRINTEACHLINE

#define _CRT_SECURE_NO_DEPRECATE

#define MAXOCRPOINTS                10
#define GC(a,b)                     ((GetBValue(GetColor(x+a,y+b))<100))
#define GCV(a,b,c)                  ((GetBValue(GetColor(x+a,y+b))<c))
//((GetBValue(GetColor(x+a,y+b))<100)||(GetRValue(GetColor(x+a,y+b))>200&&GetBValue(GetColor(x+a,y+b)<100))

/* Constructer / Destructer ***************************************************/

MsgQueue::MsgQueue()
{ 
	/* initialize data variables */
    msg_rec			= 0;
    file_input		= false;
    RVPhwnd			= 0;
    textbox			= 0;
            
    filedesc		= NULL;
    nextline		= 0;

    eliminations	= 0;
	playerOffset	= -1;

	seated			= false;
    for (int i=0; i<10; i++) players[i]=NULL;

	gameTime		= getCurrentTime();
	gameDate		= getCurrentDate();

	reset();

	/* create interfaces */
    ai=new HoldemTournamentAIManual(this);
	db=new TournamentData(this);
}

MsgQueue::~MsgQueue()
{
/*    delete ai;
	delete db;

	for (int i=0; i<11&&players[i]!=NULL;i++) delete players[i];

	if (filedesc!=NULL) 
	{
		fclose(filedesc);
		filedesc=NULL;
	}*/
}

ostream& operator << (ostream &out, PlayerAction &p) 
{
	out << p.player << ": ";
	
	switch (p.action)
	{
	case FOLD:			out << "fold";	break;
	case CALL:			out << "call";	break;
	case CHECK:			out << "check";	break;
	case BET:			out << "bet";	break;
	case RAISE:			out << "raise";	break;
	case ALLIN:			out << "allin";	break;
	case SHOW:			out << "show";	break;
	case STAGE_CHANGE:	out << "stage " << (char)p.player;	break;
	}

	out << "," << p.amt << " ";
	
	if (p.analyzed) out << "analyzed"; 
		
	return out << endl; 
}

/* MsgQueue::ClearSNGGames() **************************************************/
void MsgQueue::ClearSNGGames()
{
	db->eraseDB();
}

/* MsgQueue::reset() **********************************************************/
void MsgQueue::reset()
{
	for (int i=0; i<20; i++) {
		if (players[i]!=NULL) {
			delete players[i];
			players[i]=NULL;
		}
	}
	delete ai; ai=new HoldemTournamentAIManual(this);
	delete db; db=new TournamentData(this);

	memset(&status,0,sizeof(status));
	memset(&events,0,sizeof(events));

	eliminations=0;
	playerOffset=0;

    events.newgame=true; 
    memset(&status,0,sizeof(status));
    status.bb=20;
    status.livingplayers=10;

	gameTime		= getCurrentTime();
	gameDate		= getCurrentDate();

//	msg_rec=0;
//	SendMessage(textbox,WM_SETTEXT,5,(LPARAM)"" );
}

/* MsgQueue::ParseFile() ******************************************************/
int MsgQueue::ParseFile(const char* file)
{
	// open file
	ifstream in(file);
	if (!in) {
		cout << "ERROR: Cannot open %s for parsing." << endl;
		return 0;
	}	
	
	// initialize table for analysis mode
	setDryRun(true);
	db->enableWrite();
	parseTableInfoFromFile(file);

	ifstream gin((basedir+"gameid.txt").c_str());
	gin >> gameid;
	gameid++;
	ofstream out((basedir+"gameid.txt").c_str());
	out << gameid;

	// Parse the file
	char msgNow[4000];  memset(msgNow,0,sizeof(msgNow));
	char msgNext[4000]; memset(msgNext,0,sizeof(msgNext));
	while (in) {
		strcpy(msgNow,msgNext);
		do {
			in.getline(msgNext,sizeof(msgNext));
			processErrorMessage(msgNext);
		} while(msgNext[0]!='>'&&in);
		ProcessMessage(msgNow,msgNext);
	}

	db->RegisterGame(file);
	return 0;
}

/* MsgQueue::parseTableInfoFromFile() *****************************************/
void MsgQueue::parseTableInfoFromFile(const char* file)
{
	bool firstPass=true;
	string date,time;
	string* cur=NULL;
	for (int i=0; file[i]!=0; i++) {
		if (file[i]=='\\') {
			table.clear();
			cur=&table;
			firstPass=true;
			continue;
		}
		else if (file[i]=='-') {
			if (firstPass==true) {
				date.clear();
				cur=&date;
				firstPass=false;
				i++;
				continue;
			}
		}
		else if (file[i]=='@') {
			time.clear();
			cur=&time;
			firstPass=false;
			i++;
			continue;
		}
		if (cur!=NULL) {
			char buff[2];
			buff[0]=file[i];
			buff[1]=0;
			cur->append(buff);
		}
	}
	table=table.substr(0,table.size()-1);
	date=date.substr(0,date.size()-1);
	date=date.substr(6,2)+"-"+date.substr(0,5);
	for (unsigned int i=0; i<time.size(); i++) {
		if (time[i]=='-') {
			time[i]=':';
		}
	}
	gameDate=date;
	gameTime=time;
}

/* MsgQueue::processErrorMessage() ********************************************/
void MsgQueue::processErrorMessage(const char* msg)
{
	if (msg[0]=='*') {
		if (strncmp(msg,"*MINUS",5)==0) {
			db->registerError(ERR_MINUS);
		}
		if (strncmp(msg,"*DOUBLE",5)==0) {
			db->registerError(ERR_DOUBLE);
		}
		if (strncmp(msg,"*stayActive",5)==0) {
			db->registerError(ERR_NOPLAY);
		}
	}
}

/* MsgQueue::getCurrentDate() *************************************************/
string MsgQueue::getCurrentDate()
{	
	string ret;
	char buff[400]; 
	memset(buff,0,sizeof(buff));
	_strdate(buff);
	char date[400];
	memset(date,0,sizeof(date));
	date[0]='2';
	date[1]='0';
	date[2]=buff[6];
	date[3]=buff[7];
	date[4]='-';
	date[5]=buff[0];
	date[6]=buff[1];
	date[7]='-';
	date[8]=buff[3];
	date[9]=buff[4];
	date[10]=0;
	ret=date;
	return ret;
}

/* MsgQueue::getCurrentTime() *************************************************/
string MsgQueue::getCurrentTime()
{
	string ret;
	char time[400]; 
	_strtime(time);
	ret=time;
	return ret;
}

/* MsgQueue::loadTable() ******************************************************/
void MsgQueue::loadTable()
{	
	starttime=timeGetTime();
	hwndFront=GetForegroundWindow();
	SetForegroundWindow(RVPhwnd);
	RVPhdc=GetDC(RVPhwnd);
	Sleep(200);
	rounds++;
}

/* MsgQueue::closeTable() *****************************************************/
void MsgQueue::closeTable()
{	
//	if (hwndFront!=NULL) SetForegroundWindow(hwndFront);
	totaltime+=timeGetTime()-starttime;
}

/* MsgQueue::update() *********************************************************/
void MsgQueue::update()
{
	if (!seated) {
		takeSeat();
	}
	stayActive();
	CalcPlayerOffset();

/*	if (getNewHand()/*&&getNumHands()>1/*&&getActionPlayer()==ai->getAIPlayer()*) {
		cur (0,0);
		cout << "Dealing Cards..." << endl;
		PokerHand h=GetHoleCardsOnTable(ai->getAIPlayer());

		cur(0,0);
		cout << "Waiting for deal. " << /*i <<* endl;
		h=GetHoleCardsOnTable(ai->getAIPlayer());
//		Sleep(200);

		status.cards[0]=h[0];
		status.cards[1]=h[1];
	}*/

	ParseNewMessages();
	updateHoleCards();

/*	for (int i=0; i<10; i++)
		holeCards[i]=GetHoleCardsOnTable(i);

	PokerHand h=GetHoleCardsOnTable(ai->getAIPlayer());
	if (h[0]!=NOCARD&&h[1]!=NOCARD) {
		status.cards[0]=h[0];
		status.cards[1]=h[1];
	}
*/
	/* do AI stuff */
	ai->update();
}

/* MsgQueue::updateHoleCards() ************************************************/
void MsgQueue::updateHoleCards()
{	
	for (int i=0; i<10; i++) {
		holeCards[i]=GetHoleCardsOnTable(i);
		if (holeCards[i][0]!=NOCARD) {
			status.cards[0]=holeCards[i][0];
			status.cards[1]=holeCards[i][1];
		}
	}

/*	PokerHand h=GetHoleCardsOnTable(ai->getAIPlayer());
	if (h[0]!=NOCARD&&h[1]!=NOCARD) {
		status.cards[0]=h[0];
		status.cards[1]=h[1];
	}*/
}

/* MsgQueue::ProcessMessage() *************************************************/
int MsgQueue::ProcessMessage(char* msg, char* nextline)
{       
}

/* MsgQueue::RegisterAction() *************************************************/

int MsgQueue::RegisterAction(char* command, char* name)
{
	bool allin=false;
	events.action=true;
	int nameindex=getNameIndex((char*)name);
	status.active=nameindex;
	int cmdoffset=0;

    if (strncmp((char*)command,"bet",3)==0)
	{
		players[nameindex]->bet();
		cmdoffset=8;
		status.lastaction=BET;
	}
	else if (strncmp((char*)command,"called",6)==0)
	{
        players[nameindex]->call();  
		cmdoffset=11;
		status.lastaction=CALL;
	}
    else if (strncmp((char*)command,"raised",6)==0)
    {       
        players[nameindex]->raise();  
		cmdoffset=11;
		status.lastaction=RAISE;
	}
    else if (strncmp((char*)command,"went all-in",8)==0)
	{
        status.allins++;
        players[nameindex]->allin();
		cmdoffset=16;
		allin=true;
		status.lastaction=ALLIN;
	}
    else if (strncmp((char*)command,"checked",7)==0)
    {       
        players[nameindex]->check();         
		status.lastaction=CHECK;
	}
    else if (strncmp((char*)command,"folded",6)==0||strncmp((char*)command,"did not act",6)==0)
    {        
        players[nameindex]->fold();       
		status.lastaction=FOLD;
    }
    else if (strncmp((char*)command,"shows",5)==0)
    {     
		status.lastaction=SHOW;
		int i;
		for (i=0; i<100&&command[i]!='('; i++);i++;
		
		Card hand[5];
		for (int j=0; j<5;j++)
		{
			if (((char*)command)[i]=='1') i++;
			char v=((char*)command)[i++];
			char s=((char*)command)[i++];
			switch (s)
			{
				case 'ª': s='s'; break; //spads
				case '©': s='h'; break; //hearts
				case '§': s='c'; break; //clubs
				case '¨': s='d'; break; //Diamonds
			}
			hand[j].SetCard(s,v);
		}

		int matches=0;
		for (int j=0; j<5; j++)
		{
			int dif=1;
			for (int k=0; k<7; k++)
			{
				if (hand[j]!=status.cards[k])
					dif*=1;
				else
					dif*=0;
			}
			if (dif==1)
			{
				status.cards[matches++]=hand[j];
//				hand[j].Print();
			}
		}
    }

	PushAction(status.lastaction,nameindex);	
	db->RegisterHand();  // Must combine last hand's status (changes below) with current hand (changes above) ie this line goes in the middle

	char* tmp=(char*)command+cmdoffset;
	char buff[10];
	for (int i=0,j=0; i<10; i++){if (tmp[i]!=','){if (tmp[i] >= '0' && tmp[i] <= '9'){buff[j]=tmp[i];}else{buff[i]=0;break;}j++;}}
	int amt=atoi(buff);
	status.pot+=amt;  
	players[nameindex]->stack-=amt;    
	status.bet[nameindex]+=amt;

	status.active=getNextActivePlayer(nameindex);

	if (allin)
	{
        int max=0;
        for (int i=0; i<10; i++)
        {
            if (status.bet[i]>max&&i!=nameindex) max=status.bet[i];
        }
        status.extrabet=abs(-amt+max);
	}

	return 0;
}


/* MsgQueue::PushAction() *****************************************************/
void MsgQueue::PushAction(int loc, int player, int amt, bool analyzed)
{
	PlayerAction action;
	action.action=loc;
	action.player=player;
	action.amt=amt;
	action.analyzed=analyzed;
	ai->pushAction(action);
}

/* MsgQueue::getNameIndex() ***************************************************/
int MsgQueue::GetNameIndexTrue(int n)
{
    return (10+n-playerOffset+0)%10;
}

/* MsgQueue::LoadTable() ******************************************************/
int MsgQueue::LoadTable(char* buy, char* b, char* t)
{
    char str[512];
	memset(str,0,sizeof(str));
    str[0]=0;

//    strcat(str, ("Royal Vegas Poker - (10 Seat Table) - "+ai->getAIMoniker()+" - Blinds ").c_str()); // Old table info
//    strcat(str, ("PokerTime - (10 Seat Table) - "+ai->getAIMoniker()+" - Blinds ").c_str());
    strcat(str, ("Poker Time - Hold'em (10 Seat Table) - "+ai->getAIMoniker()+" - ").c_str());
    strcat(str, b);
    strcat(str, " ");
    strcat(str, t);
    strcat(str, " [Hold'Em Real Money No Limit]");

	cout << "---" << endl << str << "---" << endl;

    strcat(blinds,b);
    table=t;
	strcat(buyin,db->getBuyinFromName(table).c_str());
	if (db->getBuyinFromName(table)=="unknown") {
		memset(buyin,0,sizeof(buyin));
		strcat(buyin,"unkn");
	}

    RVPhwnd = FindWindow(NULL, str);
    textbox = GetDlgItem(RVPhwnd, 101);


    // Used for debugging against new program versions

    for (int i=0; i<256*256; i++) {
        HWND tmp=GetDlgItem(RVPhwnd, i);
        if (tmp!=0) printf("%i\n",i);
    }
    printf("%s, %s, %s, %i, %i\n", b, t, str, RVPhwnd, textbox);

    if (RVPhwnd != NULL && textbox != NULL) {
        printf("Connected on table %s.\n", t);
    }
    else {
        printf("Could not connect to %s.  Check Blinds structures.\n", table);
        return 0;
    }
    return 1;
}

/* MsgQueue::LoadDumpFile() ***************************************************/
int MsgQueue::LoadDumpFile()
{                
	reset();

    char File[512];
    char time[32];
    char date[32];

    _strtime(time);
    _strdate(date);

    for (int i=0; i<32; i++) {
        if (date[i]=='/') date[i]='-';
        if (time[i]==':') time[i]='-';
    }

	filename="C:\\poker\\GameArchive\\tmp\\";
	filename+=table;
	filename+=" - ";
	filename+=date;
	filename+=" @ ";
	filename+=time;

	filedesc = fopen(filename.c_str(),"a");

    return 0;
}

/* MsgQueue::CloseDumpFile() ***************************************************/
int MsgQueue::CloseDumpFile()
{
    if (filedesc != NULL) {
        fclose(filedesc);
        filedesc=NULL;        
        cout<<"Game "<<table<<" ended."<<endl;

		string oldfile=filename;
		filename.replace(21,3,db->getBuyinFromName(table));
		int res=rename(oldfile.c_str(),filename.c_str());
		if (res==0) {
			cout<<"File saved and closed."<<endl;
		}
		else {
			perror("Could not move file");
		}
    }

    return 0;
}

/* MsgQueue::ParseNewMessages() ***********************************************/
int MsgQueue::ParseNewMessages()
{
    return 0;
}

/* MsgQueue::ackPlace() *******************************************************/
void MsgQueue::ackPlace()
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	MouseClick(cx/2,cy/2+50,true);
}

/* MsgQueue::tableGood() ******************************************************/
bool MsgQueue::tableGood()
{
	return IsWindow(RVPhwnd);
}

/* MsgQueue::takeSeat() *******************************************************/
void MsgQueue::takeSeat()
{
	if (ai->watchingOnly()) return;
	if (!allowPlay) return;

	MouseClick(20+70, 70 +50);	Sleep(10);
	MouseClick(20+250,20 +50);	Sleep(10);
	MouseClick(20+490,20 +50);	Sleep(10);
	MouseClick(20+680,70 +50);	Sleep(10);
	MouseClick(20+720,200+50);	Sleep(10);
	MouseClick(20+680,370+50);	Sleep(10);
	MouseClick(20+500,420+50);	Sleep(10);
	MouseClick(20+250,420+50);	Sleep(10);
	MouseClick(20+70, 370+50);	Sleep(10);
	MouseClick(20+20, 210+50);	Sleep(10);

	Sleep(200);

	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	MouseClick(cx/2-50,cy/2+105,true);

}

/* MsgQueue::MouseClick() *****************************************************/
int MsgQueue::MouseClick(int x, int y, bool raw)
{
	if (ai->watchingOnly()) return -1;

    static int firstinit=1;
//    static UINT(*SendIn)(UINT,INPUT*,int); 
//    static BOOL (*GetWindowInfo)(HWND,PWINDOWINFO);
    static HMODULE lib;

    if (firstinit)
    {
        lib=LoadLibrary("user32.dll");
//        SendIn=(UINT(*)(UINT,INPUT*,int))GetProcAddress(lib,"SendInput");
//        GetWindowInfo=(BOOL(*)(HWND,PWINDOWINFO))GetProcAddress(lib,"GetWindowInfo");

        firstinit=0;
    }
	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	SetForegroundWindow(RVPhwnd);

	if (!raw) {
		WINDOWINFO inf;
		memset(&inf,0,sizeof(inf));
		GetWindowInfo(RVPhwnd,&inf);
		x+=inf.rcWindow.left;
		y+=inf.rcWindow.top;
	}

    INPUT in;
    memset(&in,0,sizeof(in));
    in.type=INPUT_MOUSE;
    in.mi.dx=x*(65535/cx);
    in.mi.dy=y*(65535/cy);;
    in.mi.dwFlags=MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;

	SendInput(1,&in,sizeof(in));
    in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP;
    SendInput(1,&in,sizeof(in));


    return 0;
}

/* MsgQueue::Keyboard() *******************************************************/
int MsgQueue::Keyboard(int key)
{
    INPUT in;
    memset(&in,0,sizeof(in));
    in.type=INPUT_KEYBOARD;
    in.ki.wVk = key;
    in.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1,&in,sizeof(in));
    in.ki.dwFlags = 0;

    SendInput(1,&in,sizeof(in));

    return 0;
}

/* MsgQueue::DoBet() **********************************************************/
int MsgQueue::DoBet(int b)
{
    MouseClick(760,480);

	Sleep(50);

    const int keybase=0x30;

    char buff[10];
    memset(buff,0,sizeof(buff));
    itoa(10,buff,10);

    Keyboard(0x08);
    Keyboard(0x08);
    Keyboard(0x08);
    Keyboard(0x08);
    Keyboard(0x08);

    Keyboard(0x2E);
    Keyboard(0x2E);
    Keyboard(0x2E);
    Keyboard(0x2E);
    Keyboard(0x2E);

//	Keyboard(keybase);
//	Keyboard(keybase);
//	Keyboard(keybase);
    if ((b/10000)%10!=99) Keyboard(keybase+(b/10000)%10);
    if ((b/1000 )%10!=99) Keyboard(keybase+(b/1000 )%10);
    if ((b/100  )%10!=99) Keyboard(keybase+(b/100  )%10);
    if ((b/10   )%10!=99) Keyboard(keybase+(b/10   )%10);
    if ((b/1    )%10!=99) Keyboard(keybase+(b/1    )%10);

	Sleep(500);

	return MouseClick(460,330); 

    return 0;
}

/* MsgQueue::stayActive() *****************************************************/
void MsgQueue::stayActive()
{
	bool click=false;
	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			if (GetRValue(GetColor(6+i,524+j))<100) {
				click=true;
			}
		}
	}

	if (click) {
		cur (0,1);
		cout << "Activate: " << (void*)GetColor(8,526) << "   " << endl;
		MouseClick(30,565);
		ai->dbRegAct("stayActive","");
	
		char buff[400];
		memset(buff,0,sizeof(buff));
		strcat(buff,"*stayActive");
		fwrite(buff,1,12,filedesc);
	}
}

/* MsgQueue::GetColor() *******************************************************/
int MsgQueue::GetColor(int x, int y)
{
	SetForegroundWindow(RVPhwnd);
    return GetPixel(RVPhdc,x,y);
}

/* MsgQueue::GetCardOnTable() *************************************************/
struct point
{
    int x,y;
};

struct OCRDef
{
    char val;
    point p[MAXOCRPOINTS];
};

Card MsgQueue::GetCardOnTable(int x, int y)
{
	SetForegroundWindow(RVPhwnd);
    SetPixel(RVPhdc,x,y,0x00ff00);

    // OCR stuff

    OCRDef cards[52];
    memset(cards,0,sizeof(cards));

    char val=0,suit=0;

    cards[0].val='2';
    cards[0].p[0].x=4;  cards[0].p[0].y=8;
    cards[0].p[1].x=12; cards[0].p[1].y=9;
    cards[0].p[2].x=8;  cards[0].p[2].y=12;
    cards[0].p[3].x=12; cards[0].p[3].y=17;
    cards[0].p[4].x=5;  cards[0].p[4].y=17;

    cards[1].val='3';
    cards[1].p[0].x=4;  cards[1].p[0].y=5;
    cards[1].p[1].x=6;  cards[1].p[1].y=5;
    cards[1].p[2].x=8;  cards[1].p[2].y=5;
    cards[1].p[3].x=10; cards[1].p[3].y=5;
    cards[1].p[4].x=12; cards[1].p[4].y=5;
    cards[1].p[5].x=10; cards[1].p[5].y=7;
    cards[1].p[6].x=9;  cards[1].p[6].y=8;
    cards[1].p[7].x=12; cards[1].p[7].y=13;
    cards[1].p[7].x=12; cards[1].p[8].y=14;

    cards[2].val='4';
    cards[2].p[0].x=10; cards[2].p[0].y=13;
    cards[2].p[1].x=9;  cards[2].p[1].y=6;
    cards[2].p[2].x=3;  cards[2].p[2].y=13;
    cards[2].p[3].x=10; cards[2].p[3].y=16;
    cards[2].p[4].x=7;  cards[2].p[4].y=14;
    cards[2].p[5].x=5;  cards[2].p[5].y=14;

    cards[3].val='5';
    cards[3].p[0].x=11; cards[3].p[0].y=5;
    cards[3].p[1].x=4;  cards[3].p[1].y=5;
    cards[3].p[2].x=6;  cards[3].p[2].y=5;
    cards[3].p[3].x=8;  cards[3].p[3].y=5;
    cards[3].p[4].x=4;  cards[3].p[4].y=10;
    cards[3].p[5].x=4;  cards[3].p[5].y=8;
    cards[3].p[6].x=4;  cards[3].p[6].y=7;
    cards[3].p[7].x=7;  cards[3].p[7].y=9;
    cards[3].p[8].x=8;  cards[3].p[8].y=9;
    cards[3].p[9].x=11; cards[3].p[9].y=11;

    cards[4].val='6';
    cards[4].p[0].x=12; cards[4].p[0].y=8;
    cards[4].p[1].x=10; cards[4].p[1].y=6;
    cards[4].p[2].x=8;  cards[4].p[2].y=10;
    cards[4].p[3].x=10; cards[4].p[3].y=10;
    cards[4].p[4].x=4;  cards[4].p[4].y=12;
    cards[4].p[5].x=4;  cards[4].p[5].y=13;
    cards[4].p[6].x=3;  cards[4].p[6].y=12;

    cards[5].val='7';
    cards[5].p[0].x=4;  cards[5].p[0].y=8;
    cards[5].p[1].x=4;  cards[5].p[1].y=7;
    cards[5].p[2].x=4;  cards[5].p[2].y=6;
    cards[5].p[3].x=6;  cards[5].p[3].y=6;
    cards[5].p[4].x=8;  cards[5].p[4].y=6;
    cards[5].p[5].x=10; cards[5].p[5].y=6;
    cards[5].p[6].x=12; cards[5].p[6].y=6;
    cards[5].p[7].x=8;  cards[5].p[7].y=17;
    cards[5].p[8].x=8;  cards[5].p[8].y=14;

    cards[6].val='8';
    cards[6].p[0].x=5;  cards[6].p[0].y=10;
    cards[6].p[1].x=10; cards[6].p[1].y=10;
    cards[6].p[2].x=4;  cards[6].p[2].y=7;
    cards[6].p[3].x=12; cards[6].p[3].y=7;
    cards[6].p[4].x=4;  cards[6].p[4].y=11;
    cards[6].p[5].x=11; cards[6].p[5].y=11;
    cards[6].p[6].x=8;  cards[6].p[6].y=10;
    cards[6].p[7].x=12; cards[6].p[7].y=6;
    cards[6].p[8].x=12; cards[6].p[8].y=7;
    cards[6].p[9].x=12; cards[6].p[9].y=8;
    cards[6].p[10].x=11; cards[6].p[10].y=9;
    cards[6].p[11].x=11; cards[6].p[11].y=10;
    cards[6].p[12].x=11; cards[6].p[12].y=8;

    cards[7].val='9';
    cards[7].p[0].x=7;  cards[7].p[0].y=5;
    cards[7].p[1].x=11; cards[7].p[1].y=8;
    cards[7].p[2].x=3;  cards[7].p[2].y=14;
    cards[7].p[3].x=3;  cards[7].p[3].y=10;
    cards[7].p[4].x=6;  cards[7].p[4].y=12;
    cards[7].p[5].x=9;  cards[7].p[5].y=12;

    cards[8].val='0';
    cards[8].p[0].x=3;  cards[8].p[0].y=6;
    cards[8].p[1].x=3;  cards[8].p[1].y=10;
    cards[8].p[2].x=6;  cards[8].p[2].y=10;
    cards[8].p[3].x=6;  cards[8].p[3].y=11;
    cards[8].p[4].x=6;  cards[8].p[4].y=12;
    cards[8].p[5].x=3;  cards[8].p[5].y=18;
    cards[8].p[6].x=13; cards[8].p[6].y=11;
    cards[8].p[7].x=13; cards[8].p[7].y=12;
    cards[8].p[8].x=13; cards[8].p[8].y=13;

    cards[9].val='J';
    cards[9].p[0].x=4;  cards[9].p[0].y=14;
    cards[9].p[1].x=13; cards[9].p[1].y=5;
    cards[9].p[2].x=7;  cards[9].p[2].y=17;
    cards[9].p[3].x=11; cards[9].p[3].y=5;
    cards[9].p[4].x=11; cards[9].p[4].y=6;
    cards[9].p[5].x=11; cards[9].p[5].y=7;
    cards[9].p[6].x=11; cards[9].p[6].y=8;
    cards[9].p[7].x=11; cards[9].p[7].y=9;
    cards[9].p[8].x=11; cards[9].p[8].y=10;
    cards[9].p[9].x=11; cards[9].p[9].y=11;

    cards[10].val='Q';
    cards[10].p[0].x=11; cards[10].p[0].y=18;
    cards[10].p[1].x=12; cards[10].p[1].y=11;
    cards[10].p[2].x=4;  cards[10].p[2].y=13;
    cards[10].p[3].x=8;  cards[10].p[3].y=14;
    cards[10].p[4].x=9;  cards[10].p[4].y=15;
    cards[10].p[5].x=3;  cards[10].p[5].y=12;
    cards[10].p[6].x=3;  cards[10].p[6].y=13;

    cards[11].val='K';
    cards[11].p[0].x=4;  cards[11].p[0].y=9;
    cards[11].p[1].x=8;  cards[11].p[1].y=9;
    cards[11].p[2].x=5;  cards[11].p[2].y=12;
    cards[11].p[3].x=9;  cards[11].p[3].y=13;

    cards[12].val='A';
    cards[12].p[0].x=8;  cards[12].p[0].y=6;
    cards[12].p[1].x=7;  cards[12].p[1].y=7;
    cards[12].p[2].x=8;  cards[12].p[2].y=8;
    cards[12].p[3].x=4;  cards[12].p[3].y=17;
    cards[12].p[4].x=8;  cards[12].p[4].y=14;
    cards[12].p[5].x=9;  cards[12].p[5].y=14;

    for (int i=0; i<52&&cards[i].val!=0; i++)
    {
        bool good=true;
        bool first=false;
        int j;
        for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
        {
            if (!GC(cards[i].p[j].x,cards[i].p[j].y))
            {
                good=false;
            }
        }
        if (good) first=true;
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x,cards[i].p[j].y+1))
                {
                    good=false;
                }
            }
        }
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x,cards[i].p[j].y-1))
                {
                    good=false;
                }
            }
        }
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x+1,cards[i].p[j].y))
                {
                    good=false;
                }
            }
        }
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x-1,cards[i].p[j].y))
                {
                    good=false;
                }
            }
        }
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x+1,cards[i].p[j].y+1))
                {
                    good=false;
                }
            }
        }
        if (!good)
        {
            good=true;
            for (j=0; j<MAXOCRPOINTS&&cards[i].p[j].x!=0; j++)
            {
                if (!GC(cards[i].p[j].x-1,cards[i].p[j].y-1))
                {
                    good=false;
                }
            }
        }
        if (good)
        {
            val=cards[i].val;
            i=j=1000;
        }
    }

    // Suits

    if (GetColor(x+5,y+28)==0x00 && GetColor(x+12,y+28)==0x00)
    {
        if (GetGValue(GetColor(x+9,y+25))>100)
        {
            suit='c';
        }
        else
        {
            suit='s';
        }
    }

    if (GetColor(x+6,y+24)==0x0000fd && GetColor(x+10,y+24)==0x0000fd)
    {
        suit='h';
    }
          
    if (GetGValue(GetColor(x+4,y+25))>100 && GetColor(x+7,y+28)==0x0000fd)
    {
        suit='d';
    }

//    printf("%c%c ",val,suit);

    return Card(suit,val);
}

/* MsgQueue::GetButtonOnTable() ***********************************************/
int MsgQueue::GetButtonOnTable()
{
	int ret=2;

	if		(GetRValue(GetColor(90, 270))>=200) ret=9;
	else if (GetRValue(GetColor(110,305))>=200) ret=8;
	else if (GetRValue(GetColor(340,355))>=200) ret=7;
	else if (GetRValue(GetColor(555,355))>=200) ret=6;
	else if (GetRValue(GetColor(620,355))>=200) ret=5;
	else if (GetRValue(GetColor(620,220))>=200) ret=4;
	else if (GetRValue(GetColor(595,135))>=200) ret=3;
	else if (GetRValue(GetColor(250,100))>=200) ret=1;
	else if (GetRValue(GetColor(185,115))>=200) ret=0;

	return ret;
}

/* MsgQueue::GetHoleCardsOnTable() ********************************************/
PokerHand MsgQueue::GetHoleCardsOnTable(int prel)
{
	int p=GetNameIndexTrue(prel);
	PokerHand ret;    

	if (ai->watchingOnly()&&getNewHand()) {
		ret += Card(rand()%52);
		ret += Card(rand()%52);
		return ret;
	}
	
	int xpos[10],ypos[10];

    xpos[0]=62;  ypos[0]=16;    // Verified
    xpos[1]=239; ypos[1]=0;     // Verified
    xpos[2]=479; ypos[2]=0;     // Verified
    xpos[3]=663; ypos[3]=24;
    xpos[4]=700; ypos[4]=168;   // Verified
    xpos[5]=667; ypos[5]=326;
    xpos[6]=479; ypos[6]=367;   // Verified
    xpos[7]=239; ypos[7]=366;   // Verified
    xpos[8]=55;  ypos[8]=327;   // Verified
    xpos[9]=3;   ypos[9]=169;

    const int card2shift=30;

    ret=GetCardOnTable(xpos[p],ypos[p]); 
	ret+=GetCardOnTable(xpos[p]+card2shift,ypos[p]);

	return ret;
}

/* MsgQueue::CalcPlayerOffset() ***********************************************/
void MsgQueue::CalcPlayerOffset()
{
//	getch();
	ShowWindow(RVPhwnd,SW_SHOWNORMAL);
	BringWindowToTop(RVPhwnd);
	int tmp=GetButtonOnTable();
	playerOffset=(10+getDealer()-tmp)%10;
//	cout << tmp << " " << getDealer() << " " << playerOffset;
//	getch();
}