#ifndef _Hand_h_
#define _Hand_h_

/* #includes ******************************************************************/

#include <vector>
#include "card.h"

class BotCard;

using namespace std;

/* class PokerHand ************************************************************/

class PokerHand
{
private:
	vector<BotCard> hand;

	void delCard(int n);

public:
	PokerHand();
	PokerHand(const BotCard& c);

	void print() const;
	const string toString() const;

	void clear()					{ hand.clear(); }
	void pop_back()					{ hand.pop_back(); }
	void push_back(const BotCard& c)	{ hand.push_back(c); }

	vector<PokerHand> GetCombinations(int combSize);

	double WinPercent(PokerHand& board, int cardsLeft);

	BotCard HighCard();
	BotCard Pair();
	BotCard Trip();
	BotCard Quad();
	BotCard Straight(int n=5);
	PokerHand Flush();
	bool isFlush(int n=5);
	
	static bool lessThan(PokerHand h1, PokerHand h2);
	static bool lessThanFast(PokerHand h1, PokerHand h2);

	BotCard  operator [] (const int i) const;
//	Card& operator [] (const int i);

	const bool isEmpty() const								{ return (hand.size()==0); }
	const int  size() const									{ return (int)hand.size(); }

	const bool eq(const PokerHand& h) const;

	const bool operator != (const PokerHand& h) const		{ return !(*this==h); }
	const bool operator == (const PokerHand& h) const		{ return (!lessThan(*this,h))&&(!lessThan(h,*this)); }
	const bool operator <  (const PokerHand& h) const		{ return lessThan(*this, h); }
	const bool operator >  (const PokerHand& h) const		{ return lessThan(h, *this); }
	const bool operator <= (const PokerHand& h) const		{ return !(*this>h); }
	const bool operator >= (const PokerHand& h) const		{ return !(*this<h); }

	PokerHand& operator += (const PokerHand& h)				{ return (*this = *this + h); }
	PokerHand& operator += (const BotCard& c)					{ return (*this = *this + c); }

	PokerHand& operator =  (const PokerHand& h);
	PokerHand& operator =  (const BotCard& c);
	PokerHand  operator +  (const PokerHand& h) const;
	PokerHand  operator +  (const BotCard& c) const;
};

#endif