#ifndef _HoldemTournamentAI_H_
#define _HoldemTournamentAI_H_

/* Prototypes *****************************************************************/

#include "Poker/poker.h"
#include "PlayerHoleCards.h"
#include "MsgQueue.h"

/* Macros *********************************************************************/
#define PASSIVE_SLOW            (passivity>30)
#define PASSIVE_MED             (passivity>=0)
#define PASSIVE_FAST            (passivity<0)

#define POSITION_GOOD           (pos>2)
#define POSITION_MED            (!POSITION_GOOD&&!POSITION_BAD)
#define POSITION_BAD            (pos<1)

#define AIDebug(x)				/*uberDebug(x);*/

/* structs ********************************************************************/
struct PlayerStrat
{
	double bluff,
		semibluff,
		draw,
		protectHand,
		vulnHand,
		nutz,
		strong;
};

struct PlayerAction
{
	int action,
		amt,
		player;

	bool analyzed;
};

struct ActionRet
{
	string name,sub;

	int action,
		amt;
};

struct TableTexture
{
	/* general */
	double potodds;

	bool desperate,
		uberDesperate,
		medAgg,
		uberAgg;

	/* flop */
	bool straightdraw;
	bool flushdraw;
	
	bool suited2,
		suited3,
		suited4,
		suited5,
		trips,
		pair,
		inrow2,
		inrow3,
		inrow4,
		inrow5;

	int totalaction,
		activebefore,
		activeafter,
		activetotal;

	BotCard tripcard,
		paircard;

	int facecards;

	/* turn */
	bool flushscare,
		straightscare,
		overcard,
		dangercard;
};

/* Action structs */

struct HypotheticalAction
{
	double probHap, probWin, bet;
};

struct ActionMatrix
{
	double noActionWin;

	HypotheticalAction
		betPot_callWin,
		betPot_callLose,
		betPot_win;

	HypotheticalAction
		call_win,
		call_lose;
};

ostream& operator << (ostream &out, PlayerAction &p); 

/* class HoldemTournamentAIBase ***********************************************/
class HoldemTournamentAIBase
{
protected:
	/* communication */
    MsgQueueBase* table;
	char* playerName;

	string moniker;
	string aiverstr;
	int aiverint;

	/* debug log */
	bool watchOnly;

//	ofstream profiler;

//#define profile_line(x) profiler << "Game " << table->getGameID() << ", Hand #" << table->getNumHands() << " [ " << table->getHand().toString() << " - " << table->getBoard().toString() << " ], " << /*" File " << __FILE__ <<*/ " (" << __LINE__ << ") :  " << (x) << endl";
#define toggleVar(v) {profile_line(#v); v=true;}

	/* strategy */
	PlayerHoleCards playercards[10];
	PlayerHoleCards handWinMask;
	PlayerStrat strat[10];
	
	vector<PlayerAction>	actionqueue;

	HandRankings* aiRank;
	HandRankings flopRank,turnRank,rivRank;
    int passivity,pos;

protected:
	/* strategy */
	double FindHandPercent(const HandRankings& hr, const PokerHand& b);
	HandRankings RankFlop(const PokerHand& b);
	PlayerHoleCards MatchedFlop();

	bool FlushDraw(const PokerHand& h) const;
	PlayerHoleCards GetStraightDraws();
    int GetPFQual();
	
	void calcHandWinMask();
	ActionMatrix calcActionMatrix();

	int allin();
	int betAgg();
	int betPassive();

	void uberDebug(char* s);

private:
    HoldemTournamentAIBase();

public:
    HoldemTournamentAIBase(MsgQueueBase* q);
    ~HoldemTournamentAIBase();

	/* interface */
	void reset();
	void openProfiler(int game);
	void printPlayerCards(int p, int x, int y, int c, bool bw);
	void pushAction(PlayerAction a);

	void dbRegAct(const char* func, const char* sub, char action=' ');

	int getTableHandedness();
	int getTotalRaises();
	int getAggPlayer(int stage);

	void makePlay();
	void update();
    void EvaluatePlay(int play);

	virtual void print()									= 0;
	virtual ActionRet getPlay()								= 0;

	/* variables */
	PlayerHoleCards getPlayerCards(int player)				{ return playercards[player];							}
	HandRankings getFlopRank()								{ return flopRank;										}
	HandRankings getTurnRank()								{ return turnRank;										}
	HandRankings getRivRank()								{ return rivRank;										}
	const char* getPlayer()									{ return playerName;									} 

	void setWatchingOnly(bool x)							{ watchOnly=x;											}
	bool watchingOnly()										{ return watchOnly||table->getDryRun();						}
	int getAIPlayer();

	string getAIMoniker()									{ return moniker;										}
	string getAIVer()										{ return aiverstr;										}
	int getAIVerMinor()										{ return aiverint;										}

	double percentHandsBeaten(int player, bool print=false);
};

/* class HoldemTournamentAI ***************************************************/
class HoldemTournamentAI : public HoldemTournamentAIBase
{
private:
    HoldemTournamentAI();

public:
    HoldemTournamentAI(MsgQueueBase* q);
    ~HoldemTournamentAI();

	/* interface */
	void print() {}
	ActionRet getPlay();
	int makePlay();
};

/* class HoldemTournamentAIManual *********************************************/
class HoldemTournamentAIManual : public HoldemTournamentAIBase
{
private:
    HoldemTournamentAIManual();

public:
    HoldemTournamentAIManual(MsgQueueBase* q);
    ~HoldemTournamentAIManual();

	/* interface */
	void print() {}

	TableTexture evalTexture(bool debug=false);

	ActionRet getPlay();
	ActionRet pfAlspachMethod(const TableTexture& t, bool debug=true); 
	ActionRet pfLimp(const TableTexture& t, bool debug=true);
	ActionRet pfBlindSteal(const TableTexture& t, bool debug=true);
	ActionRet pfValuePlay(const TableTexture& t, bool debug=true);
	ActionRet headsUp(const TableTexture& t, bool debug=true);
	ActionRet chaseDraws(const TableTexture& t, bool debug=true);
	ActionRet checkSteal(const TableTexture& t, bool debug=true);
	ActionRet continueBet(const TableTexture& t, bool debug=true);
	ActionRet probeBet(const TableTexture& t, bool debug=true);
	ActionRet valuePlay(const TableTexture& t, bool debug=true);

	void dbRegAct(const ActionRet& r, const TableTexture& t);
};

#endif // #ifndef _HoldeMtournamentAI_H_
