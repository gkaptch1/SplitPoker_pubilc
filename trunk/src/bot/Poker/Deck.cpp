
/* #includes ******************************************************************/

#include <time.h>

#include "stdafx.h"
#include "Deck.h"

/* Constructer / Destructer ***************************************************/

Deck::Deck()
{
    Reset();
}

Deck::~Deck()
{
}

/* math operators *************************************************************/

Deck Deck::operator + (const BotCard& c) const				
{ 
	Deck tmp=*this;
	tmp.deck.push_back(c); 
	return tmp; 
}

Deck Deck::operator + (const PokerHand& h) const
{
	Deck tmp=*this;
	for (int i=0; i<h.size(); i++)
	{
		tmp.deck.push_back(h[i]);
	}

	return tmp;
}

/* Deck::Reset() **************************************************************/
void Deck::Reset()
{
	deck.clear();

	for (int i=0; i<52; i++)
	{
		deck+=BotCard(i);
	}
}

/* Deck::Shuffle() ************************************************************/
void Deck::Shuffle()
{
	for (int i=0; i<rand()%3500+5000; i++)
	{
		int pos1=rand()%(int)deck.size(),
			pos2=rand()%(int)deck.size();

		BotCard tmp=deck[pos1];
		deck[pos1]=deck[pos2];
		deck[pos2]=tmp;
	}
}

/* Deck::print() **************************************************************/
void Deck::print()
{
	for (int i=0; i<deck.size(); i++)
	{
		deck[i].Print();
		printf(" ");
	}
}

/* Deck::Deal() **************************************************************/
BotCard Deck::Deal()
{
	BotCard tmp=deck[deck.size()-1];
	deck.pop_back();
	return tmp;
}

/* Deck::Deal() **************************************************************/
BotCard Deck::Deal(BotCard c)
{
	PokerHand deck2;

	for (int i=0; i<deck.size(); i++)
	{
		if (deck[i]!=c) deck2+=deck[i];
	}
	deck=deck2;

	return c;
}

/* Deck::Deal() **************************************************************/
PokerHand Deck::Deal(int n)
{
	PokerHand tmp;

	for (int i=0; i<n; i++)
		tmp+=Deal();

	return tmp;
}