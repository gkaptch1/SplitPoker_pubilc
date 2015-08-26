
/* #includes ******************************************************************/

#include "HoldemTournamentAI.h"

/* Constructer / Destructer ***************************************************/
HoldemTournamentAI::HoldemTournamentAI(MsgQueueBase* q):HoldemTournamentAIBase(q)
{
}

HoldemTournamentAI::~HoldemTournamentAI()
{
}

/* HoldemTournamentAI::getPlay() **********************************************/
ActionRet HoldemTournamentAI::getPlay()
{
	AIDebug("getPlay");
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
	
	return ActionRet();

	/* getPlay ********************************************
	Evaluate our possible plays, and make a decission based
	on their profitability.
	******************************************************/
	ActionMatrix ret=calcActionMatrix();

	double noActionWin=
		ret.noActionWin;
	double betWin=
		ret.betPot_callWin.probHap	*ret.betPot_callWin.probWin+
		ret.betPot_callLose.probHap	*ret.betPot_callLose.probWin+
		ret.betPot_win.probHap		*ret.betPot_win.probWin;
	double callWin=
		ret.call_win.probHap		*ret.call_win.probWin+
		ret.call_lose.probHap		*ret.call_lose.probWin;

	char play=FOLD;

	/* print player info */
	cout << table->getName(getAIPlayer()) << " (" << table->getStack(getAIPlayer()) << "):  ["; h.print(); cout << "] " << play << "\t\t\t" << endl;

	cout << "  bW [ " << ret.betPot_callWin.probHap   << ",\t" << ret.betPot_callWin.probWin  << ",\t" << ret.betPot_callWin.bet	<< " ]          " << endl;
	cout << "  bL [ " << ret.betPot_callLose.probHap  << ",\t" << ret.betPot_callLose.probWin << ",\t" << ret.betPot_callLose.bet	<< " ]          " << endl;
	cout << "  w  [ " << ret.betPot_win.probHap		  << ",\t" << ret.betPot_win.probWin	  << ",\t" << ret.betPot_win.bet		<< " ]          " << endl;
	cout << "  cW [ " << ret.call_win.probHap		  << ",\t" << ret.call_win.probWin		  << ",\t" << ret.call_win.bet			<< " ]          " << endl;
	cout << "  cL [ " << ret.call_lose.probHap		  << ",\t" << ret.call_lose.probWin		  << ",\t" << ret.call_lose.bet			<< " ]          " << endl << endl;

	cout << "  noAct: " << noActionWin	<< endl;
	cout << "  bet:   " << betWin		<< endl;
	cout << "  call:  " << callWin		<< endl;

//	return ret;

	/* get TournamentSkills 
	What follows is the "old way" of doing things.  It was tuned into a specific 
	aspect of tournament strategy, but couldn't win because of the other aspects.
	*/
	double per=FindHandPercent(*hr,h);

	/* get play */
	if ((table->getStage()==STAGE_PREFLOP)&&((table->getPlayersDead()*.05+.10)>per)) play=RAISE;
	else if ((table->getStage()!=STAGE_PREFLOP)&&(table->getPlayersDead()*.03+.30)>per) play=RAISE;
	else play=FOLD;

	/* print results */
	cout << (table->getPlayersDead()*.05+.10) << "  " << per << "  " << (char)play << "\t\t\t" << endl;
	if (strcmp(table->getName(table->getActionPlayer()),"moniker")==0)	{
		PokerHand q=table->GetHoleCards(table->getNameIndex("moniker"));
//		cout << table->getNameIndex("moniker") << endl;
//		cout << table->GetNameIndexTrue(table.getNameIndex("moniker")) << endl;
		table->getCard(0)=q[0];
		table->getCard(1)=q[1];
	}

	/* Determine Amount */
	cout << "Player cards: "; h.print(); cout << " " << aiRank->findHand(h) << endl;
	if (h[0]==NOCARD||h[1]==NOCARD) {cout << endl << endl;}
	else {
		cout << "Rank pf: " << FindHandPercent(defRanks,h)	<< "\t\t" << endl;
		cout << "Rank f:  " << FindHandPercent(flopRank,h)	<< "\t\t" << endl;
		cout << "Rank t:  " << FindHandPercent(turnRank,h)	<< "\t\t" << endl;
		cout << "Rank r:  " << FindHandPercent(rivRank,h)	<< "\t\t" << endl;
		cout << endl << endl;
	}

	cout << endl << endl;
//	return play;
}