#ifndef _BotCard_H_
#define _BotCard_H_

/* #includes ******************************************************************/

#include <string.h>
#include "PokerHand.h"

using namespace std;

class PokerHand;
class BotCard;

extern BotCard NOCARD;

/* class Card *****************************************************************/

class BotCard
{
private:
    int val;

public:
    BotCard();
    BotCard(int id);
    BotCard(char suit, char index);
    BotCard(std::string cardinfo);
    ~BotCard();

    int GetID() const;
    char GetSuit() const;
    char GetIndex() const;
    int GetRank() const;
	
	const string toString() const;

	operator PokerHand ()								{ return PokerHand(*this);			};
    void operator =  (BotCard c)							{ val=c.val;						};

    const bool operator != (const BotCard& c) const		{ return (c.val!=val);	};
    const bool operator == (const BotCard& c) const		{ return (c.val==val);	};
//    const bool operator != (const Card& c) const		{ return (c.GetRank()!=GetRank());	};
//    const bool operator == (const Card& c) const		{ return (c.GetRank()==GetRank());	};
	const bool eqRank(const BotCard& c) const				{ return (c.GetRank()==GetRank());  };

	const bool operator >  (const BotCard& c) const		{ return (GetRank()> c.GetRank());	};
    const bool operator >= (const BotCard& c) const		{ return (GetRank()>=c.GetRank());	};
    const bool operator <  (const BotCard& c) const		{ return (GetRank()< c.GetRank());	};
    const bool operator <= (const BotCard& c) const		{ return (GetRank()<=c.GetRank());	};

	const PokerHand operator + (const BotCard& c) const	{ return ((PokerHand)(*this)+(PokerHand)c); }

    void Print() const;
	void print() const	{ Print(); }
    void Clear();
    void SetID(int id);
    void SetCard(char suit, char index);
};

#endif // #ifndef _BotCard_H_
