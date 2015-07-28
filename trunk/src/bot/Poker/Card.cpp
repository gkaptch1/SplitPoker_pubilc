
/* #includes ******************************************************************/

#include "stdafx.h"

Card NOCARD(-1);

/* Constructer / Destructer ***************************************************/

Card::Card()
{
    Clear();
}

Card::Card(int id)
{
    SetID(id);
}

Card::Card(char suit, char index)
{
    SetCard(suit,index);
}

Card::Card(std::string cardinfo)
{
    assert(cardinfo.length() == 2);
    //Cards from HN come as <Rank><Suit>
    SetCard(cardinfo[1], cardinfo[0]);
}

Card::~Card()
{
}

/* Card::GetID() **************************************************************/
int Card::GetID() const
{
    return val;
}

/* Card::GetSuit() ************************************************************/
char Card::GetSuit() const
{   
    char ret=' ';
    if (val<0) return ' ';
    switch (val/13)
    {
        case 0: ret='h'; break;
        case 1: ret='d'; break;
        case 2: ret='s'; break;
        case 3: ret='c'; break;
        case 4: ret='u'; break;
    }
    return ret;
}

/* Card::GetIndex() ***********************************************************/
char Card::GetIndex() const
{    
    char ret=' ';
    switch (val%13)
    {
        case 0: ret='2'; break;
        case 1: ret='3'; break;
        case 2: ret='4'; break;
        case 3: ret='5'; break;
        case 4: ret='6'; break;
        case 5: ret='7'; break;
        case 6: ret='8'; break;
        case 7: ret='9'; break;
        case 8: ret='0'; break;
        case 9: ret='J'; break;
        case 10: ret='Q'; break;
        case 11: ret='K'; break;
        case 12: ret='A'; break;
    }
    return ret;
}   

/* Card::GetRank() ************************************************************/
int Card::GetRank() const
{
	if (val%13<0) return 0;
    return val%13;
}   

/* Card::SetID() **************************************************************/
void Card::SetID(int id)
{
    /*if (id>0 && id<52)*/ val=id;
}       

/* Card::Print() **************************************************************/
void Card::Print() const
{
    char s=GetSuit(),v=GetIndex();

    if (val>=0/*&&val<52*/)
    {
        if (v=='0') printf("%c",'1');
        printf("%c",v);

        switch (s)
        {
            case 'h': printf("%c",(char)3); break;
            case 'd': printf("%c",(char)4); break;
            case 'c': printf("%c",(char)5); break;
            case 's': printf("%c",(char)6); break;
            case 'u': printf("u"); break;
        }
    }
}     

/* Card::toString() ***********************************************************/
const string Card::toString() const
{
	char s=GetSuit(),v=GetIndex();

	string ret="";

    if (val>=0/*&&val<52*/)
    {
        if (v=='0') ret+="1";
        ret+=(char)v;
		ret+=(char)s;
    }

	return ret;
}

/* Card::Clear() **************************************************************/
void Card::Clear()
{
    val=-1;
}   

/* Card::SetCard() ************************************************************/
void Card::SetCard(char suit, char index)
{
    int s=0;
    int v=0;
    Clear();

	bool fail=false;

    switch (index)
    {
        case '2': v=0; break;
        case '3': v=1; break;
        case '4': v=2; break;
        case '5': v=3; break;
        case '6': v=4; break;
        case '7': v=5; break;
        case '8': v=6; break;
        case '9': v=7; break;
        case '0': case't': case 'T': v=8; break;
        case 'j':case 'J': v=9; break;
        case 'q':case 'Q': v=10; break;
        case 'k':case 'K': v=11; break;
        case 'a':case 'A': v=12; break;
		default: fail=true;
    }
    switch (suit)
    {
        case 'h': s=0; break;
        case 'd': s=1; break;
        case 's': s=2; break;
        case 'c': s=3; break;
        case 'u': s=4; break;
		default: fail=true;
    }

	if (fail) 
	{
		val=-1;
		return;
	}

    val=13*s+v;
}
