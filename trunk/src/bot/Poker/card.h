#ifndef _Card_H_
#define _Card_H_

/* #includes ******************************************************************/

#include <string.h>
#include "PokerHand.h"

using namespace std;

class PokerHand;
class Card;

extern Card NOCARD;

/* class Card *****************************************************************/

class Card
{
private:
    int val;

public:
    Card();
    Card(int id);
    Card(char suit, char index);
    Card(std::string cardinfo);
    ~Card();

    int GetID() const;
    char GetSuit() const;
    char GetIndex() const;
    int GetRank() const;
	
	const string toString() const;

	operator PokerHand ()								{ return PokerHand(*this);			};
    void operator =  (Card c)							{ val=c.val;						};

    const bool operator != (const Card& c) const		{ return (c.val!=val);	};
    const bool operator == (const Card& c) const		{ return (c.val==val);	};
//    const bool operator != (const Card& c) const		{ return (c.GetRank()!=GetRank());	};
//    const bool operator == (const Card& c) const		{ return (c.GetRank()==GetRank());	};
	const bool eqRank(const Card& c) const				{ return (c.GetRank()==GetRank());  };

	const bool operator >  (const Card& c) const		{ return (GetRank()> c.GetRank());	};
    const bool operator >= (const Card& c) const		{ return (GetRank()>=c.GetRank());	};
    const bool operator <  (const Card& c) const		{ return (GetRank()< c.GetRank());	};
    const bool operator <= (const Card& c) const		{ return (GetRank()<=c.GetRank());	};

	const PokerHand operator + (const Card& c) const	{ return ((PokerHand)(*this)+(PokerHand)c); }

    void Print() const;
	void print() const	{ Print(); }
    void Clear();
    void SetID(int id);
    void SetCard(char suit, char index);
};

#endif // #ifndef _Card_H_
