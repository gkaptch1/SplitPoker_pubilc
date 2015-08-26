
/* #includes ******************************************************************/

#include "HoldemTournamentAI.h"
//#include <stdlib.h>  //WE CAN PUT THIS BACK IN IF WE NEED TO USE ITOA (IN PROFILER CODE)

/* Constructer / Destructer ***************************************************/
HoldemTournamentAIBase::HoldemTournamentAIBase(MsgQueueBase* q)
{
	playerName = NULL;

    passivity	= 0;
	pos			= 0;
	
	table=q;
	reset();

	moniker  = conf.getStrValue("moniker");
	aiverstr = conf.getStrValue("aiver_major");
	aiverint = (int)conf.getRealValue("aiver_minor");

	watchOnly=false;

	bool boo=false;
	toggleVar(boo);
	profile_line("fudge");
}

HoldemTournamentAIBase::~HoldemTournamentAIBase()
{
	//profiler.close();
}

void HoldemTournamentAIBase::uberDebug(char* s)
{
	const int x=30;
	const int y=60;

	static int count=0;
	count++;

	static int handMaskFlagOld=0;
	int handMaskFlag=0;
	for (int i=0; i<13; i++) {
		for (int j=0; j<13; j++) {
			if (handWinMask.GetHand(Card(i),Card(j))&PHC_WINNER) {
				i=j=20;
				handMaskFlag=57;
			}
		}
	}

	if (handMaskFlag!=0&&handMaskFlagOld==0) {
		cout << count;
	}

	handMaskFlagOld=handMaskFlag;

	cur(x,y+count%15);
	cout << count << ": " << handMaskFlag << " " << s << "\t\t";
	cur(x,y+1+count%15);
	cout << "\t\t\t";
}

/* HoldemTournamentAIBase::getAIPlayer() **************************************/
int HoldemTournamentAIBase::getAIPlayer()										
{ 
	if (watchingOnly()&&!table->getDryRun()) {
		return 0;
	}
	else {
		return table->getNameIndex(moniker.c_str(),false);	
	}
}

/* HoldemTournamentAIBase::reset() ********************************************/
void HoldemTournamentAIBase::reset()
{
	AIDebug("reset");
    for (int i=0; i<10; i++)
    {
		playercards[i].SetAll(1);
    }

	aiRank=&defRanks;

	flopRank.clear();
	turnRank.clear();
	rivRank.clear();

	actionqueue.clear();
}

/* HoldemTournamentAIBase::openProfiler() *************************************/
void HoldemTournamentAIBase::openProfiler(int game)
{
	/*string file="C:\\poker\\profiler\\game_";
	char buff[40];
	memset(buff,0,sizeof(buff));
	itoa(table->getGameID(),buff,10);
	file+=buff;
	file+=".txt";
	profiler.close();
	profiler.open(file.c_str());//, ofstream::out | ofstream::app);
	*/
}

/* HoldemTournamentAIBase::makePlay() *****************************************/
void HoldemTournamentAIBase::makePlay()
{
	bool utg=false;
	if (table->getNumHands()!=1) {

		if (table->getPlayersActive()<=1&&!table->isEveryPlayerAllin()) {
			static int tmpCtr=0;
			tmpCtr++;
			return;
		}

		utg=(table->getUTGPlayer()==getAIPlayer());
	}

	if (table->getCard(0)!=NOCARD&&table->getCard(1)!=NOCARD)
	{
		//GABE TODO ;ets make sure that this doesnt break everything without the sleep
		if (utg) //Sleep(4000);
		table->updateHoleCards();

		if (getAIPlayer()==table->getActionPlayer()) {	
			ActionRet play=getPlay();
//			Sleep(rand()%2000);
			switch(play.action)
			{
			case BET:
			case RAISE:
				table->DoBet(play.amt);
				//Sleep(50);
				table->DoCall();
				break;

			case CALL:
				table->DoCheck();
				//Sleep(50);
				table->DoCall();
				break;

			case CHECK:
			case FOLD:
				table->DoCheck();
				//Sleep(50);
				table->DoFold();
				break;
			}
		}
		else {
//			t.getAI()->getPlay();
		}
	}
}

/* HoldemTournamentAIBase::update() *******************************************/
void HoldemTournamentAIBase::update()
{
/*	AIDebug("EvaluateStrategy");
	cur(0,64);

	PokerHand h;//=table->getBoard();
	for (int i=2; i<7&&table->getCard(i)!=NOCARD; i++) {
		h+=table->getCard(i);
	}

	/* handWinMask */
//	calcHandWinMask();

	/* evaluate actions */
/*	if (flopRank.empty()&&table->getStage()==STAGE_FLOP)  flopRank=RankFlop(h);
	if (turnRank.empty()&&table->getStage()==STAGE_TURN)  turnRank=RankFlop(h);
	if (rivRank.empty() &&table->getStage()==STAGE_RIVER) rivRank=RankFlop(h);

	for (unsigned int i=0; i<actionqueue.size(); i++) {
		if (!actionqueue[i].analyzed) {
			EvaluatePlay(i);
		}
	}

	AIDebug("End EvaluateStrategy");
*/
}

/* HoldemTournamentAIBase::EvaluatePlay() *******************************************/
void HoldemTournamentAIBase::EvaluatePlay(int play)
{
	AIDebug("EvaluatePlay");
	int ag[4]	= {0,0,0,0},
		ppl[4]	= {0,0,0,0};

	const int pf=0,f=1,t=2,r=3;
	int curstate=pf;

	// Estimate table conditions thus far
	for (int i=0; i<(signed)actionqueue.size()&&i<=play; i++)
	{
		switch (actionqueue[i].action)
		{
		case BET:
		case RAISE:
		case ALLIN:
			ag[curstate]++;
		case CALL:
			ppl[curstate]++;
		case FOLD:

/*			int checkStage=pf;
			for (int j=0; j<i; j++) {
				if (curstate==checkStage) {
					if (actionqueue[j].action==CHECK) {
					}
				}
				if (actionqueue[j].action==STAGE_CHANGE) {
					if (actionqueue[j].player==STAGE_FLOP ) checkStage=f;
					if (actionqueue[j].player==STAGE_TURN ) checkStage=t;
					if (actionqueue[j].player==STAGE_RIVER) checkStage=r;
				}
			}
*/
			break;

		case STAGE_CHANGE:
			switch (actionqueue[i].player)
			{
			case STAGE_FLOP:  curstate=f; break;
			case STAGE_TURN:  curstate=t; break;
			case STAGE_RIVER: curstate=r; break;
			}
		}
	}

	// Estimate card quality	
	double qmin=0,
		qmax=1;

	if (actionqueue[play].action!=FOLD&&actionqueue[play].action!=CHECK) {
		qmin=(table->getPlayersDead())*.04+.40;
		for (int i=0; i<ag[pf]; i++) {
			qmax=1;
			qmin-=(1-qmin)*.2;
		}
	}
	
	// Evaluate play
	if (actionqueue[play].action!=FOLD&&actionqueue[play].action!=CHECK) 
	switch (curstate)
	{
	case pf:
		playercards[actionqueue[play].player].SetAll(PHC_VALID);
		playercards[actionqueue[play].player].SetPercent(qmin,qmax);
		break;
	case f:
	case t:
	case r:
		if (actionqueue[play].action!=FOLD&&actionqueue[play].action!=CHECK) {
			PlayerHoleCards flopped=MatchedFlop();
			flopped.print(42,42);
			if (actionqueue[play].player<10&&actionqueue[play].player>0) {
				PlayerHoleCards tmp=playercards[actionqueue[play].player];
				tmp &= flopped;
				playercards[actionqueue[play].player]=tmp;
			}
			break;
		}
	}

	AIDebug("End EvaluatePlay");
}

/* HoldemTournamentAIBase::printPlayerCards() *********************************/
void HoldemTournamentAIBase::printPlayerCards(int p, int x, int y, int c, bool bw)
{
	AIDebug("printPlayerCards");

//	playercards[p] &= PHC_VALID;
//	playercards[p] |= handWinMask;
	playercards[p].print(x,y,c,bw);
}

/* HoldemTournamentAIBase::allin() ********************************************/
int HoldemTournamentAIBase::allin()
{
	return table->getStack(getAIPlayer());
}

/* HoldemTournamentAIBase::betAgg() *******************************************/
int HoldemTournamentAIBase::betAgg()
{
	return betPassive();
	/*
	int bet=0;
	
	if (table->getStage()==STAGE_PREFLOP&&getTotalRaises()==0) {
		if (table->getStack(getAIPlayer())<table->getBB()*4) {
			bet=allin();
		}
		else if (table->getBB()<=80) {
			bet=3*table->getBB();
		}
		else {
			bet=2*table->getBB();
		}
	}
	else {
		bet=table->getPot();
		bet/=10;
		bet*=10;

		if (table->getBB()>=320) {
			bet/=100;
			bet*=100;
		}
	}

	if (bet>=table->getStack(getAIPlayer())/2||table->getStack(getAIPlayer())<400) {
		bet=allin();
	}

	return bet;*/
}

/* HoldemTournamentAIBase::betPassive() ***************************************/
int HoldemTournamentAIBase::betPassive()
{
	int bet=0;
	
	/* eval normal bet strategy */
	if (table->getStage()==STAGE_PREFLOP&&getTotalRaises()==0) {
		if (table->getStack(getAIPlayer())<table->getBB()*4) {
			bet=allin();
		}
		else {
			if (table->getBB()<=80) {
				bet=3*table->getBB();
			}
			else {
				bet=2*table->getBB();
			}

			if (getAIPlayer()==table->getNextActivePlayer(table->getDealer())) { // sub sb if necessary
				bet-=table->getBB()/2;
			}
		}		
	}
	else {
		bet=table->getPot()/2;
		bet/=10;
		bet*=10;

		if (table->getBB()>=320) {
			bet/=100;
			bet*=100;
		}
	}

	/* eval special allin conditions */
	int originalBet=bet;
	for (int i=0; i<10; i++) { // FIXME: hardcoded number of players
		if (i!=getAIPlayer()&&table->isAlive(i)&&table->isActive(i)) {
			if (bet					>= table->getStack(i)/2	||
				table->getPot()		>= table->getStack(i)	||
				400					>= table->getStack(i)	||
				table->getBB()*3	>= table->getStack(i)	) {
				bet=table->getStack(i);
			}
		}
	}
	if (bet < originalBet) {
		bet=originalBet;
	}

	int enemyAdj=bet;
	if (bet					>= table->getStack(getAIPlayer())/3	||
		table->getPot()		>= table->getStack(getAIPlayer())/2	||
		400					>= table->getStack(getAIPlayer())	||
		table->getBB()*3	>= table->getStack(getAIPlayer())	) {
		bet=allin();
	}

	cur(40,0);
	cout << "betPassive: " << originalBet << " --> " << enemyAdj << " --> " << bet << "          " << endl;

	return bet;
}

/* HoldemTournamentAIBase::pushAction() ***************************************/
void HoldemTournamentAIBase::pushAction(PlayerAction a)
{
	AIDebug("pushAction");
	actionqueue.push_back(a);
}

/* HoldemTournamentAIBase::dbRegAct() *****************************************/
void HoldemTournamentAIBase::dbRegAct(const char* func, const char* sub, char action)
{
	AIStat ai;

	ai.action=action;
	ai.bb=table->getBB();
	ai.bet=0;
	ai.card_h1=table->getCard(0);
	ai.card_h2=table->getCard(1);
	ai.func=func;
	ai.players_active=table->getPlayersActive();
	ai.players_alive=table->getPlayersAlive();
	ai.pot=table->getPot();
	ai.stack=table->getStack(getAIPlayer());
	ai.stage=table->getStage();
	ai.sub=sub;
	ai.version="dev 5";
	TableTexture* t=new TableTexture();
	memset(t,0,sizeof(t));
	ai.t=*t;
	table->regAIStat(ai);
	delete t;
}

/* HoldemTournamentAIBase::getTableHandedness() *******************************/
int HoldemTournamentAIBase::getTableHandedness()
{
	int c=1;
	for (unsigned int i=0; i<actionqueue.size(); i++) {
		switch (actionqueue[i].action)
		{
		case FOLD: break;
		
		case BET:
		case ALLIN:
		case RAISE: 
			c=0;
			break;

		case CALL:
			c++;
		}
	}
	return c;
}

/* HoldemTournamentAIBase::getTotalRaises() ***********************************/
int HoldemTournamentAIBase::getTotalRaises()
{
	int c=0;
	for (unsigned int i=0; i<actionqueue.size(); i++) {
		switch (actionqueue[i].action)
		{
		case STAGE_CHANGE:
			c=0;
			break;

		case CALL:
		case FOLD: 
			break;
		
		case BET:
		case ALLIN:
		case RAISE: 
			c++;
			break;
		}
	}
	return c;
}

/* HoldemTournamentAIBase::getAggPlayer() *************************************/
int HoldemTournamentAIBase::getAggPlayer(int stage)
{
	int player=-1;
	bool curstage=stage==STAGE_PREFLOP;

	for (unsigned int i=0; i<actionqueue.size(); i++) {
		switch (actionqueue[i].action)
		{
		case STAGE_CHANGE:
			curstage=actionqueue[i].player==stage;
			break;

		case CALL:
		case FOLD: 
			break;
		
		case BET:
		case ALLIN:
		case RAISE:
			player=actionqueue[i].player;
			break;
		}
	}
	return player;
}

/* HoldemTournamentAIBase::calcHandWinMask() **********************************/
void HoldemTournamentAIBase::calcHandWinMask()
{	
	AIDebug("handWinMask");		if (table->getNewStage()) { 
	AIDebug("handWinMask1A");		handWinMask.SetAll(PHC_INVALID);
	AIDebug("handWinMask1B");		PokerHand h=table->getHand();
	AIDebug("handWinMask1C");		int mid=aiRank->findHand(h);
/*
	cout << "---------" << endl;

	cout << " Poker Hand: ["; h.print(); cout << "]" << endl;

	getch();
*/
	AIDebug("handWinMask2");		for (unsigned int i=0; i<(unsigned)mid&&i<aiRank->size(); i++) {
										handWinMask.setHandValid(((*aiRank)[i]).h[0],((*aiRank)[i]).h[1],PHC_LOSER);
									}
	AIDebug("handWinMask3");		for (unsigned int i=mid; i<aiRank->size(); i++) {
										handWinMask.setHandValid(((*aiRank)[i]).h[0],((*aiRank)[i]).h[1],PHC_WINNER);
									}
	AIDebug("end handWinMask");	}

	if (table->getAction()||table->getNewStage()) {
		for (int i=0; i<10; i++) {
			if (table->isActive(i)) {
				playercards[i]|=handWinMask;
			}
		}
	}
}

/* HoldemTournamentAIBase::calcActionMatrix() *********************************/
ActionMatrix HoldemTournamentAIBase::calcActionMatrix()
{
	ActionMatrix ret;
	memset(&ret,0,sizeof(ret));
	ret.betPot_callWin.probHap=1;
	ret.betPot_callLose.probHap=1;
	ret.betPot_win.probHap=1;

	PokerHand h=table->getCard(0)+table->getCard(1);
	
	/* get HandRankings */
	HandRankings* hr=&defRanks;
	if (table->getStage()==STAGE_FLOP)  hr=&flopRank;
	if (table->getStage()==STAGE_TURN)  hr=&turnRank;
	if (table->getStage()==STAGE_RIVER) hr=&rivRank;
	if (hr!=NULL) {
		aiRank=hr;
	}
	else {
		aiRank=&defRanks;
	}
	double per=FindHandPercent(*hr,h);

	/**** estimate personalities *****************************
	
	*********************************************************/

	/**** deltaCash ******************************************
	Calculate the cash needed for each action.
	TODO: manipulate bet sizes to force players to call/fold;
	*********************************************************/
	const double cash=table->getStack(getAIPlayer()); // Used in winResults stage
	double bet=table->getPot();
	if (table->getPot()*2>cash) { bet=cash; }

	ret.betPot_callWin.bet	= bet;
	ret.betPot_callLose.bet	= bet;
	ret.betPot_win.bet		= bet;
	ret.call_win.bet		= table->getCostToCall();
	ret.call_lose.bet		= table->getCostToCall();

	/**** bluff? *********************************************
	Here we calculate the chaces of the other players folding to a raise.
	TODO: account for personalities and stacks
	*********************************************************/
	
	/* Calculate individual play probabilities */
	double foldChance[10]; 
	for (int i=0; i<10; i++) {
		if (table->getStage()==STAGE_PREFLOP) {
			foldChance[i]=1-(table->getPlayersDead()*.03+.30);
		}
		else {
			foldChance[i]=1-(.80); // <== FIXME: Needs better formula
		}
		if (i==getAIPlayer()) {
			foldChance[i]=1;
		}
	}

	/* Calculate total probability */
	ret.betPot_win.probHap=1; 
	for (int i=0; i<10; i++) {
		if (table->isActive(i)&&i!=getAIPlayer()) {
			ret.betPot_win.probHap*=foldChance[i];
		}
	}

	/**** valueBet *******************************************
	Calc the chance that our hand will win/lose if our bet is 
	smooth called or we smooth call
	TODO: Needs stronger algorithm
	*********************************************************/
	double tmpBetWin=0;
	for (int i=0; i<10; i++) {
		if (table->isActive(i)) {
			tmpBetWin+=percentHandsBeaten(i);
		}
	}
	tmpBetWin/=table->getPlayersActive();

	ret.betPot_callWin.probHap=(1-ret.betPot_win.probHap)*tmpBetWin;
	ret.betPot_callLose.probHap=1-(ret.betPot_callWin.probHap+ret.betPot_win.probHap);

	ret.call_win.probHap=tmpBetWin;
	ret.call_lose.probHap=1-(ret.call_win.probHap);

	/**** winResults *****************************************
	Calculate the chance of us winning the tournament in 1st 
	for each alternative action/result hypothetical.
	TODO: needs to account for skill levels, stack sizes.
	A more advanced implimentation would take into account
	probable number of hands left, and chances of winning each.
	future round "implied" algorithm could be smarter
	*********************************************************/
	double implied=0;
	if (table->getStage()==STAGE_PREFLOP)	implied+=200;
	if (table->getStage()==STAGE_FLOP)		implied+=200;
	if (table->getStage()==STAGE_TURN)		implied+=100;

	ret.noActionWin				= (cash								  )/10000;
	ret.betPot_callWin.probWin	= (cash+table->getPot()+bet	  +implied)/10000;
	ret.betPot_callLose.probWin	= (cash-ret.betPot_callWin.bet-implied)/10000;
	ret.betPot_win.probWin		= (cash+table->getPot()				  )/10000;
	ret.call_win.probWin		= (cash+ret.betPot_callWin.bet+implied)/10000;
	ret.call_lose.probWin		= (cash-ret.betPot_callWin.bet-implied)/10000;

	return ret;
}

/* HoldemTournamentAIBase::RankFlop() *****************************************/
class AI_MatchedFlop
{
	PokerHand board;
public:
	AI_MatchedFlop(const PokerHand& b)//(const PokerHand& b)
	{
//		AIDebug("AI_MatchedFlop");
		board=b;
	};

	bool operator () (const HandRank_elem& a, const HandRank_elem& b)
	{
		PokerHand q=a.h+board;
		PokerHand r=b.h+board;
		return (r<q);
/*		if (q<r) return true;
		if (r<q) return false;

		for (int i=0; i<a.size(); i++) {
			if (a[0]<b[0]) return true;
		}
		return false;
		return (a<b);
*/	}
};

HandRankings HoldemTournamentAIBase::RankFlop(const PokerHand& b)
{
	AIDebug("RankFlop");
	HandRankings ret=defRanks;
/*
	PokerHand b;
	for (int i=2; i<7; i++) {
		b+=table->status.cards[i];
	}
*/
	sort(ret.begin(),
		ret.end(),
		AI_MatchedFlop(b));

	int cardsleft[13];
	for (int i=0; i<13; i++) 
		cardsleft[i]=4;

	for (int i=0; i<b.size(); i++)
		cardsleft[b[i].GetRank()]--;

	for (unsigned int i=0; i<ret.size(); i++) {
		ret[i].num=cardsleft[ret[i].h[0].GetRank()]*cardsleft[ret[i].h[1].GetRank()];
	}

	return ret;
}

/* HoldemTournamentAIBase::FindHandPercent() **********************************/
double HoldemTournamentAIBase::FindHandPercent(const HandRankings& hr, const PokerHand& b)
{
	AIDebug("FindHandPercent");
//	cout << "[" << table->status.stage << hr.size() << "]" ;

	int totalHands=0,
		handsAbove=0;

	unsigned int i;
	for (i=0; i<hr.size(); i++) {
		totalHands+=hr[i].num;
	}

	for (i=0; i<hr.size(); i++) {
		if (hr[i].h==b) break;
		handsAbove+=hr[i].num;
	}

	return (double(i)/hr.size());
}

/* HoldemTournamentAIBase::percentHandsBeaten() *******************************/
double HoldemTournamentAIBase::percentHandsBeaten(int player, bool print)
{
	AIDebug("percentHandsBeaten");

	if (!table->isActive(player)) return 1;

	int handsTotal=playercards[player].getNumHandsAbs(),
		handsAbove=playercards[player].getNumHandsWonAbs();

	if (print) {
		cout << "[" << handsAbove << "/" << handsTotal << "=" << handsAbove/(double)handsTotal << "]   ";
	}

	return handsAbove/(double)handsTotal;
}

/* HoldemTournamentAIBase::MatchedFlop() **************************************/
PlayerHoleCards HoldemTournamentAIBase::MatchedFlop()
{
	AIDebug("MatchedFlop");
	PlayerHoleCards ret;
	ret.ClrGroups();

	HandRankings* hr=&defRanks;
	if (table->getStage()==STAGE_FLOP)  hr=&flopRank;
	if (table->getStage()==STAGE_TURN)  hr=&turnRank;
	if (table->getStage()==STAGE_RIVER) hr=&rivRank;

	const double topPercent=.25;//+.02*table->getPlayersDead();
	if (hr->size()>1) {
//		for (unsigned int i=unsigned int(hr->size()*(1-topPercent)); i<hr->size(); i++) {
		for (unsigned int i=0; i<hr->size()*(topPercent)&&i<hr->size(); i++) {
			ret.setHandValid(((*hr)[i].h)[0],((*hr)[i].h)[1]);
		}
	}

	AIDebug("End MatchedFlop");
	return ret;
}

/* HoldemTournamentAIBase::GetStraightDraws() *********************************/
PlayerHoleCards HoldemTournamentAIBase::GetStraightDraws()
{
	AIDebug("GetStraightDraws");
	PlayerHoleCards ret;
	ret.ClrGroups();

	PokerHand h;
	h.print();
	for (int k=2; k<7; k++) {
		h+=table->getCard(k);
	}

	for (int i=0; i<13; i++) {
		for (int j=i; j<13; j++) {
			PokerHand tmp=h+Card(i)+Card(j);
			if (tmp.Straight(4)!=NOCARD) {
				ret.setHandValid(Card(i+13),Card(j));
			}
		}
	}

	AIDebug("End GetStraightDraws");
	return ret;
}

/* HoldemTournamentAIBase::GetPFQual() ****************************************/
int HoldemTournamentAIBase::GetPFQual()
{
	AIDebug("GetPFQual");
    int ret=0;

    if (table->getCard(0).GetRank()<table->getCard(1).GetRank())
    {
        Card c;
        c=table->getCard(0);
        table->setCard(table->getCard(1),0);
        table->setCard(c,1);
    }

    if (table->getCard(0).GetIndex()==table->getCard(1).GetIndex())
        ret|=PF_PAIR;

    if (table->getCard(0).GetSuit()==table->getCard(1).GetSuit())
        ret|=PF_SUITED;

    if ((table->getCard(0).GetRank()-1==table->getCard(1).GetRank()) ||
        (table->getCard(0).GetRank()+1==table->getCard(1).GetRank()))
        ret|=PF_CONNECTED;

    if ((table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='A') ||
        (table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='K') ||
        (table->getCard(0).GetIndex()=='Q'&&table->getCard(1).GetIndex()=='Q'))
        ret|=PF_ALLIN_A;
    else if
       ((table->getCard(0).GetIndex()=='J'&&table->getCard(1).GetIndex()=='J' )||
        (table->getCard(0).GetIndex()=='0'&&table->getCard(1).GetIndex()=='0' )||
        (table->getCard(0).GetIndex()=='9'&&table->getCard(1).GetIndex()=='9' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='K' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='Q'))
        ret|=PF_ALLIN_B;
    else if
       ((table->getCard(0).GetIndex()=='8'&&table->getCard(1).GetIndex()=='8' )||
        (table->getCard(0).GetIndex()=='7'&&table->getCard(1).GetIndex()=='7' )||
        (table->getCard(0).GetIndex()=='6'&&table->getCard(1).GetIndex()=='6' )||
        (table->getCard(0).GetIndex()=='5'&&table->getCard(1).GetIndex()=='5' )||
        (table->getCard(0).GetIndex()=='3'&&table->getCard(1).GetIndex()=='4' )||
        (table->getCard(0).GetIndex()=='2'&&table->getCard(1).GetIndex()=='3' )||
        (table->getCard(0).GetIndex()=='1'&&table->getCard(1).GetIndex()=='2' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='J' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='0' )||
        (table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='Q'))
        ret|=PF_ALLIN_C;

    if ((table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='A' )||
        (table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='K' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='K'&&ret&PF_SUITED))
        ret|=PF_GRP_A;
    if ((table->getCard(0).GetIndex()=='Q'&&table->getCard(1).GetIndex()=='Q' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='K' )||
        (table->getCard(0).GetIndex()=='J'&&table->getCard(1).GetIndex()=='J' )||
        (table->getCard(0).GetIndex()=='0'&&table->getCard(1).GetIndex()=='0'))
        ret|=PF_GRP_B;
    if ((table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='Q'&&ret&PF_SUITED) ||
        (table->getCard(0).GetIndex()=='9'&&table->getCard(1).GetIndex()=='9' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='Q' )||
        (table->getCard(0).GetIndex()=='8'&&table->getCard(1).GetIndex()=='8' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='J'&&ret&PF_SUITED))
        ret|=PF_GRP_C;
    if ((table->getCard(0).GetIndex()=='7'&&table->getCard(1).GetIndex()=='7' )||
        (table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='Q'&&ret&PF_SUITED )||
        (table->getCard(0).GetIndex()=='6'&&table->getCard(1).GetIndex()=='6' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='0'&&ret&PF_SUITED )||
        (table->getCard(0).GetIndex()=='5'&&table->getCard(1).GetIndex()=='5' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='J'))
        ret|=PF_GRP_D;
    if ((table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='Q' )||
        (table->getCard(0).GetIndex()=='4'&&table->getCard(1).GetIndex()=='4' )||
        (table->getCard(0).GetIndex()=='K'&&table->getCard(1).GetIndex()=='J'&&ret&PF_SUITED )||
        (table->getCard(0).GetIndex()=='3'&&table->getCard(1).GetIndex()=='3' )||
        (table->getCard(0).GetIndex()=='2'&&table->getCard(1).GetIndex()=='2' )||
        (table->getCard(0).GetIndex()=='A'&&table->getCard(1).GetIndex()=='0' )||
        (table->getCard(0).GetIndex()=='Q'&&table->getCard(1).GetIndex()=='J'&&ret&PF_SUITED))
        ret|=PF_GRP_E;

    return ret;
}


