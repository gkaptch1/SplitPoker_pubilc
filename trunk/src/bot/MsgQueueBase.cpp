
/* #includes ******************************************************************/

#include "stdafx.h"
#include "MsgQueue.h"
#include "TournamentData.h"

/* Constructer / Destructer ***************************************************/
MsgQueueBase::MsgQueueBase()
{
	memset(&status,0,sizeof(status));
    memset(&events,0,sizeof(events));

	memset(&players,  0,sizeof(players));
    memset(&holeCards,0,sizeof(holeCards));

	db			= NULL;
    ai			= NULL;

	starttime	= 0;
	totaltime	= 0;
	rounds		= 0;

	gameid		= 0;
	view		= 0;
	buyint		= 0;
	table		= "";
	allowPlay	= true;
	dryRun		= false;

    memset(blinds,0,sizeof(blinds));
}

MsgQueueBase::~MsgQueueBase()
{
}

/* MsgQueueBase::isAlive() ****************************************************/
bool MsgQueueBase::isAlive(int n)
{
	if (players[n]==NULL) {
		return false;
	}
	else {
		return players[n]->isAlive();
	}
}

/* MsgQueueBase::isActive() ***************************************************/
bool MsgQueueBase::isActive(int n)
{
	if (players[n]==NULL) {
		return false;
	}
	else {
		return players[n]->isActive();
	}
}

/* MsgQueueBase::isAllin() ****************************************************/
bool MsgQueueBase::isAllin(int n)
{
	if (players[n]==NULL) {
		return false;
	}
	else {
		return players[n]->isAllin();
	}
}

/* MsgQueueBase::getCostToCall() **********************************************/
int MsgQueueBase::getCostToCall(int dude)
{
    int max=0;

    for (int i=0; i<10; i++)
    {
        if (status.bet[i]>max) max=status.bet[i];
    }

    return max-status.bet[dude];
}

/* MsgQueueBase::isEveryPlayerAllin() *****************************************/
bool MsgQueueBase::isEveryPlayerAllin()
{
	bool ret=true;

	for (int i=0; i<10; i++) {
		if (i!=ai->getAIPlayer()&&isActive(i)&&!isAllin(i)) {
			ret=false;
		}
	}

	return ret;
}

/* MsgQueueBase::getPlayerPlace() *********************************************/
char* MsgQueueBase::getPlayerPlace(int place)
{
	for (int i=0; players[i]!=NULL; i++)
	{
		if (players[i]->GetPlace()==place) return players[i]->GetName();
	}
	return "";
}

/* MsgQueueBase::getCheckAct() ************************************************/
int MsgQueueBase::getCheckAct(int player, int act)
{
	if (!isAlive(player)) return 0;

	if (act==0) return players[player]->GetCheckAct(FOLD)+players[player]->GetCheckAct(CALL)+players[player]->GetCheckAct(RAISE);

	return players[player]->GetCheckAct(act);
}

/* MsgQueueBase::regAIStat() **************************************************/
void MsgQueueBase::regAIStat(const AIStat& s)
{	
	db->RegisterAIAct(s);
}

/* MsgQueueBase::checkCashIntegrity() *****************************************/
void MsgQueueBase::checkCashIntegrity()
{	
	int tableCash=0;
	for (int i=0; i<10; i++) {
		if (isAlive(i)) {
			tableCash+=getStack(i);
		}
	}
	tableCash+=getPot();
	bool cashPos=true;
	for (int i=0; i<10; i++) {
		if (getStack(i)<0) {
			cashPos=false;
		}
	}
	if (tableCash!=10000||!cashPos) {
		cur(0,0);
		color(CUR_RED);
		cout << "ERROR: Cash on table is " << tableCash << " pos: " << cashPos << "\t\t\t" << endl;
		cur(0,0);
	}
	else {
		cur(0,0);
		color(CUR_GREEN);
		cout << " Cash on table is " << tableCash << "\t\t\t" << endl;
		cur(0,0);
	}
	color(CUR_GRAY);
}

/* MsgQueueBase::getNameIndex() ***********************************************/
int MsgQueueBase::getNameIndex(const char* name, bool addPlayer)
{
    int i=0;
    for (i=0; i<9&&players[i]!=NULL; i++) {
        if (strcmp(players[i]->GetName(),name)==0) {
            break;
        }
    }

    if (players[i]==NULL&&addPlayer) {
//        printf("--->Player %s added at %i. \n",name,i);
        players[i]=new PlayerStats(name);
    }

    return i;
}

/* MsgQueueBase::getPlayersActive() *******************************************/
int MsgQueueBase::getPlayersActive()
{
	int ret=0;
	for (int i=0; i<10; i++) {
		if (isActive(i)) {
			ret++;
		}
	}
	return ret;
}

/* MsgQueueBase::getPositionBeforeAbs() ***************************************/
int MsgQueueBase::getPositionBeforeAbs(int dude)
{
    int i,j,found=0;
    for (i=status.dealer+1,j=0;i<10;i++)
    {
        if (i==dude) {
            return j;
        }      
        if (isAlive(i)) j++;
    }
    for (i=0;i<=status.dealer;i++)
    {
        if (i==dude) return j;        
        if (isAlive(i)) j++;
    }

    return -1;
}

/* MsgQueueBase::getPositionBeforeActive() ************************************/
int MsgQueueBase::getPositionBeforeActive(int dude)
{
    int i,j,found=0;
    for (i=status.dealer+1,j=0;i<10;i++)
    {
        if (i==dude) {
            return j;
        }      
        if (isAlive(i)&&players[i]->isActive()) j++;
    }
    for (i=0;i<=status.dealer;i++)
    {
        if (i==dude) return j;        
        if (isAlive(i)&&players[i]->isActive()) j++;
    }

    return -1;
}

/* MsgQueueBase::getPositionAfterAbs() ****************************************/
int MsgQueueBase::getPositionAfterAbs(int dude)
{
    int i,j,found=0;
    for (i=status.dealer,j=0;i>=0;i--)
    {
        if (i==dude) {
            return j;
        }      
        if (isAlive(i)) j++;
    }
    for (i=9;i>=status.dealer;i--)
    {
        if (i==dude) return j;        
        if (players[i]!=NULL&&isAlive(i)) j++;
    }

    return -1;
}

/* MsgQueueBase::getPositionAfterActive() *************************************/
int MsgQueueBase::getPositionAfterActive(int dude)
{
    int i,j,found=0;
    for (i=status.dealer,j=0;i>=0;i--)
    {
        if (i==dude) {
            return j;
        }      
        if (players[i]!=NULL&&isAlive(i)&&players[i]->isActive()) j++;
    }
    for (i=9;i>=status.dealer;i--)
    {
        if (i==dude) return j;        
        if (players[i]!=NULL&&isAlive(i)&&players[i]->isActive()) j++;
    }

    return -1;
}

/* MsgQueueBase::getNextActivePlayer() ****************************************/
int MsgQueueBase::getNextActivePlayer(int p)
{
    for (int i=p+1; i<=9&&players[i]!=NULL; i++)
    {
        if (isAlive(i)&&players[i]->isActive())
        {
            return i;
        }
    }
    for (int i=0; i<p&&players[i]!=NULL; i++)
    {
        if (isAlive(i)&&players[i]->isActive())
        {
            return i;
        }
    }

    return p;
}

/* MsgQueueBase::getNextAllivePlayer() ****************************************/
int MsgQueueBase::getNextAllivePlayer(int p)
{
    for (int i=p+1; i<=9&&players[i]!=NULL; i++)
    {
        if (isAlive(i))
        {
            return i;
        }
    }
    for (int i=0; i<p&&players[i]!=NULL; i++)
    {
        if (isAlive(i))
        {
            return i;
        }
    }

    return p;
}

/* MsgQueueBase::getLastActivePlayer() ****************************************/
int MsgQueueBase::getLastActivePlayer(int p)
{
    for (int i=p-1; i>=0&&players[i]!=NULL; i--)
    {
        if (isAlive(i))//&&players[i]->isActive())
        {
            return i;
        }
    }
    for (int i=9; i>p&&players[i]!=NULL; i--)
    {
        if (isAlive(i))//&&players[i]->isActive())
        {
            return i;
        }
    }
      
//    printf("EE");

    return 0;
}

/* MsgQueueBase::getHandsUntilBB() ********************************************/
int MsgQueueBase::getHandsUntilBB(int player)				
{ 
	int c=0; 

	for (int i=(status.dealer+2)%10; i!=player&&c<10; i=++i%10,c++) {
		while(!isAlive(i)) {
			i++; 
		}
	}

	return c;	
}

/* MsgQueueBase::cur() *******************************************************/
ostream& MsgQueueBase::cur(int x, int y)
{
	if (!dryRun) {
		return ::cur(x,y);
	}
	return cout;
}

/* MsgQueueBase::print() *****************************************************/
void MsgQueueBase::print()
{
	switch (view)
	{
	case VIEW_LIGHT: 
		printLight(); 
		break;

	case VIEW_DEFAULT:
		printDefaultView();
		break;

	case VIEW_PLAYER:
		printPlayerView();
		break;
	}
}

/* MsgQueueBase::printLightHeader() *******************************************/
void MsgQueueBase::printLightHeader()
{
	char buff[90];
	memset(buff,0,sizeof(buff));
	sprintf(buff,"Game Name          | BuyN | Plrs | Actv | Hnds | Stack | Cards |  Time | Av Tm");
	cout << buff << endl;
	for (int i=0; i<80; i++) cout << "-";
	cout << endl;
}

/* MsgQueueBase::printLight() *************************************************/
void MsgQueueBase::printLight()
{
	char buff[90];
	memset(buff,0,sizeof(buff));
	sprintf(buff,"%18s | %4s | %4i | %4i | %4i | %5i | %c%c %c%c | %5i | %5i", table.c_str(), buyin, status.livingplayers, status.active, status.handcount, getStack(ai->getAIPlayer()), status.cards[0].GetIndex(), status.cards[0].GetSuit(), status.cards[1].GetIndex(), status.cards[1].GetSuit(), timeGetTime()-starttime, totaltime/rounds);
	if (filedesc==NULL) {
		color(CUR_RED);
	}
	cout << buff << endl;
	color(CUR_WHITE);

	cur(0,10) << "File: \"" << filename << "\"" << endl;
}

/* MsgQueueBase::printDefaultBG() *********************************************/
void MsgQueueBase::printDefaultBG()
{
	if (!getNewHand()) {
		return;
	}

	cur (0,1);

    printf("--------------------------------------------------------------------------------");
    printf("Game:\t%s\n",table);
	printf("Player:\t%s\n",getName(ai->getAIPlayer()));
    printf("Blinds:\t\n");
    printf("--------------------------------------------------------------------------------");
    printf(" Hand:                                   |                                      ");
    printf(" Cash:                                   |                                      ");
    printf(" Pot:                                    |                                      ");
    printf(" Action:                                 |                                      ");
    printf(" Cards:                                  |                                      ");
    printf("--------------------------------------------------------------------------------");
    printf("       Cards | Player         Cash  Info |                                      ");
    printf(" Pos0:       |                           |                                      ");
    printf(" Pos1:       |                           |                                      ");
    printf(" Pos2:       |                           |                                      ");
    printf(" Pos3:       |                           |                                      ");
    printf(" Pos4:       |                           |                                      ");
    printf(" Pos5:       |                           |                                      ");
    printf(" Pos6:       |                           |                                      ");
    printf(" Pos7:       |                           |                                      ");
    printf(" Pos8:       |                           |                                      ");
    printf(" Pos9:       |                           |                                      ");
    printf("--------------------------------------------------------------------------------");   
    printf("\n");       
}

/* MsgQueueBase::printDefaultView() *******************************************/
void MsgQueueBase::printDefaultView()
{
	checkCashIntegrity();
	color(CUR_GRAY);

	printDefaultBG();

    if (getBlindsUp())
    {
        cur(8,4);
        printf("%i/%i",getBB()/2,getBB());
        cur(0,0);
    }

    if (getNewHand())
    {
        cur(7,6);
        printf("%i",getNumHands());
        cur(7,8);
        printf("      ");
        cur(8,10);
        printf("                              ");
        cur(0,0);
    }

    if (getAction()||getNewStage()||getNewHand())
    {
        cur(7,8);
        printf("%i",getPot());    
        cur(7,7);       
        printf("      ");
        cur(7,7);       
        printf("%i",getStack(ai->getAIPlayer()));
        cur(8,9);
        printf("                              ");
        cur(8,9);
        printf("%s, call: %i",getName(getActionPlayer()), getCostToCall(getActionPlayer()));
        cur(0,0);

        cur(43,6);
        double odds=getPotOdds(getCostToCall(getActionPlayer()));
        if (getCostToCall(getActionPlayer())==0) printf("Pot odds: ---          ");
        else printf("Pot odds: %i.%02i  ",int(((int)(100*odds))/100),int((int)(100*odds)%100));
        cur(43,7);
//            printf("Position (bef,aft): %i,%i -- %i   ",0/*getPositionBeforeActive(getNameIndex(PLAYER))*/,getPositionAfterActive(getNameIndex(PLAYER)),ai->pos);

//        printf("Position (bef,aft): %i,%i -- %i   ",getPositionBeforeActive(getNameIndex(PLAYER)),getPositionAfterActive(getNameIndex(PLAYER)),ai->pos);


//******************************   
    // tmp card test

    int xpos[10],ypos[10];

    xpos[0]=62;  ypos[0]=16;    // Verified
    xpos[1]=239; ypos[1]=0;     // Verified
    xpos[2]=479; ypos[2]=0;     // Verified
    xpos[3]=663; ypos[3]=24;
    xpos[4]=700; ypos[4]=168;   // Verified
    xpos[5]=667; ypos[5]=326;
    xpos[6]=479; ypos[6]=367;   // Verified
    xpos[7]=240; ypos[7]=366;   // Verified
    xpos[8]=55;  ypos[8]=327;   // Verified
    xpos[9]=3;   ypos[9]=169;

    int card2shift=30;
    
	cur(7,13); GetHoleCards(0).print();
    cur(7,14); GetHoleCards(1).print();
    cur(7,15); GetHoleCards(2).print();
    cur(7,16); GetHoleCards(3).print();
    cur(7,17); GetHoleCards(4).print();
    cur(7,18); GetHoleCards(5).print();
    cur(7,19); GetHoleCards(6).print();
    cur(7,20); GetHoleCards(7).print();
    cur(7,21); GetHoleCards(8).print();
    cur(7,22); GetHoleCards(9).print();

    int player0=5;//getNameIndex("ekaare");  // FIXME
    
    int mode=1;
    bool dispall=false;

    for (int i=0; i<10; i++)
    {
        cur(15,13+i-player0+(((i-player0)<0)?(10):(0)));
        printf("%s",getName(i));
        cur(30,13+i-player0+(((i-player0)<0)?(10):(0)));
        printf("      ");
        cur(30,13+i-player0+(((i-player0)<0)?(10):(0)));
        printf("%i",getStack(i));
        cur(36,13+i-player0+(((i-player0)<0)?(10):(0)));
        printf((getPlayer(i).isActive()?"a":" "));
        cur(38,13+i-player0+(((i-player0)<0)?(10):(0)));
        printf((getPlayer(i).isAlive()?"y":" "));

        if (isActive(i))
        {
            switch (mode)
            {
            case 0:
                cur(42,12);
                printf(" Fld Chk Cal Bet Ras | Tight   Aggres ");
                cur(43,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetFold());
                cur(47,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetCheck());
                cur(51,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetCall());
                cur(55,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetBet());
                cur(59,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetRaise());
                cur(63,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("| %i",int(getPlayer(i).GetTight()*1000));
                cur(72,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf(" %i",int(getPlayer(i).GetAggressive()*1000));
                break;

            case 1:  
                cur(42,12);
                printf(" Flp Trn Riv Pot Shw | Tight   Aggres ");
                cur(43,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetFlop());
                cur(47,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetTurn());
                cur(51,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetRiver());
                cur(55,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("%i",getPlayer(i).GetPotsWon());
                cur(59,13+i-player0+(((i-player0)<0)?(10):(0)));
//                    printf("%i",GetPlayer(i)->GetShowdownWon());
                cur(63,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf("| %i",int(getPlayer(i).GetTight()*1000));
                cur(72,13+i-player0+(((i-player0)<0)?(10):(0)));
                printf(" %i",int(getPlayer(i).GetAggressive()*1000));

            }
        }
        else if (!dispall)
        {    
            cur(43,13+i-player0+(((i-player0)<0)?(10):(0)));
            printf("                    |                ");
        }
    }

    cur(0,0);
//****************************///
    }

    if (getNewStage())
    {            
        cur(16,10);
        printf("            ");         
        cur(9,10);
        getCard(0).Print(); printf(" ");
        getCard(1).Print(); printf(" -- ");
        getCard(2).Print(); printf(" ");
        getCard(3).Print(); printf(" ");
        getCard(4).Print(); printf(" ");
        getCard(5).Print(); printf(" ");
        getCard(6).Print(); printf(" ");
        cur(0,0);
    }

    cur(0,24);
    printf(" --> ");
    int move=0;//ai->GetAction(getNameIndex(PLAYER));
    if (move==FOLD)  printf("Fold        ");
    if (move==CHECK) printf("Check       ");
    if (move==CALL)  printf("Call        ");
    if (move==ALLIN) printf("All-In      ");
    if (move==BET)   printf("Bet         ");
    if (move>BET)    printf("Bet %i      ",move);

    cur(0,0);
}

/* MsgQueueBase::printLight() *************************************************/
void MsgQueueBase::printPlayerView()
{
	checkCashIntegrity();

	bool utg=(getUTGPlayer()==getAI()->getAIPlayer());
	if (getAction()||getNewStage()||getNewHand()||utg)
	{
		printPlayer(0,1, 2);
		printPlayer(1,21,2);
		printPlayer(2,41,2);
		printPlayer(3,61,2);
		printPlayer(4,1, 22);
		printPlayer(5,21,22);
		printPlayer(6,41,22);
		printPlayer(7,61,22);
		printPlayer(8,1, 42);
		printPlayer(9,21,42);

		cur (1,62);
	}
}

/* MsgQueueBase::printPlayers() ***********************************************/
void MsgQueueBase::printPlayers(int x, int y)
{
	cur(x,y);
	cout << "players:  " << endl;
	for (int i=0; i<10; i++) {
		cur(x,y+1+i);
		if (isActive(i)) {
			color(CUR_WHITE);
		}
		else {
			color(CUR_GRAY);
		}

		cout << "  " << i << ((isAlive(i))?(" "):("*")) << " " << getStack(i) << "    \t" << getName(i) << "          " << endl;
	}
	color(CUR_WHITE);
}

/* MsgQueueBase::printPlayer() ************************************************/
void MsgQueueBase::printPlayer(int player, int x, int y)
{	
	if (!isAlive(player)) {
		for (int i=0; i<20; i++) {
			cur(x,y+i);
			printf("                   ");
		}
		return;
	}

	int c=CUR_WHITE;
	bool bw=false;
	if		(!isActive(player))	{ c=(CUR_BLUE);  bw=true; }
//	else if (!isAlive (player))	{ c=(CUR_GREEN); bw=false; }

	getAI()->printPlayerCards(player, x, y, c, bw);
	cur(x,y+16); printf("%s (%i) %i  ",getName(player),getStack(player),GetNameIndexTrue(player));

	cur(x,y+15); printf("                    ");	
//	cur(x,y+15); getAI()->percentHandsBeaten(player,true);
	cur(x,y+15); printf("%i: %i,%i,%i",getCheckAct(player),getCheckAct(player,FOLD),getCheckAct(player,CALL),getCheckAct(player,RAISE));

	cur(x,y+17); printf("                    ");
	cur(x,y+17);
	int dealer=getDealer(),
		sb=getNextAllivePlayer(dealer),
		bb=getNextAllivePlayer(sb);
	if (dealer==player)										printf("[BUTTON] ");
	if (sb==player)											printf("[SB] ");
	if (bb==player)											printf("[BB] ");
	if (getActionPlayer()==player)							printf("Action ");
	cur(x,y+18); printf("                    ");
	cur(x,y+18);


	PokerHand h=GetHoleCards(player);  // Use to verify hand guesses
/*	static PokerHand h2=h;
	 if (h[0]!=NOCARD&&getStage()==STAGE_RIVER) {
		handsTotal++;
		handsAccuracy+=getAI()->getPlayerCards(player).getNumHands();
		h2[0]=h[0];
		h2[1]=h[1];
		if (getAI()->getPlayerCards(player).GetHand(h[0],h[1])==1) {
		handsMatched++;
			color(CUR_GREEN);
		}
		else { 
			color(CUR_RED);
		}	
	}
*/
	h.print();

	color(CUR_GRAY);
}