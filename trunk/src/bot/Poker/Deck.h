#ifndef _Deck_H_
#define _Deck_H_

/* #includes ******************************************************************/

#include <vector>
#include "card.h"
#include "PokerHand.h"

/* class BotCard  *****************************************************************/

class Deck
{
private:
	PokerHand deck;

public:
    Deck();
    ~Deck();

	void Reset();
	void Shuffle();

	void print();

	BotCard Deal();
	BotCard Deal(BotCard  c);
	PokerHand Deal(int size);

	vector<PokerHand> GetCombinations(int size)		{ return deck.GetCombinations(size); }
	
	Deck& operator += (const PokerHand& h)				{ return (*this = *this + h); }
	Deck& operator += (const BotCard& c)					{ return (*this = *this + c); }
	Deck  operator +  (const PokerHand& h) const;
	Deck  operator +  (const BotCard& c) const;
};

#endif // #ifndef _Deck_H_
