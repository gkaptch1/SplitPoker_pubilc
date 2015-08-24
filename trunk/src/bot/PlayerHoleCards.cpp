
/* #includes ******************************************************************/

#include "stdafx.h"
#include "PlayerHoleCards.h"

/* Globals ********************************************************************/
//HandRankings inetRanks=PlayerHoleCards::LoadHandRankings("C:\\poker\\startinghands-www.4texasholdem.com.txt");
//HandRankings inetRanksMod=PlayerHoleCards::LoadHandRankings("C:\\poker\\startinghands-mod.txt");
HandRankings inetRanksMod=PlayerHoleCards::LoadHandRankings("C:\\poker\\startinghands-unsuited-www.gameorama.com.txt");
HandRankings defRanks=inetRanksMod;

/* Constructer / Destructer ***************************************************/
PlayerHoleCards::PlayerHoleCards()
{
	SetAll(1);
//???	Normalize();
}

PlayerHoleCards PlayerHoleCards::operator &= (PlayerHoleCards a)
{
	for (int i=0; i<RANKS; i++)
	{
		for (int j=0; j<RANKS; j++)
		{
			cur(0,0);
//			cout << "[" << i << "," << j << "]";
			int tmp=(a.hand[i][j]|PHC_WINNER|PHC_LOSER);
//			cout << "[" << i << "," << j << "]";
			hand[i][j]&=tmp;
//			if (hand[i][j]>a.hand[i][j]) hand[i][j]=0;//a.hand[i][j];
		}
	}

	return *this;
}

PlayerHoleCards PlayerHoleCards::operator &= (int a)
{
	for (int i=0; i<RANKS; i++)
	{
		for (int j=0; j<RANKS; j++)
		{
			hand[i][j]&=a;
//			if (hand[i][j]>a.hand[i][j]) hand[i][j]=0;//a.hand[i][j];
		}
	}

	return *this;
}

PlayerHoleCards& PlayerHoleCards::operator |= (PlayerHoleCards a)
{
	for (int i=0; i<RANKS; i++)
	{
		for (int j=0; j<RANKS; j++)
		{
			hand[i][j]|=(a.hand[i][j]);
//			if (hand[i][j]>a.hand[i][j]) hand[i][j]=0;//a.hand[i][j];
		}
	}

	return *this;
}

PlayerHoleCards& PlayerHoleCards::operator |= (int a)
{
	for (int i=0; i<RANKS; i++)
	{
		for (int j=0; j<RANKS; j++)
		{
			hand[i][j]|=a;
//			if (hand[i][j]>a.hand[i][j]) hand[i][j]=0;//a.hand[i][j];
		}
	}

	return *this;
}

/* PlayerHoleCards::LoadHandRankings() ****************************************/
HandRankings PlayerHoleCards::LoadHandRankings(char* str)
{	
	ifstream in(str);
	HandRankings ret;

	cout << "Loading hand ranks... ";

	while (in) {
		char v1,v2,s=' ';
//		char tmp;
//		double d1,d2,d3;

		in >> v1;
		if (v1=='T') v1='0';
		in >> v2;
		if (v2=='T') v2='0';
//		in >> s; 
//		in >> d1 >> tmp >> d2 >> tmp >> d3 >> tmp;
		
		HandRank_elem e;
		if (s=='s')  e.h=Card('h',v1)+Card('h',v2);
		else e.h=Card('h',v1)+Card('c',v2);
		ret.push_back(e);
	}
/*	for (int i=0; i<ret.size(); i++) {
		cout << i << ": ";
		ret[i].h.print();
		cout << endl;
	}
	getch();
*/
	ret.pop_back();

	cout << "done." << endl;

	return ret;
}

/* PlayerHoleCards::ClrGroups() ***********************************************/
void PlayerHoleCards::ClrGroups()
{
	for (int i=0; i<13; i++)
	{
		for (int j=0; j<13; j++)
		{
			hand[i][j]=0;
		}
	}	
}

/* PlayerHoleCards::SetAll() **************************************************/
void PlayerHoleCards::SetAll(int n)
{
	for (int i=0; i<13; i++)
	{
		for (int j=0; j<13; j++)
		{
			hand[i][j]=n;
		}
	}	
}

/* PlayerHoleCards::SetPercent() **********************************************/
void PlayerHoleCards::SetPercent(double begin, double end, HandRankings& hr)
{
	ClrGroups();
	for (int i=(int)(hr.size()*(1-end)); i<(int)((begin)*hr.size()); i++) {
		setHandValid((hr[i].h)[0],(hr[i].h)[1]);
	}
}

/* PlayerHoleCards::SetGroups() ***********************************************/
void PlayerHoleCards::SetGroups(int grp)
{
	ClrGroups();
	AddGroups(grp);
}

/* PlayerHoleCards::AddGroups() ***********************************************/
void PlayerHoleCards::AddGroups(int grp)
{
	if (grp&PF_GRP_A)		ParseStr("aa kk aks");
	if (grp&PF_GRP_B)		ParseStr("qq ak jj 00");
	if (grp&PF_GRP_C)		ParseStr("aqs 99 aq 88 ajs");
	if (grp&PF_GRP_D)		ParseStr("77 kqs 66 a0s 55 aj");
	if (grp&PF_GRP_E)		ParseStr("kq 44 kjs 33 22 a0 qjs");
	if (grp&PF_GRP_F)		ParseStr("j0 qj kj k0 k9s k8s a9 a8 a7 a6s a5s a4s a3s a2s 9js 80s");
	if (grp&PF_GRP_G)		ParseStr("q0 q9 q8 j9 k9 k8 k7 k6 a6 a5 a4 a3 a2");
	if (grp&PF_SUITCONN)	ParseStr("67s 78s 89s 90s 0js");
}

/* PlayerHoleCards::ParseStr() ************************************************/
void PlayerHoleCards::ParseStr(char* str)
{
	while (str[0]!=0)
	{
		char c1=str[0],
			c2=str[1];

		bool suited=false;
		if (str[2]=='s')
			suited=true;

		setHandValid(c1,c2,suited);

		str+=3;
		if (suited) str++;
	}
}

/* PlayerHoleCards::print() ***************************************************/
void PlayerHoleCards::print(int x, int y, int c, bool bw)
{
	color(c);
	cur(x+2,y);

	for (int i=0; i<13; i++)
	{
		cout << Card(i).GetIndex() << "";
	}
	cur(x,y+1); cout << " --";
	for (int i=0; i<13; i++)
	{
		cout << "-";
	}
	cout << endl;

	for (int i=0; i<13; i++)
	{
		color(c);
		cur(x,y+2+i); cout << Card(i).GetIndex() << "|";
		for (int j=0; j<13; j++)
		{
			if (!bw&&hand[i][j]&PHC_VALID) {
				if (hand[i][j]&PHC_LOSER)		{ color(CUR_RED);	}
				else if (hand[i][j]&PHC_WINNER)	{ color(CUR_GREEN);	}
				else							{ color(c);			}
			}
			else {
				color(c);
			}
				
			cout << (((hand[i][j]&PHC_VALID)!=0)?(1):(0)) << "";
		}
		cout << endl;
	}
	color(c);
}

/* PlayerHoleCards::getNumHands() *********************************************/
int PlayerHoleCards::getNumHands()
{
	int ret=0;
	for (int i=0; i<13; i++) {
		for (int j=0; j<13; j++) {
			if (hand[i][j]&PHC_VALID) ret++;
		}
	}
	return ret;
}

/* PlayerHoleCards::getNumHandsAbs() ******************************************/
int PlayerHoleCards::getNumHandsAbs()
{
	int ret=0;
	for (int i=0; i<13; i++) {
		for (int j=i; j<13; j++) {
			if (hand[i][j]&PHC_VALID) ret++;
		}
	}
	return ret;
}

/* PlayerHoleCards::getNumHandsWonAbs() ***************************************/
int PlayerHoleCards::getNumHandsWonAbs()
{
	int ret=0;
	for (int i=0; i<13; i++) {
		for (int j=i; j<13; j++) {
			if (hand[i][j]&PHC_WINNER&&
				hand[i][j]&PHC_VALID) ret++;
		}
	}
	return ret;
}

/* PlayerHoleCards::GetHand() *************************************************/
int PlayerHoleCards::GetHand(BotCardc1, BotCardc2)
{
	if (hand[c1.GetRank()][c2.GetRank()]&PHC_VALID||
		hand[c2.GetRank()][c1.GetRank()]&PHC_VALID) return 1;
	return 0;
/*	if (c1.GetSuit()!=c2.GetSuit())
	{
		if (c1>c2)
		{
			return hand[c1.GetIndex()][c2.GetIndex()];
		}
		else
		{
			return hand[c2.GetIndex()][c1.GetIndex()];
		}
	}
	else
	{
		if (c1<c2)
		{
			return hand[c1.GetIndex()][c2.GetIndex()];
		}
		else
		{
			return hand[c2.GetIndex()][c1.GetIndex()];
		}
	}
*/}

/* PlayerHoleCards::GetHand() *************************************************/
int PlayerHoleCards::GetHand(char c1, char c2, bool suited)
{
	char suit='s';
	if (suited) suit='h';

	return GetHand(Card('h',c1),Card(suit,c2));
}

/* PlayerHoleCards::setHandValid() ********************************************/
void PlayerHoleCards::setHandValid(BotCardc1, BotCardc2, int mask)
{
	if (c1.GetRank()>=13||c1.GetRank()<0||c2.GetRank()>=13||c2.GetRank()<0) {
		cout << "err";
	}

	if (c1.GetSuit()!=c2.GetSuit())
	{
			hand[c1.GetRank()][c2.GetRank()]|=mask;
			hand[c2.GetRank()][c1.GetRank()]|=mask;
	}
	else
	{
		if (c1<c2)
		{
			hand[c1.GetRank()][c2.GetRank()]|=mask;
		}
		else
		{
			hand[c2.GetRank()][c1.GetRank()]|=mask;
		}
	}
}

/* PlayerHoleCards::setHandValid() ********************************************/
void PlayerHoleCards::setHandValid(char c1, char c2, bool suited, int mask)
{
	char suit='s';
	if (suited) suit='h';

	return setHandValid(Card('h',c1),Card(suit,c2),mask);
}

/* PlayerHoleCards::setHandInvalid() ******************************************/
void PlayerHoleCards::setHandInvalid(BotCardc1, BotCardc2)
{
	if (c1.GetRank()>13||c1.GetRank()<0||c2.GetRank()>13||c2.GetRank()<0) {
		cout << "err";
	}

	if (c1.GetSuit()!=c2.GetSuit())
	{
			hand[c1.GetRank()][c2.GetRank()]|=PHC_INVALID;
			hand[c2.GetRank()][c1.GetRank()]|=PHC_INVALID;
	}
	else
	{
		if (c1<c2)
		{
			hand[c1.GetRank()][c2.GetRank()]|=PHC_INVALID;
		}
		else
		{
			hand[c2.GetRank()][c1.GetRank()]|=PHC_INVALID;
		}
	}
}

/* PlayerHoleCards::setHandInvalid() ******************************************/
void PlayerHoleCards::setHandInvalid(char c1, char c2, bool suited)
{
	char suit='s';
	if (suited) suit='h';

	return setHandInvalid(Card('h',c1),Card(suit,c2));
}