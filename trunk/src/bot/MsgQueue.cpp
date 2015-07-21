
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
    void* buffindex=(void*)msg;
	static int firsthand=1;

	static bool extraChips=false;
	static int extraChipPlayer=0;

	if (((char*)buffindex)[0]!='>') return -2;

    if (strncmp((char*)buffindex,"> Deciding Dealer",16)==0)
    {
       
//        printf("Tournament beginning.\n");
        return 1;
    }
    if (strncmp((char*)buffindex,"> The tournament is now at level 1.",35)==0)
    {
//        printf("Tournament ending.\n");
        return 0;
    }
	if (extraChips)
	{
		for (int i=0; i<500; i++) {
			printf("mmm");
		}

        char* tmp=(char*)buffindex;
        char buff[10];
		memset(buff,0,sizeof(buff));
        for (int i=0,j=0; i<10; i++){if (tmp[i]!=','){if (tmp[i] >= '0' && tmp[i] <= '9'){buff[j]=tmp[i];}else{buff[i]=0;break;}j++;}}
		int amt=atoi(buff);

		if (amt==0) {
			char buff[400];
			memset(buff,0,sizeof(buff));
			strcat(buff,"ANT: 0");
			fwrite(buff,1,12,filedesc);
		}
		else {
			char buff[400];
			memset(buff,0,sizeof(buff));
			strcat(buff,"ANT: ");
			strcat(buff,tmp);
			fwrite(buff,1,12,filedesc);
		}

		extraChips=false;
/*		char intbuff[40];
		memset(intbuff,0,sizeof(intbuff));
		char* charIndex=((char*)buffindex);
        for (int i=0,j=0; i<10; i++){if (charIndex[i]!=','){if (charIndex[i] >= '0' && charIndex[i] <= '9'){intbuff[j]=charIndex[i];}else{intbuff[i]=0;break;}j++;}}
		int amt=atoi(intbuff);
		extraChips=false;
		status.pot-=amt;
        players[extraChipPlayer]->stack+=amt;
*/	}

    int k=0;
    void* name=buffindex;
    void* command=buffindex;
    for (k=0;((char*)command)[k]!=' ';k++);
    name=(void*)((int)command+k+1);
    for (k++;((char*)command)[k]!=' ';k++);
    command=(void*)((int)command+k+1);

    for(k=0;((char*)name)[k]!=' ';k++);
    ((char*)name)[k]=0;

    if (strncmp((char*)command,"#",1)==0)
    {
        events.newhand=true;
        events.newstage=true;
        status.stage=STAGE_PREFLOP;
        status.extrabet=0;
        status.allins=0;
        status.handcount++;
        status.pot=int(status.bb*1.5);
        for (int i=0; i<7; i++) status.cards[i].Clear();

        if (!firsthand){
			ai->reset();	

            for (int i=0; i<sizeof(players)&&players[i]!=NULL; i++)
            {
                if (isAlive(i)) 
				{
					players[i]->NewRound();
				}
            }
        }
    } 
    if (strncmp((char*)command,"the Flop",8)==0)
    {
        events.newstage=true;  
        status.stage=STAGE_FLOP;
        memset(status.bet,0,sizeof(status.bet));
        status.active=getNextActivePlayer(status.dealer);
		PushAction(STAGE_CHANGE,STAGE_FLOP);
        for (int i=0; i<sizeof(players)&&players[i]!=NULL; i++)
        {
            if (players[i]->isActive()&&isAlive(i)) players[i]->Flop();
        }

        if (((char*)command)[9]=='1') command=(void*)(((int)command)+1);
        char v=((char*)command)[9];
        char s=((char*)command)[10];
        switch (s)
        {
            case 'ª': s='s'; break; //spads
            case '©': s='h'; break; //hearts
            case '§': s='c'; break; //clubs
            case '¨': s='d'; break; //Diamonds
        }
        status.cards[2].SetCard(s,v);//=CardToInt(v,s);

        if (((char*)command)[11]=='1') command=(void*)(((int)command)+1);
        v=((char*)command)[11];
        s=((char*)command)[12];
        switch (s)
        {
            case 'ª': s='s'; break; //spads
            case '©': s='h'; break; //hearts
            case '§': s='c'; break; //clubs
            case '¨': s='d'; break; //Diamonds
        }
        status.cards[3].SetCard(s,v);//=CardToInt(v,s);

        if (((char*)command)[13]=='1') command=(void*)(((int)command)+1);
        v=((char*)command)[13];
        s=((char*)command)[14];
        switch (s)
        {
            case 'ª': s='s'; break; //spads
            case '©': s='h'; break; //hearts
            case '§': s='c'; break; //clubs
            case '¨': s='d'; break; //Diamonds
        }
        status.cards[4].SetCard(s,v);//=CardToInt(v,s);
    }
    if (strncmp((char*)command,"the turn",8)==0)
    {    
        events.newstage=true;
        status.stage=STAGE_TURN;
		memset(status.bet,0,sizeof(status.bet));            
        status.active=getNextActivePlayer(status.dealer);
		PushAction(STAGE_CHANGE,STAGE_TURN);
        for (int i=0; i<sizeof(players)&&players[i]!=NULL; i++)
        {
            if (players[i]->isActive()&&isAlive(i)) players[i]->Turn();
        }  

        if (((char*)command)[9]=='1') command=(void*)(((int)command)+1);
        char v=((char*)command)[9];
        char s=((char*)command)[10];
        switch (s)
        {
            case 'ª': s='s'; break; //spads
            case '©': s='h'; break; //hearts
            case '§': s='c'; break; //clubs
            case '¨': s='d'; break; //Diamonds
        }
        status.cards[5].SetCard(s,v);//=CardToInt(v,s);
    }
    if (strncmp((char*)command,"the river",9)==0)
    {                
        events.newstage=true;
        status.stage=STAGE_RIVER;  
        memset(status.bet,0,sizeof(status.bet));
        status.active=getNextActivePlayer(status.dealer);
		PushAction(STAGE_CHANGE,STAGE_RIVER);
        for (int i=0; i<sizeof(players)&&players[i]!=NULL; i++)
        {
            if (players[i]->isActive()&&isAlive(i)) players[i]->River();
        }    

        if (((char*)command)[10]=='1') command=(void*)(((int)command)+1);
        char v=((char*)command)[10];
        char s=((char*)command)[11];
        switch (s)
        {
            case 'ª': s='s'; break; //spads
            case '©': s='h'; break; //hearts
            case '§': s='c'; break; //clubs
            case '¨': s='d'; break; //Diamonds
        }
        status.cards[6].SetCard(s,v);//=CardToInt(v,s);
    }
    if (strncmp((char*)command,"has left",8)==0)
    {
        int i=0;
        for (i=0; ((char*)command)[i]!=0; i++);
        if (((char*)command)[i-1]==' ') ((char*)command)[i]=' '; 

		if (strncmp((char*)command,"has left the",10)==0 && strncmp((char*)command,"has left the table",15)!=0)
		{
			int nameindex=getNameIndex((char*)name);
			players[nameindex]->eliminated(10-eliminations);
			eliminations++;                                         
			status.livingplayers--;
			if (nameindex==ai->getAIPlayer()&&ai->getAIMoniker()==getName(nameindex)) {
				Sleep(1000);
				ackPlace();
				seated=false;
				db->RegisterGameAI(11-eliminations);
			}
			if (eliminations>=9) {
				seated=false;
			}
		}
	}

    if (strncmp((char*)command,"tournament is now at level",10)==0)
    {
        events.blindsup=true;
        switch (((char*)command)[27])
        {
            case '1': status.bb=20;    break;
            case '2': status.bb=40;    break;
            case '3': status.bb=80;    break;
            case '4': status.bb=160;   break;
            case '5': status.bb=320;   break;
            case '6': status.bb=640;   break;
            case '7': status.bb=1280;  break;
            case '.': printf("error"); break;
        }
    }

    if (strncmp((char*)command,"posted small",9)==0)
    {          
//        events.action=true;
        int nameindex=getNameIndex((char*)name);
        players[nameindex]->stack-=status.bb/2; 
        memset(status.bet,0,sizeof(status.bet));
        status.bet[nameindex]=status.bb/2;
		status.dealer=getLastActivePlayer(nameindex);
	}
    if (strncmp((char*)command,"posted big",9)==0)
    {                 
        events.action=true;
        int nameindex=getNameIndex((char*)name);
        players[nameindex]->stack-=status.bb;  
        status.bet[nameindex]=status.bb;

		// Set UTG's turn
		status.stage=STAGE_PREFLOP;
		status.active=getNextAllivePlayer(nameindex);

    }
    if (strncmp((char*)command,"you",3)==0)
    {
		int i=0;
		for (i=0; ((char*)name)[i]!=','; i++);
		((char*)name)[i]=0;
		int nameindex=getNameIndex((char*)name); // ensure the player is on the list
		status.active=nameindex;
		events.action=true;
		ai->dbRegAct("timeOut",(char*)name);

		if (nameindex==ai->getAIPlayer()) {
			ai->dbRegAct("timeOutFix",(char*)name);
			updateHoleCards();
			if (status.cards[0]!=NOCARD&&status.cards[1]!=NOCARD) {
			}
			else {
				cur(0,16);
				string str=(char*)command;
				cout << "ERROR(" << nameindex << "," << this->getNumHands() << "): Cards = "<<str<<"      " << endl;
				ai->dbRegAct("timeOutFixError",str.c_str());
			}
			if (!dryRun) {
				ai->makePlay();
			}
		}
    }
    if (strncmp((char*)command,"wins",3)==0)
    {               
        events.newstage=true;
        events.action=true;
        status.stage=STAGE_SHOWDOWN;
        int nameindex=getNameIndex((char*)name);
        players[nameindex]->win();

        char* tmp=(char*)command+5;
        char buff[10];
		memset(buff,0,sizeof(buff));
        for (int i=0,j=0; i<10; i++){if (tmp[i]!=','){if (tmp[i] >= '0' && tmp[i] <= '9'){buff[j]=tmp[i];}else{buff[i]=0;break;}j++;}}
		int amt=atoi(buff);

		if (nameindex==ai->getAIPlayer()) db->UpdateAI(amt);
		else db->UpdateAI(0);

		ai->dbRegAct("wins",(char*)name);
		players[nameindex]->stack+=amt;
		status.pot=0;
	}
    if (strncmp((char*)command,"chips returned to ",10)==0)
    {
        events.action=true;
        name=(void*)(((int)command)+18);
        int q=0;
        for (q=0; q<20; q++)
        {
            if (((char*)name)[q]==',')
            {
                ((char*)name)[q]=(char)0;
                break;
            }
        }
        int nameindex=getNameIndex((char*)name);
		extraChipPlayer=nameindex;

        char* tmp=(char*)command+20+q;
        char buff[10];
        for (int i=0,j=0; i<10; i++){if (tmp[i]!=','){if (tmp[i] >= '0' && tmp[i] <= '9'){buff[j]=tmp[i];}else{buff[i]=0;break;}j++;}}
        int amt=atoi(buff);
		if (amt==0) {
			tmp=nextline;
			memset(buff,0,sizeof(buff));
			for (int i=0,j=0; i<10; i++){if (tmp[i]!=','){if (tmp[i] >= '0' && tmp[i] <= '9'){buff[j]=tmp[i];}else{buff[i]=0;break;}j++;}}
			amt=atoi(buff);
		}
		if (amt != status.pot) {
			// maybe a calculation error, but probably a split pot
			//printf("%i ERROR %i %i %s\n", status.handcount, amt, status.pot, command);
		}

		status.pot-=amt;
        players[nameindex]->stack+=amt;
    }
    if (strncmp((char*)command,"bet",		 3)==0 ||
		strncmp((char*)command,"called",	 6)==0 ||
		strncmp((char*)command,"raised",	 6)==0 ||
		strncmp((char*)command,"went all-in",8)==0 ||
		strncmp((char*)command,"checked",	 7)==0 ||
		strncmp((char*)command,"folded",	 6)==0 ||
		strncmp((char*)command,"shows",		 5)==0 ||
		strncmp((char*)command,"did not act",6)==0)
    {  
		RegisterAction((char*)command,(char*)name);
    }

    firsthand=0;
    return 1;
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
    memset(&events, 0, sizeof(events));
    int msg_new=(int) SendMessage (textbox,EM_GETLINECOUNT,0,0);
    if (msg_new > msg_rec)
    {
        for (; msg_rec<msg_new; msg_rec++)
        {
            char tmp[3][255*255];
            tmp[0][0]=(char)255;
            tmp[1][0]=(char)255;
            tmp[2][0]=(char)255;
            
            SendMessage(textbox,EM_GETLINE,msg_rec,(LPARAM)tmp[0] );
            SendMessage(textbox,EM_GETLINE,msg_rec+1,(LPARAM)tmp[1] );
            SendMessage(textbox,EM_GETLINE,msg_rec+2,(LPARAM)tmp[2] );

            if (strncmp  (tmp[0], "> Deciding Dealer", 15)==0)
			{   
				seated=true;
				ifstream in((basedir+"gameid.txt").c_str());
				in >> gameid;
				gameid++;
				ofstream out((basedir+"gameid.txt").c_str());
				out << gameid;
				ai->openProfiler(gameid);

				if (msg_new > 1300)
				{
					printf("Error adjustment on %s.\n", table);
					msg_rec=0;
					SendMessage(textbox,WM_SETTEXT,5,(LPARAM)"" );
				}
                LoadDumpFile();
//                printf("Game %s beginning.\n", table);
            }

            char filebuff[255*255], buff[50];
            filebuff[0]=0;
            itoa(msg_rec, buff, 10);
//            strcat(filebuff, buff);
//            strcat(filebuff, ": ");
            strcat(filebuff, tmp[0]);
            strcat(filebuff, "\n");
//            printf("%s.%i: %s\n",table,msg_rec,tmp[0]);

			int i=0;
            for (i=0; filebuff[i]!=0; i++);
			if (filedesc != NULL) {
				char buff[1000];
				memset(buff,0,sizeof(buff));
				for (int j=0; j<10; j++) {
					if (getStack(j)<0) {
						if (getStack(j)>-10) {
							strcat(buff,"*DOUBLE: ");
						}
						else {
							strcat(buff,"*MINUS: ");
						}
					}
				}
				for (int j=0; j<10; j++) {
					strcat(buff,getName(j));
					strcat(buff," ");
					char buff2[20];
					itoa(getStack(j),buff2,10);
					strcat(buff,buff2);
					strcat(buff," ");
				}
				strcat(buff,"\n");
				int k=0;
				for (k=0; buff[k]!=0; k++);
				fwrite(buff,1,k,filedesc);
				fwrite ( filebuff, 1, i, filedesc );
			}

            if (strncmp("> The tournament is now at level 1.", tmp[0],35)==0) {
				if (eliminations!=0) {
					CloseDumpFile();
				}
            }

            ProcessMessage(tmp[0],tmp[1]);
        }
    }
            
    if (msg_new > 1300)
    {
        printf("Error adjustment on %s.\n", table);
        msg_rec=0;
        SendMessage(textbox,WM_SETTEXT,5,(LPARAM)"" );
    }

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