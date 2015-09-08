
/* #includes ******************************************************************/

#include "stdafx.h"
#include "combination.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>


/* Constructer / Destructer ***************************************************/

PokerHand::PokerHand()
{
}

PokerHand::PokerHand(const BotCard& c)
{
	hand.push_back(c);
}

/* assignment operators *******************************************************/

PokerHand& PokerHand::operator = (const PokerHand& h)				
{ 
	hand=h.hand; 
	return *this; 
}

PokerHand& PokerHand::operator = (const BotCard& c)					
{ 
	hand.clear(); 
	hand.push_back(c); 
	return *this; 
}

BotCard  PokerHand::operator [] (int i) const					
{ 
	if (i<0||i>=(signed)hand.size()) {
		char buff[40];
		
//		itoa(i,buff,10);
//		MessageBox(NULL,__FILE__,buff,0);

		return NOCARD;
	}

	return hand[i]; 
}
/*
BotCard& PokerHand::operator [] (int i)					
{ 
	//assert(i<0||i>=hand.size());
	if (i<0||i>=hand.size()) {
		char buff[40];
		itoa(i,buff,10);
		MessageBox(NULL,__FILE__,buff,0);
	}
	//assert(i<0||i>=hand.size());
	return (BotCard&)hand[i]; 
}

/* math operators *************************************************************/

PokerHand PokerHand::operator + (const BotCard& c) const				
{ 
	PokerHand tmp=*this;
	tmp.hand.push_back(c); 
	return tmp; 
}

PokerHand PokerHand::operator + (const PokerHand& h) const
{
	PokerHand tmp=*this;
	for (unsigned int i=0; i<h.hand.size(); i++)
	{
		tmp.hand.push_back(h.hand[i]);
	}

	return tmp;
}

/* PokerHand::delCard() *******************************************************/
void PokerHand::delCard(int n)
{
	PokerHand tmp;

	for (unsigned int i=0; i<hand.size(); i++)
	{
		if (i!=n) tmp+=hand[i];
	}

	hand=tmp.hand;
}

/* PokerHand::eq() ************************************************************/
const bool PokerHand::eq(const PokerHand& h) const
{
	if (h.size()!=size()) return false;

	PokerHand tmp1=h,tmp2=*this;

	for (int i=0; i<h.size(); i++)
		if (tmp1.HighCard().GetRank()!=tmp2.HighCard().GetRank()) return false;

	return true;
}

/* PokerHand::lessThan() ******************************************************/
bool PokerHand::lessThan(PokerHand h1, PokerHand h2)
{
	BotCard c1=NOCARD,c1b=NOCARD,
		 c2=NOCARD,c2b=NOCARD;
	int cards=5;

	// Straight Flush
	PokerHand h1f=h1.Flush(),
			  h2f=h2.Flush();

	if (!h1f.isEmpty()) c1=h1f.Straight();
	if (!h2f.isEmpty()) c2=h2f.Straight();
	if (c1!=NOCARD&&c2==NOCARD) { return false;  }
	if (c1==NOCARD&&c2!=NOCARD) { return true; }
	if (c1<c2) { return true;  }
	if (c1>c2) { return false; }
	if (c1!=NOCARD&&c1.eqRank(c2)) { return false; }

	// Quads
	c1=h1.Quad();
	c2=h2.Quad();
	if (c1!=NOCARD&&c2==NOCARD) { return false;  }
	if (c1==NOCARD&&c2!=NOCARD) { return true; }
	if (c1<c2) { return true;  }
	if (c1>c2) { return false; }
	if (c1!=NOCARD&&c1.eqRank(c2))
	{
		c1b=h1.HighCard();
		c2b=h2.HighCard();
		if (c1b< c2b) { return true;  }
		if (c1b>=c2b) { return false; }
	}

	// Fullhouse
	c1=h1.Trip(); if (c1!=NOCARD) c1b=h1.Pair();
	c2=h2.Trip(); if (c2!=NOCARD) c2b=h2.Pair();
	BotCard c1trip=c1,c2trip=c2;
	if ((c1==NOCARD||c1b==NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2==NOCARD||c2b==NOCARD)) { return false; }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1<c2)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1>c2)) { return false;  }
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b!=NOCARD&&c1b.eqRank(c2b))) return false;
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b<c2b)) return true;
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b>c2b)) return false;

	// Flush
	if (!h1f.isEmpty()&&h2f.isEmpty()) return false;
	if (h1f.isEmpty()&&!h2f.isEmpty()) return true;
	if (!h1f.isEmpty()&&!h2f.isEmpty()) { h1=h1f; h2=h2f; }
	// Straight
	c1=h1.Straight();
	c2=h2.Straight();
	if (c1!=NOCARD&&c2==NOCARD) { return false;  }
	if (c1==NOCARD&&c2!=NOCARD) { return true; }
	if (c1<c2) { return true;  }
	if (c1>c2) { return false; }
	if (c1!=NOCARD&&c1.eqRank(c2)) { return false; }
	
	// Trips
	c1=c1trip;
	c2=c2trip;
	if (c1!=NOCARD&&c2==NOCARD) { return false; }
	if (c1==NOCARD&&c2!=NOCARD) { return true;  }
	if (c1<c2) { return true;  }
	if (c1>c2) { return false; }
	if (c1!=NOCARD&&c1.eqRank(c2)) { cards=2; goto highcard; }

	// Two Pair
	c1=h1.Pair(); c1b=h1.Pair();
	c2=h2.Pair(); c2b=h2.Pair();
	if ((c1==NOCARD||c1b==NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2==NOCARD||c2b==NOCARD)) { return false; }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1<c2)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1>c2)) { return false;  }
	if (c1!=NOCARD&&c1.eqRank(c2)&&(c1b<c2b)) return true;
	if (c1!=NOCARD&&c1.eqRank(c2)&&(c1b>c2b)) return false;
	if (c1!=NOCARD&&c2!=NOCARD&&c1.eqRank(c2)) { cards=1; goto highcard; }
/*
	cout << "1: "; c1.print(); c1b.print(); cout << endl;
	cout << "2: "; c2.print(); c2b.print(); cout << endl;
	cout << endl;
	cout << "noflush" << endl;
*/
	// Pair
	if (c1b==NOCARD&&c2b==NOCARD)
	{
		if (c1!=NOCARD&&c2==NOCARD) { return false;  }
		if (c1==NOCARD&&c2!=NOCARD) { return true; }
		if (c1<c2) { return true;  printf("test"); }
		if (c1>c2) { return false; printf("test"); }
		if (c1!=NOCARD&&c1.eqRank(c2)) { cards=3; }
	}

	// High Card
highcard:
	while ((!h1.isEmpty()&&!h2.isEmpty()))//&&(--cards>=0))
	{
		c1=h1.HighCard();
		c2=h2.HighCard();

		if (c1<c2) return true;
		if (c1>c2) return false;
	}

	return false;
}

/* PokerHand::lessThanFast() **************************************************/
bool PokerHand::lessThanFast(PokerHand h1, PokerHand h2)
{
	BotCard c1=NOCARD,c1b=NOCARD,
		 c2=NOCARD,c2b=NOCARD;
	int cards=5;

	// Fullhouse
	c1=h1.Trip(); if (c1!=NOCARD) c1b=h1.Pair();
	c2=h2.Trip(); if (c2!=NOCARD) c2b=h2.Pair();
	BotCard c1trip=c1,c2trip=c2;
	if ((c1==NOCARD||c1b==NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2==NOCARD||c2b==NOCARD)) { return false; }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1<c2)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1>c2)) { return false;  }
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b!=NOCARD&&c1b.eqRank(c2b))) return false;
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b<c2b)) return true;
	if ((c1!=NOCARD&&c1.eqRank(c2))&&(c1b>c2b)) return false;
	
	// Trips
	c1=c1trip;
	c2=c2trip;
	if (c1!=NOCARD&&c2==NOCARD) { return false; }
	if (c1==NOCARD&&c2!=NOCARD) { return true;  }
	if (c1<c2) { return true;  }
	if (c1>c2) { return false; }
	if (c1!=NOCARD&&c1.eqRank(c2)) { cards=2; goto highcard; }

	// Two Pair
	c1=h1.Pair(); c1b=h1.Pair();
	c2=h2.Pair(); c2b=h2.Pair();
	if ((c1==NOCARD||c1b==NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2==NOCARD||c2b==NOCARD)) { return false; }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1<c2)) { return true;  }
	if ((c1!=NOCARD&&c1b!=NOCARD)&&(c2!=NOCARD&&c2b!=NOCARD)&&(c1>c2)) { return false;  }
	if (c1!=NOCARD&&c1.eqRank(c2)&&(c1b<c2b)) return true;
	if (c1!=NOCARD&&c1.eqRank(c2)&&(c1b>c2b)) return false;
	if (c1!=NOCARD&&c2!=NOCARD&&c1.eqRank(c2)) { cards=1; goto highcard; }
/*
	cout << "1: "; c1.print(); c1b.print(); cout << endl;
	cout << "2: "; c2.print(); c2b.print(); cout << endl;
	cout << endl;
	cout << "noflush" << endl;
*/
	// Pair
	if (c1b==NOCARD&&c2b==NOCARD)
	{
		if (c1!=NOCARD&&c2==NOCARD) { return false;  }
		if (c1==NOCARD&&c2!=NOCARD) { return true; }
		if (c1<c2) { return true;  printf("test"); }
		if (c1>c2) { return false; printf("test"); }
		if (c1!=NOCARD&&c1.eqRank(c2)) { cards=3; }
	}

	// High Card
highcard:
	while ((!h1.isEmpty()&&!h2.isEmpty()))//&&(--cards>=0))
	{
		c1=h1.HighCard();
		c2=h2.HighCard();

		if (c1<c2) return true;
		if (c1>c2) return false;
	}

	return false;
}

/* PokerHand::GetCombinations() ***********************************************/
vector<PokerHand> PokerHand::GetCombinations(int size)
{
	vector<PokerHand> ret;

	vector<BotCard> tmp=hand;

	PokerHand h;
	for (int i=0; i<size; i++)
		h+=hand[i];
	do
	{
		ret.push_back(h);
	}
	while(next_combination(tmp.begin(),tmp.end(),h.hand.begin(),h.hand.end()));

	return ret;
}

/* PokerHand::WinPercent() ****************************************************/
double PokerHand::WinPercent(PokerHand& board, int cardsLeft)
{
	PokerHand tmp=*this+board;
	int cardProb[13];
	memset(cardProb,4,sizeof(cardProb));

	for (int i=0; i<tmp.size(); i++) {
		if (tmp[i]==NOCARD) continue;
		cardProb[tmp[i].GetRank()]--;
	}

	int totalcards=0;
	for (int i=0; i<13; i++) {
		if (cardProb[i]>0)
			totalcards+=cardProb[i];
		if (cardsLeft==1) {

		}
	}

	return 0;
}


/* PokerHand::HighCard() ******************************************************/
BotCard PokerHand::HighCard()
{
	BotCard c(0);
	int pos=0;

	for (unsigned int i=0; i<hand.size(); i++)
	{
		if (hand[i]>c) 
		{
			c=hand[i];
			pos=i;
		}
	}

	delCard(pos);

	return c;
}

/* PokerHand::Pair() **********************************************************/
BotCard PokerHand::Pair()
{
	BotCard c=NOCARD;
	int pos1=0,pos2=0;

	for (unsigned int i=0; i<hand.size(); i++)
	{
		for (unsigned int j=i+1; j<hand.size(); j++)
		{
			if (hand[i].GetRank()==hand[j].GetRank()&&hand[i]!=NOCARD&&hand[i]>c)
			{
				c=hand[i];
				pos1=i;
				pos2=j;
			}
		}
	}

	if (c!=NOCARD)
	{
		delCard(pos2);
		delCard(pos1);
	}

	return c;
}

/* PokerHand::Trip() **********************************************************/
BotCard PokerHand::Trip()
{
	BotCard c=NOCARD;
	int pos1=0,pos2=0,pos3=0;

	for (unsigned int i=0; i<hand.size(); i++)
	{
		for (unsigned int j=i+1; j<hand.size(); j++)
		{
			if (hand[i].GetRank()==hand[j].GetRank()&&hand[i]!=NOCARD&&hand[j]!=NOCARD)
			{
				for (unsigned int k=j+1; k<hand.size(); k++)
				{
					if (hand[i].GetRank()==hand[k].GetRank()&&hand[k]!=NOCARD)
					{
						c=hand[i];
						pos1=i;
						pos2=j;
						pos3=k;
					}
				}
			}
		}
	}

	if (c!=NOCARD)
	{
		delCard(pos3);
		delCard(pos2);
		delCard(pos1);
	}

	return c;
}

/* PokerHand::Quad() **********************************************************/
BotCard PokerHand::Quad()
{
	BotCard c=NOCARD;
	int pos1=0,pos2=0,pos3=0,pos4=0;

	for (unsigned int i=0; i<hand.size(); i++)
	{
		for (unsigned int j=i+1; j<hand.size(); j++)
		{
			if (hand[i].GetRank()==hand[j].GetRank())
			{
				for (unsigned int k=j+1; k<hand.size(); k++)
				{
					if (hand[i].GetRank()==hand[k].GetRank())
					{
						for (unsigned int l=k+1; l<hand.size(); l++)
						{
							if (hand[i].GetRank()==hand[l].GetRank())
							{
								c=hand[i];
								pos1=i;
								pos2=j;
								pos3=k;
								pos4=l;
							}
						}
					}
				}
			}
		}
	}

	if (c!=NOCARD)
	{
		delCard(pos4);
		delCard(pos3);
		delCard(pos2);
		delCard(pos1);
	}

	return c;
}

/* PokerHand::Flush() *********************************************************/
PokerHand PokerHand::Flush()
{
	PokerHand h;

	int suit[4]={0,0,0,0};

	for (unsigned int i=0; i<hand.size(); i++)
	{
		if (hand[i].GetSuit()=='h') suit[0]++;
		if (hand[i].GetSuit()=='d') suit[1]++;
		if (hand[i].GetSuit()=='c') suit[2]++;
		if (hand[i].GetSuit()=='s') suit[3]++;
	}

	char flushsuit=' ';

	if (suit[0]>=5) flushsuit='h';
	if (suit[1]>=5) flushsuit='d';
	if (suit[2]>=5) flushsuit='c';
	if (suit[3]>=5) flushsuit='s';

	for (unsigned int i=0; i<hand.size(); i++)
	{
		if (hand[i].GetSuit()==flushsuit) h+=hand[i];
	}

	if (!h.isEmpty())
	{
		h[0]=BotCard('h',h[0].GetIndex());
		h[1]=BotCard('d',h[1].GetIndex());
	}

	return h;
}

/* PokerHand::isFlush() *******************************************************/
bool PokerHand::isFlush(int n)
{
	PokerHand h;
	int suit[4]={0,0,0,0};

	for (unsigned int i=0; i<hand.size(); i++)
	{
		if (hand[i].GetSuit()=='h') suit[0]++;
		if (hand[i].GetSuit()=='d') suit[1]++;
		if (hand[i].GetSuit()=='c') suit[2]++;
		if (hand[i].GetSuit()=='s') suit[3]++;
	}

	for (int i=0; i<4; i++) {
		if (suit[i]>=n) return true;
	}

	return false;
}

/* PokerHand::Straight() ******************************************************/
BotCard PokerHand::Straight(int n)
{
	PokerHand h=*this;
	BotCard c=h.HighCard();
	int count=1,
		size=(signed)h.hand.size();

	for (int i=0; i<size&&count<n; i++)
	{
		BotCard tmp=h.HighCard();
		if (c.GetRank()-tmp.GetRank()==1)
		{
			count++;
		}
		else if (c.GetRank()-tmp.GetRank()==0)
		{
		}
		else
		{
			count=1;
		}
		c=tmp;
	}

	if (count>=n) 
	{
		return c;
	}

	return NOCARD;
}

/* PokerHand::print() *********************************************************/
void PokerHand::print() const
{
	for (unsigned int i=0; i<hand.size(); i++)
	{
		hand[i].Print();
		printf(" ");
	}
}

/* PokerHand::toString() *********************************************************/
const string PokerHand::toString() const
{
	string ret=" ";
	for (unsigned int i=0; i<hand.size(); i++)
	{
		ret+=hand[i].toString();
		ret+=" ";
	}

	return ret;
}