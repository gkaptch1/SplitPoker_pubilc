
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

	return 0;
}

/* MsgQueue::parseTableInfoFromFile() *****************************************/
void MsgQueue::parseTableInfoFromFile(const char* file)
{

}

/* MsgQueue::processErrorMessage() ********************************************/
void MsgQueue::processErrorMessage(const char* msg)
{
	//TODO this should probably have something in it...
}

/* MsgQueue::getCurrentDate() *************************************************/
string MsgQueue::getCurrentDate()
{	
	// GABE: i gueess this foesnt hurt anyone....
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
	//GABE: This really cant do anything... the bot doesnt have the ability to ask for information.
}

/* MsgQueue::ProcessMessage() *************************************************/
int MsgQueue::ProcessMessage(char* msg, char* nextline)
{       
}

/* MsgQueue::RegisterAction() *************************************************/

int MsgQueue::RegisterAction(char* command, char* name)
{

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

}

/* MsgQueue::LoadDumpFile() ***************************************************/
int MsgQueue::LoadDumpFile()
{                
	return 0;
}

/* MsgQueue::CloseDumpFile() ***************************************************/
int MsgQueue::CloseDumpFile()
{
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
}

/* MsgQueue::tableGood() ******************************************************/
bool MsgQueue::tableGood()
{
	return IsWindow(RVPhwnd);
}

/* MsgQueue::takeSeat() *******************************************************/
void MsgQueue::takeSeat()
{
//TODO rework the messaging

}

/* MsgQueue::DoBet() **********************************************************/
int MsgQueue::DoBet(int b)
{
    //TODO rework the messaging here?
}

/* MsgQueue::stayActive() *****************************************************/
void MsgQueue::stayActive()
{
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

void MsgQueue::CalcPlayerOffset()
{
}