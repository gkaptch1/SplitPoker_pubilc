
/* #includes ******************************************************************/

#include "msgqueue.h"

/* Constructer / Destructer ***************************************************/
MsgQueueBase::MsgQueueBase()
{
	memset(&status,0,sizeof(status));
    memset(&events,0,sizeof(events));

	memset(&players,  0,sizeof(players));
    memset(&holeCards,0,sizeof(holeCards));

	//db			= NULL;
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
	//db->RegisterAIAct(s);
}

/* MsgQueueBase::checkCashIntegrity() *****************************************/
void MsgQueueBase::checkCashIntegrity()
{	

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

}

/* MsgQueueBase::print() *****************************************************/
void MsgQueueBase::print()
{

}

/* MsgQueueBase::printLightHeader() *******************************************/
void MsgQueueBase::printLightHeader()
{

}

/* MsgQueueBase::printLight() *************************************************/
void MsgQueueBase::printLight()
{

}

/* MsgQueueBase::printDefaultBG() *********************************************/
void MsgQueueBase::printDefaultBG()
{

}

/* MsgQueueBase::printLight() *************************************************/
void MsgQueueBase::printPlayerView()
{

}

/* MsgQueueBase::printPlayers() ***********************************************/
void MsgQueueBase::printPlayers(int x, int y)
{

}

/* MsgQueueBase::printPlayer() ************************************************/
void MsgQueueBase::printPlayer(int player, int x, int y)
{	

}