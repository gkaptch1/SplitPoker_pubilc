#ifndef _PlayerHoleCards_H_
#define _PlayerHoleCards_H_

/* #includes ******************************************************************/

#include "Poker/poker.h"
#include <vector>
//#include <windows.h>

using namespace std;

/* Globals ********************************************************************/
struct HandRank_elem
{
	PokerHand h;
	int num;
};

class HandRankings : public vector<HandRank_elem>
{
public:
	int findHand(const PokerHand& h) const
	{
		for (unsigned int i=0; i<size(); i++) {
			if ((*this)[i].h.eq(h)) {
				return i;
			}

		}

		return (int)size();
	}
};

extern HandRankings inetRanks;
extern HandRankings defRanks;

/* defines ********************************************************************/
#define PHC_INVALID								0x00000000
#define PHC_VALID								0x00000001
#define PHC_WINNER								0x00000002
#define PHC_LOSER								0x00000004

/* class PlayerHoleCards ******************************************************/
class PlayerHoleCards
{
private:
	int hand[RANKS][RANKS];

	HandRankings inetRanks;

public:
	PlayerHoleCards();

	PlayerHoleCards operator &= (PlayerHoleCards);
	PlayerHoleCards operator &= (int);
	PlayerHoleCards& operator |= (PlayerHoleCards);
	PlayerHoleCards& operator |= (int);

	void print(int x, int y, int c, bool bw);

	static HandRankings LoadHandRankings(char* str);

	void ClrGroups();
	void SetAll(int n);
	void SetPercent(double begin, double end, HandRankings& hr=defRanks);
	void SetGroups(int grp);
	void AddGroups(int grp);
	void SubGroups(int grp);
	void ParseStr(char* str);

	int getNumHands();
	int getNumHandsAbs();
	int getNumHandsWonAbs();
	int GetHand(BotCard c1, BotCard c2);
	int GetHand(char c1, char c2, bool suited);
	void setHandValid(BotCard c1, BotCard c2, int mask=PHC_VALID);
	void setHandValid(char c1, char c2, bool suited, int mask=PHC_VALID);
	void setHandInvalid(BotCard c1, BotCard c2);
	void setHandInvalid(char c1, char c2, bool suited);

};

#endif