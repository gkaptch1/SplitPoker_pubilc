
/* #includes ******************************************************************/

#include "stdafx.h"
#include "HoldemTournamentAI.h"
#include "TournamentData.h"

/* Constructer / Destructer ***************************************************/
HoldemTournamentAIManual::HoldemTournamentAIManual(MsgQueueBase* q):HoldemTournamentAIBase(q)
{
}

HoldemTournamentAIManual::~HoldemTournamentAIManual()
{
}

/* HoldemTournamentAIManual::getPlay() **********************************************/
#define endl "                         \n"
ActionRet HoldemTournamentAIManual::getPlay()
{
	int baseY=20;

	ActionRet ret;
	ret.name="none";
	ret.action=FOLD;
	ret.amt=betAgg();

	PokerHand h = table->getHand();
	const int player=getAIPlayer();

	TableTexture t=evalTexture();

	/* print info *********************/
	cur (40,baseY);
	color (CUR_WHITE);
	cout << "Agg: ";
	if (t.uberDesperate)	{ color(CUR_RED); cout << " [ UBER DESPERATE ] " << endl; color(CUR_WHITE); }
	else if (t.desperate)	{ color(CUR_RED); cout << " [ Desperate ] " << endl; color(CUR_WHITE); }
	else if (t.medAgg)		cout << " [ Med ] " << endl;
	else if (t.uberAgg)		cout << " [ Uber ] " << endl;
	else					cout << " [ Normal ] " << endl;

	cur (0,baseY+1);
	cout << table->getName(getAIPlayer()) << " (" << table->getStack(getAIPlayer()) << "):  ["; h.print(); cout << "] " << "\t\t\t" << endl << endl;


	/* make play **********************/

	/*
	Heads up is a very unique situation which requires a lot of aggression;
	handle this situation seperately
	*/
	ret=headsUp(t);

	/*
	If ret is set to something other than FOLD, then we are done;
	otherwise, we must analyze to see what the best move is.
	Depending on what stage the hand is in, the AI has a certain
	number of "moves" that it can perform.  These moves should
	be fairly self explainable from their function names.

	note: pf=PreFlop
	*/
	switch (table->getStage())
	{
	case STAGE_PREFLOP:
//#define __ALSPACH
#ifdef __ALSPACH
		if (ret.action==FOLD) ret=pfAlspachMethod(t);
#else
		//if (ret.action==FOLD) ret=headsUp(t);
		if (ret.action==FOLD) ret=pfLimp(t);		// call with weak cards, hoping to later hit a set, straight, or flush
		if (ret.action==FOLD) ret=pfBlindSteal(t);	// raise based soley on position
		if (ret.action==FOLD) ret=pfValuePlay(t);	// bet if we have a good hand
#endif
		break; 

	case STAGE_FLOP:
		if (ret.action==FOLD) ret=chaseDraws(t);	// do pot odds make it reasonable?
		if (ret.action==FOLD) ret=checkSteal(t);	// its been checked down to us, a bet might win uncontested
		if (ret.action==FOLD) ret=continueBet(t);	// if we bet big pf, then good chance a large bet here will win
		if (ret.action==FOLD) ret=probeBet(t);		// gather information about the opponents' hand
		if (ret.action==FOLD) ret=valuePlay(t);		// bet if we have a good hand
		break; 

	case STAGE_TURN: 
		if (ret.action==FOLD) ret=chaseDraws(t);	// do pot odds make it reasonable?
		if (ret.action==FOLD) ret=valuePlay(t);		// bet if we have a good hand
		break;

	case STAGE_RIVER: 
		if (ret.action==FOLD) ret=valuePlay(t);		// bet if we have a good hand
		break;
	}

	/*
	if we are desperate (defined by our stack/blinds/pot ratios), then we
	should turn a call into an allin.  
	Basically, we're already pot committed, so we have to go for gold.
	Also, prevents us from having to worry about "tricky" folds later on.
	*/
	if (t.desperate||t.uberAgg||t.uberDesperate) {
		if (ret.action==CALL) {
			ret.sub=ret.name;
			ret.name="nocall";
			ret.action=BET;
			ret.amt=allin();
		}
	}

	/* print decisions ****************/
	cur(40,baseY+1);
	cout << "stage: " << (char)table->getStage() << "  act: " << (char)ret.action << " " << ret.amt << endl;

	if (ret.action==FOLD) {
		ret.name="none";
		ret.sub="";
	}
	if (table->getActionPlayer()==getAIPlayer()) {
		dbRegAct(ret,t);
	}

	return ret;
}

/* HoldemTournamentAIManual::dbRegAct() *********************************************/
void HoldemTournamentAIManual::dbRegAct(const ActionRet& r, const TableTexture& t)
{
	AIStat ai;

	ai.action=r.action;
	ai.bb=table->getBB();
	ai.bet=r.amt;
	ai.card_h1=table->getCard(0);
	ai.card_h2=table->getCard(1);
	ai.func=r.name;
	ai.players_active=table->getPlayersActive();
	ai.players_alive=table->getPlayersAlive();
	ai.pot=table->getPot();
	ai.stack=table->getStack(getAIPlayer());
	ai.stage=table->getStage();
	ai.sub=r.sub;
	ai.version="dev 5";
	ai.t=t;
	table->regAIStat(ai);
}

/* HoldemTournamentAIManual::evalTexture() ******************************************/
TableTexture HoldemTournamentAIManual::evalTexture(bool debug)
{
	TableTexture ret;
	memset(&ret,0,sizeof(ret));
	PokerHand tmpHand;

	/* general */
	ret.potodds=table->getPotOdds(getAIPlayer());

	if (table->getNumHands()>1) {
		if (table->getStack(getAIPlayer())<300+3*table->getBB()) ret.desperate=true;
		if (table->getStack(getAIPlayer())<3*table->getBB())ret.uberDesperate=true;
		if (table->getPlayersAlive()<=6&&table->getBB()>=80) ret.medAgg=true;
		if (table->getPlayersAlive()<=4&&table->getBB()>=320) ret.uberAgg=true;
	}

	/* flop */
	tmpHand=table->getHand()+table->getBoard();

	if (tmpHand.isFlush(4))				ret.flushdraw=true;
	if (tmpHand.Straight(4)!=NOCARD)	ret.straightdraw=true;

	tmpHand=table->getBoard();
	ret.suited2=tmpHand.isFlush(2);
	ret.suited3=tmpHand.isFlush(3);
	ret.suited4=tmpHand.isFlush(4);
	ret.suited5=tmpHand.isFlush(5);
	ret.tripcard=tmpHand.Trip();
	ret.paircard=tmpHand.Pair();
	ret.trips=(ret.tripcard!=NOCARD);
	ret.pair =(ret.paircard!=NOCARD);

	tmpHand=table->getBoard();
	ret.inrow5=(tmpHand.Straight(5)==NOCARD);
	ret.inrow4=(tmpHand.Straight(4)==NOCARD);
	ret.inrow3=(tmpHand.Straight(3)==NOCARD);
	ret.inrow2=(tmpHand.Straight(2)==NOCARD);

	for (ret.facecards=0; tmpHand.HighCard().GetRank()>=9; ret.facecards++);
	
	ret.totalaction	=getTotalRaises();
	ret.activebefore=table->getPositionBeforeActive();
	ret.activeafter	=table->getPositionAfterActive();
	ret.activetotal =(ret.activeafter+ret.activebefore);

	/* Turn */
	ret.flushscare=
		(table->getFlop().isFlush(2)&&(table->getFlop()+table->getTurn()).isFlush(3)) ||	// turn brings third to a flush
		(table->getFlop().isFlush(3)&&(table->getFlop()+table->getTurn()).isFlush(4));		// turn brings fourth to a flush

	ret.straightscare=
		((table->getFlop().Straight(2)!=NOCARD)&&(table->getFlop()+table->getTurn()).Straight(3)!=NOCARD) ||	// turn brings third to a straight
		((table->getFlop().Straight(3)!=NOCARD)&&(table->getFlop()+table->getTurn()).Straight(4)!=NOCARD);		// turn brings fourth to a straight

	tmpHand=table->getBoard();
	ret.overcard=(tmpHand.HighCard()==table->getTurn());

	ret.dangercard=ret.flushscare||ret.straightscare;//||overcard;

	if (debug) {
		cout << " 2 suited: "	<< ret.suited2		<< endl;
		cout << " 3 suited: "	<< ret.suited3		<< endl;
		cout << " 4 suited: "	<< ret.suited3		<< endl;
		cout << " 5 suited: "	<< ret.suited3		<< endl;
		cout << " faces: "		<< ret.facecards	<< endl;
		cout << " trip: "		<< ret.trips		<< endl;
		cout << " pair: "		<< ret.pair			<< endl;
		cout << " 2 in row: "	<< ret.inrow2		<< endl;
		cout << " 3 in row: "	<< ret.inrow3		<< endl;
		cout << " 4 in row: "	<< ret.inrow3		<< endl;
		cout << " 5 in row: "	<< ret.inrow3		<< endl;
		cout << " action: "		<< ret.totalaction	<< endl;
		cout << " pos: "		<< ret.activebefore << " + " << ret.activeafter << " = " << ret.totalaction << endl;
		cout << endl;
		
		cout << "dangerCard: "  << ret.dangercard << endl;
		cout << " flush++: "    << ret.flushscare << endl;
		cout << " straight++: " << ret.straightscare << endl;
		cout << " overcard: "   << ret.overcard << endl;
		cout << endl;
	}

	return ret;
}

/* HoldemTournamentAIManual::pfLimp() *******************************************/
ActionRet HoldemTournamentAIManual::pfLimp(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="pfLimp";
	ret.action=FOLD;
	ret.amt=0;
	
	profile_line(" -- pfLimp() -- ");
	string debugstr="";
	PokerHand h=table->getHand();

	bool limp=false;
	int relStack=table->getStack(getAIPlayer())/double(table->getCostToCall())<10;
	if (relStack<20&&h[0].GetSuit()==h[1].GetSuit()) {
		debugstr=" [s] ";
		//limp=true;
	}
	if (relStack<20&&abs(h[0].GetRank()-h[1].GetRank())==1) {
		debugstr=" [c] ";
		//limp=true;
	}
	if (abs(h[0].GetRank()-h[1].GetRank())==1&&h[0].GetSuit()==h[1].GetSuit()) {
		debugstr=" [sc] ";
		if (h[0].GetIndex()=='A'||h[0].GetIndex()=='K') {
			limp=false; // should limp half the time with KQs
		}
		else {
			limp=true;
		}
	}
	if (relStack<20&&h[0].GetRank()==h[1].GetRank()&&h[1].GetRank()<9) {
		debugstr=" [p] ";
		limp=true;
	}

	if (getTableHandedness()<2) limp=false;
	if (t.desperate||t.uberDesperate||t.medAgg||t.uberAgg||table->getBB()>=80) {
		limp=false;
	}

	if (debug) {
		cout << "limp:  " << limp  << endl;
		cout << " handedness: " << getTableHandedness() << endl;
		cout << " stack/cost2call: " << table->getStack(getAIPlayer())/double(table->getCostToCall()) << debugstr << endl;
		cout << endl;
	}

	if (limp) {
		ret.action=CALL;
		ret.amt=table->getCostToCall(getAIPlayer());
	}

	return ret;
}

/* HoldemTournamentAIManual::pfBlindSteal() ***************************************/
ActionRet HoldemTournamentAIManual::pfBlindSteal(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="pfBlindSteal";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- pfBlindSteal() -- ");

	PokerHand h=table->getHand();

	bool steal=true;
	if (t.totalaction>=1) steal=false;
	if (getTableHandedness()>1) steal=false;
	if (t.activeafter>1) steal=false;
	bool cashGood=true;
	for (int i=0; i<10; i++) {
		if (table->isActive(i)) {
			if (table->getStack(i)<500||table->getStack(i)<table->getBB()*2) {
				cashGood=false;
			}
		}
	}
	if (!cashGood) steal=false;
	bool cardsGood=false;
	if (!t.desperate&&(h[0].GetRank()==h[1].GetRank()||h[0].GetRank()>10||h[1].GetRank()>10)) cardsGood=true;
	if ( t.desperate&&(h[0].GetRank()==h[1].GetRank()||h[0].GetRank()>9 ||h[1].GetRank()>9 )) cardsGood=true;
	if (   t.uberAgg&&(h[0].GetRank()==h[1].GetRank()||h[0].GetRank()>10||h[1].GetRank()>10)) cardsGood=true;
	if (!cardsGood) steal=false;

	if (table->getBB()==20) steal=false;

	if (debug) {
		cout << "steal: " << steal << endl;
		cout << " cost2call: " << table->getCostToCall(getAIPlayer()) << endl;
		cout << " afterActive: " << table->getPositionAfterActive() << endl;
		cout << " cardsGood: " << cardsGood << endl;
		cout << " cashGood: " << cashGood << endl;
		cout << endl;
	}

	if (steal) {
		ret.action=BET;
		ret.amt=betAgg();
	}

	return ret;
}

/* HoldemTournamentAIManual::pfValuePlay() ***************************************/
ActionRet HoldemTournamentAIManual::pfValuePlay(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="pfValuePlay";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- pfValuePlay -- ");

	bool agg=false,
		valCall=false;

	PokerHand h=table->getHand();

	int handVal=defRanks.findHand(h);
	int minHandVal=5+(table->getPositionBeforeAbs())+2*table->getPlayersDead();
	if (t.desperate)	minHandVal+=10+2*(table->getPlayersAlive()-2-table->getHandsUntilBB(getAIPlayer()));
	if (t.medAgg)		minHandVal+=5;
	if (t.uberAgg)		minHandVal+=10;
	int minHandValTrue=minHandVal;
	if (getTotalRaises()>0&&!t.desperate&&!t.uberAgg) minHandValTrue=6;
	if (getTotalRaises()>0&&!t.desperate&&!t.uberAgg&&table->getPotOdds()<3) minHandVal/=1+getTotalRaises();
	agg=handVal<=minHandValTrue;
	valCall=!agg&&handVal<minHandVal;
	
	if (table->isEveryPlayerAllin()) {
		if (table->getPotOdds()>=3) {
			agg=true;
		}
	}

	if (debug) {
		cout << "agg:   " << agg << endl;
		cout << "valC:  " << valCall << endl;
		cout << " Hand Value: " << handVal << endl;
		cout << " Desp Adj: " << table->getPlayersAlive()-table->getHandsUntilBB(getAIPlayer()) << endl;
		cout << " Playable Hands: " << minHandVal << endl;
		cout << " Raisable Hands: " << minHandValTrue << endl;
		cout << endl;
	}

	if (agg) {
		if ((h[0].GetIndex()=='0'&&h[1].GetIndex()=='J') ||
			(h[1].GetIndex()=='0'&&h[0].GetIndex()=='J')) {
			agg=false;
			valCall=true;
			static int tmpCtr=0;
			tmpCtr++;
			cur(0,11) << "10 J fool prevention: " << tmpCtr << endl;
		}
	}

	if (agg) {
		ret.action=BET;
		ret.amt=betAgg();
		ret.sub="agg";
	}
	else if (valCall) {
		ret.action=CALL;
		ret.sub="call";
		ret.amt=table->getCostToCall(getAIPlayer());
	}

	return ret;
}

/* HoldemTournamentAIManual::pfAlspachMethod() **************************************/
ActionRet HoldemTournamentAIManual::pfAlspachMethod(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="pfValuePlay";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- pfAlspachMethod -- ");

	/* build ranking lists */

	return ret;
}

/* HoldemTournamentAIManual::headsUp() **********************************************/
ActionRet HoldemTournamentAIManual::headsUp(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="headsUp";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- headsUp() -- ");

	if (table->getPlayersAlive()==2&&table->getBB()>=320) {
		ret.action=BET;
		ret.amt=allin();
	}

	return ret;
}

/* HoldemTournamentAIManual::chaseDraws() *******************************************/
ActionRet HoldemTournamentAIManual::chaseDraws(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="chaseDraws";
	ret.action=FOLD;
	ret.amt=0;
	
	profile_line(" -- chaseDraws() -- ");

	bool chasedraw=false;
	if (t.flushdraw&&table->getPotOdds(table->getCostToCall(getAIPlayer()))>4) chasedraw=true;
	if (t.straightdraw&&table->getPotOdds(table->getCostToCall(getAIPlayer()))>5) chasedraw=true;
	if (t.flushdraw&&t.straightdraw&&table->getPotOdds(table->getCostToCall(getAIPlayer()))>2) chasedraw=true;

	if (table->getBB()>=320) chasedraw=false;

	if (chasedraw) {
		ret.action=CALL;
		ret.amt=table->getCostToCall(getAIPlayer());
	}

	if (debug) {
		cout << "chase: " << chasedraw << endl;
		cout << " flushes: " << t.flushdraw << endl;
		cout << " straights: " << t.straightdraw << endl;
		cout << " pot odds: " << table->getPotOdds(table->getCostToCall(getAIPlayer())) << endl;
		cout << endl;
	}

	return ret;
}

/* HoldemTournamentAIManual::checkSteal() ****************************************/
ActionRet HoldemTournamentAIManual::checkSteal(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="checkSteal";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- checkSteal() -- ");

	bool stealpos=(table->getPlayersActive()<=3)&&(table->getPositionAfterActive(getAIPlayer())==0);

	int player1=table->getNextActivePlayer(getAIPlayer());
	double p1perFold=table->getCheckAct(player1,FOLD)/(double)table->getCheckAct(player1);
	if (table->getCheckAct(player1,RAISE)!=0) p1perFold=0;
	if (table->getCheckAct(player1)==0) p1perFold=0;

	int player2=table->getNextActivePlayer(player1);
	double p2perFold=table->getCheckAct(player2,FOLD)/(double)table->getCheckAct(player2);
	if (table->getCheckAct(player2,RAISE)!=0) p2perFold=0;
	if (table->getCheckAct(player2)==0) p2perFold=0;
	if (player2==getAIPlayer()) p2perFold=1;

	bool checksteal=stealpos&&(p1perFold>=.5)&&(p2perFold>=.5)&&t.totalaction==0;

	if (debug) {
		cout << "checksteal: " << checksteal << endl;
		cout << " stealpos: " << stealpos << endl;
		cout << " p1 %fold: " << p1perFold << endl;
		cout << " p1 %fold: " << p2perFold << endl;
		cout << endl;
	}

	if (checksteal) {
		ret.action=BET;
		ret.amt=betPassive();
	}

	return ret;
}

/* HoldemTournamentAIManual::continueBet() ****************************************/
ActionRet HoldemTournamentAIManual::continueBet(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="continueBet";
	ret.action=FOLD;
	ret.amt=0;
	
	profile_line(" -- continueBet() -- ");

	bool continueCards=(t.facecards<=1);
	bool noAction=getTotalRaises()==0;
	bool headsUp=table->getPlayersActive()==2;
	bool PFAgg=getAggPlayer(STAGE_PREFLOP)==getAIPlayer();
	bool continuebet=continueCards&&noAction&&PFAgg&&headsUp;

	if (debug) {
		cout << "continuebet: " << continuebet << endl;
		cout << " continue flop: " << continueCards << endl;
		cout << " no action: " << noAction << endl;
		cout << " heads up: " << headsUp << endl;
		cout << " pf bet: " << PFAgg << endl;
		cout << endl;
	}

	if (continuebet) {
		ret.action=BET;
		ret.amt=betPassive();
	}
	return ret;
}

/* HoldemTournamentAIManual::probeBet() ****************************************/
ActionRet HoldemTournamentAIManual::probeBet(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="probeBet";
	ret.action=FOLD;
	ret.amt=0;

	profile_line(" -- probeBet() -- ");

	PokerHand tmpHand=table->getBoard(),
		h=table->getHand();

	bool poorHand=false;
	bool noAction=(getTotalRaises()==0);
	bool headsUp=(table->getPlayersActive()<=2);
	while (tmpHand.size()>0) {
		Card tmp=tmpHand.HighCard();
		if (tmp.GetRank()==h[0].GetRank()) poorHand=true;
		if (tmp.GetRank()==h[1].GetRank()) poorHand=true;
	}
	if (h[0].GetRank()==12||h[1].GetRank()==12||h[0].GetRank()==h[1].GetRank()) poorHand=true;
	
	bool probebet=poorHand&&noAction&&headsUp;

	if (debug) {
		cout << "probe bet: " << probebet << endl;
		cout << " probe-able flop: " << poorHand << endl;
		cout << " heads up/!action: " << (noAction&&headsUp) << endl;
		cout << endl;
	}

	if (probebet) {
		ret.action=BET;
		ret.amt=betPassive();
	}

	return ret;
}

/* HoldemTournamentAIManual::valuePlay() ********************************************/
ActionRet HoldemTournamentAIManual::valuePlay(const TableTexture& t, bool debug)
{
	ActionRet ret;
	ret.name="valuePlay";
	ret.action=FOLD;
	ret.amt=0;
	
	PokerHand h=table->getHand();
	PokerHand tmpHand;

	bool medHand=false,
		goodHand=false,
		nutzHand=false;

	bool valuebet=false,
		slowplay=false;

	profile_line(" -- valuePlay() -- ");
	
	/* adjust for flush probs *********/
	bool flushAce=false;
	if (t.suited4||t.suited5) {
		tmpHand=table->getBoard();
		char suit=tmpHand.Flush()[0].GetSuit();
		if (h[0].GetSuit()==suit&&h[0].GetIndex()=='A') flushAce=true;
		if (h[1].GetSuit()==suit&&h[1].GetIndex()=='A') flushAce=true;
	}
	tmpHand=table->getBoard()+table->getHand();
	bool flush=tmpHand.isFlush()&&(flushAce||!(t.suited4||t.suited5));

	/* eval cards *********************/
	if (!t.trips&&!t.pair) {
		profile_line("Simple Board");
		tmpHand=table->getBoard();
		Card boardHigh=tmpHand.HighCard();
		if (h[0].GetRank()==boardHigh.GetRank()||h[1].GetRank()==boardHigh.GetRank()) toggleVar(medHand); // Top Pair
		if (h[0].GetRank()==h[1].GetRank()&&h[0].GetRank()>boardHigh.GetRank())       toggleVar(medHand); // Over Pair
		tmpHand=table->getBoard()+table->getHand();
		Card trip=tmpHand.Trip();
		Card p1=tmpHand.Pair();
		Card p2=tmpHand.Pair();
		if (trip!=NOCARD||(p1!=NOCARD&&p2!=NOCARD)) {
			toggleVar(goodHand);
		}
		tmpHand=table->getBoard()+table->getHand();
		if (tmpHand.Quad()!=NOCARD||flush||(tmpHand.Trip()!=NOCARD&&tmpHand.Pair()!=NOCARD)) {
			toggleVar(nutzHand);
		}
		if (table->getStage()==STAGE_FLOP) {
			if (tmpHand.Straight()!=NOCARD&& t.suited2) { toggleVar(goodHand); }
			if (tmpHand.Straight()!=NOCARD&&!t.suited2) { toggleVar(nutzHand); }
		}
		if (table->getStage()==STAGE_TURN||table->getStage()==STAGE_RIVER) {
			if (tmpHand.Straight()!=NOCARD&& t.suited3) { toggleVar(goodHand); }
			if (tmpHand.Straight()!=NOCARD&&!t.suited3) { toggleVar(nutzHand); }
		}
	}
	else if (!t.trips) {
		profile_line("Pair Board");
		tmpHand=table->getBoard();
		Card pair=tmpHand.Pair();
		Card pair2=tmpHand.Pair();
		Card loneCard=tmpHand.HighCard();
		if (loneCard.GetRank()==h[0].GetRank()||loneCard.GetRank()==h[1].GetRank()) toggleVar(medHand); // Board two Pair
		if (h[0].GetRank()==h[1].GetRank()&&h[0].GetRank()>loneCard.GetRank()) toggleVar(medHand); // Over Pair
		if (h[0].GetRank()==pair.GetRank() ||h[1].GetRank()==pair.GetRank()||
			h[0].GetRank()==pair2.GetRank()||h[1].GetRank()==pair2.GetRank()) toggleVar(goodHand); //trips or boat
		tmpHand=table->getBoard()+table->getHand();
		if (tmpHand.Quad()!=NOCARD||flush||(tmpHand.Trip()!=NOCARD&&tmpHand.Pair()!=NOCARD)) {
			toggleVar(nutzHand);
		}
		if (table->getStage()==STAGE_FLOP) {
			if (tmpHand.Straight()!=NOCARD&& t.suited2) { toggleVar(goodHand); }
			if (tmpHand.Straight()!=NOCARD&&!t.suited2) { toggleVar(nutzHand); }
		}
		if (table->getStage()==STAGE_TURN||table->getStage()==STAGE_RIVER) {
			if (tmpHand.Straight()!=NOCARD&& t.suited3) { toggleVar(goodHand); }
			if (tmpHand.Straight()!=NOCARD&&!t.suited3) { toggleVar(nutzHand); }
		}
	}
	else {
		profile_line("Weird Board");
		tmpHand=table->getBoard()+table->getHand();
		if (flush||(tmpHand.Trip()!=NOCARD&&tmpHand.Pair()!=NOCARD)) toggleVar(goodHand);
		if (tmpHand.Quad()!=NOCARD) toggleVar(nutzHand);
	}

	/* eval plays *********************/
	switch (table->getStage())
	{
	case STAGE_FLOP:
		if ((goodHand&&(t.inrow3)&&(t.inrow2)&&!t.suited2)||(nutzHand)) { slowplay=true; }
		if (medHand||goodHand||nutzHand) { valuebet=true; }
		break;

	case STAGE_TURN:
	case STAGE_RIVER:
		if (!t.dangercard&&(medHand||goodHand||nutzHand)) { valuebet=true; }
		if ( t.dangercard&&(         goodHand||nutzHand)) { valuebet=true; }
	}

	/* debug **************************/
	if (debug) {
		cout << "valuebet: " << valuebet << endl;
		cout << " med hand: " << medHand << endl;
		cout << " good hand: " << goodHand << endl;
		cout << " nutz hand: " << nutzHand << endl;
		cout << endl;
	}

	/* make play **********************/
	if (valuebet) {
		ret.action=BET;
		ret.amt=betPassive();
	}
	if (slowplay) {
		ret.action=CALL;
		ret.amt=table->getCostToCall(getAIPlayer());
	}

	return ret;
}

#undef endl


