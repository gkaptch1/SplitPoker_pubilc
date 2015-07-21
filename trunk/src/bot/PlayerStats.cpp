
/* #includes ******************************************************************/

#include "stdafx.h"
#include "PlayerStats.h"

/* MACROS *********************************************************************/
#define VERBOS(x,y,z) /*printf(x,y,z);*/

/* Constructer / Destructer ***************************************************/

PlayerStats::PlayerStats()
{
}

PlayerStats::PlayerStats(const char* name)
{         
    memset(this,0,sizeof(PlayerStats));
    strcpy(player,name);
    alive=true;
    active=true;
    totalgames=1;
    totalhands=1;
    stack=1000;
	place_eliminated=1;
    flopped=0;
}

PlayerStats::~PlayerStats()
{
}         

/* PlayerStats::Combine() *****************************************************/

void PlayerStats::Combine(PlayerStats*p)
{
    vcheck+=p->vcheck;
    vcall+=p->vcall;
    vbet+=p->vbet;
    vraise+=p->vraise;
    vfold+=p->vfold;
    flop+=p->flop;
    turn+=p->turn;
    river+=p->river;
    showdown+=p->showdown;
    totalgames++;
    totalhands+=p->totalhands;

    for (int i=0; i<10;i++)
    {
        place[i]+=p->place[i];
    }  

    potswon+=p->potswon;
    checkraise+=p->checkraise;
    checkcall+=p->checkcall;
    checkfold+=p->checkfold;
}    

/* PlayerStats::LoadPlayerFile() **********************************************/

int PlayerStats::LoadPlayerFile(char *p)
{
    char File[1000];
    strcpy(File,"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\playerdb\\");
    strcat(File,p);

    FILE* filedesc = fopen(File,"r");

    if (filedesc==NULL)
    {
        printf("Player %s not in database.\n",p);
        return 0;
    }

    fclose(filedesc);

    return 0;
}  

/* PlayerStats::LoadPlayerHile() **********************************************/

int PlayerStats::WriteFile()
{
//    PlayerStats tmp;
//    tmp.LoadPlayerFile(player);

    return 0;
}

/* PlayerStats::BuffStatsDump() ***********************************************/

char* PlayerStats::BuffStatsDump()
{
    char *buff=(char*)malloc(4000);
    memset(buff,0,4000);
    char num[100];
    strcpy(buff,player);

    // raw

    strcat(buff,"\t");
    itoa(vcheck,num,10);
    strcat(buff,num);
    strcat(buff,"\t");  
    itoa(vcall,num,10);
    strcat(buff,num);
    strcat(buff,"\t"); 
    itoa(vbet,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(vraise,num,10);
    strcat(buff,num);
    strcat(buff,"\t"); 
    itoa(vfold,num,10);
    strcat(buff,num);
    strcat(buff,"\t"); 
    itoa(totalhands,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(totalgames,num,10);
    strcat(buff,num);
    strcat(buff,"\t"); 
    itoa(flop,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(turn,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(river,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(showdown,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    for (int i=0; i<10; i++)
    {
        itoa(place[i],num,10);
        strcat(buff,num);
        strcat(buff,"\t");
    } 
    itoa(potswon,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(checkraise,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(checkcall,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    itoa(checkfold,num,10);
    strcat(buff,num);
    strcat(buff,"\t");

    // %

    char*tmp;
    int*a =new int,*b=new int;

    itoa(totalhands/totalgames,num,10);
    strcat(buff,num);
    strcat(buff,"\t");
    tmp=fcvt(flop/(float)totalhands,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(turn/(float)totalhands,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(river/(float)totalhands,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(showdown/(float)totalhands,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(potswon/(float)totalhands,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(checkraise/(float)vcheck,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(checkcall/(float)vcheck,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");
    tmp=fcvt(checkfold/(float)vcheck,2,a,b);
    strcat(buff,tmp);
    strcat(buff,"\t");

    strcat(buff,"\n");

    return buff;
}  

/* PlayerStats::BuffStatsDumpTitle() ******************************************/

char* PlayerStats::BuffStatsDumpTitle()
{
    return "player\tcheck\tcall\tbet\traise\tfold\ttotalhands\ttotalgames\tflop\tturn\triver\tshowdown\t1st\t2nd\t3rd\t4th\t5th\t6th\t7th\t8th\t9th\t10th\tpotswon\tcheckraise\tcheckcall\tcheckfold\thands/game\t%flop\t%turn\t%river\t%showdown\t%potswon\t%craise\t%ccall\t%cfold\n";
}      

/* PlayerStats::ScreenStatsDump() *********************************************/

void PlayerStats::ScreenStatsDump()
{
//    printf("%14s:\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",player,flop,turn,river,showdown,vfold,totalhands,totalgames);
    printf("%14s:\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",player,place[0],place[1],place[2],place[3],place[4]+place[5]+place[6]+place[7]+place[8]+place[9],totalhands,totalgames);
}

/* PlayerStats::ScreenStatsDumpTitle() ****************************************/

void PlayerStats::ScreenStatsDumpTitle()
{
 //   printf("%14s:\tflop\tturn\triver\tshowdwn\tfolds\thands\n","player");
    printf("%14s:\t1\t2\t3\t4\t5-10\thands\tgames\n","place");
}

/* PlayerStats::GetName() *****************************************************/

char* PlayerStats::GetName()
{
    char *ret=(char*)malloc(40);
    strcpy(ret,player);
    return player;
}  

/* PlayerStats::GetPlace() ****************************************************/

int PlayerStats::GetPlace()
{ 
/*	for (int i=0; i<10; i++) 
	{
		if (place[i]>1) return i; 
	}   
*/
	return place_eliminated;
}		

/* PlayerStats::GetCheckAct() *************************************************/
int PlayerStats::GetCheckAct(int act)
{
	if (act==FOLD)			return checkfold;
	else if (act==CALL)		return checkcall;
	else					return checkraise;
}		

/* PlayerStats::NewRound() ****************************************************/

void PlayerStats::NewRound()
{
    totalhands++;
//    printf("%s totalhands: %i",player,totalhands);
	if (alive) {
		active=true;
	}
	else {
		active=false;
	}

	isallin=false;

     havechecked=false;
     flopped=0;

/*    for (int i=0;i<strlen(player);i++)
    {
    char a=player[i];
    if ((a>='0'&&a<='9')||(a>='a'&&a<='z')||(a>='A'&&a<='Z')||a==' '||a==','||a=='.'||a=='#'||a=='$');
    else
    {
        printf("ERROR");
    }
    }
*/}

/* PlayerStats::call() ********************************************************/

void PlayerStats::call()
{                
    vcall++;
    VERBOS("%s: call %i.\n",player,vcall);
                               
    tight      -= .4;
    aggressive -= .4;

    if (havechecked)
    {                        
        checkcall++;
        VERBOS("%s: check-call %i.\n",player,checkcall);
    }
}

/* PlayerStats::bet() *********************************************************/

void PlayerStats::bet()
{                                         
    vbet++;
    VERBOS("%s: bet %i.\n",player,vbet); 

    tight      -= .4;
    aggressive += .2;
}

/* PlayerStats::raise() *******************************************************/

void PlayerStats::raise()
{                                         
    vraise++;
    VERBOS("%s: raise %i.\n",player,vraise);

    tight      -= .4;
    aggressive += .4;

    if (havechecked)
    {
        aggressive += .6;
        checkraise++;
        VERBOS("%s: check-raise %i.\n",player,checkraise);
    }
}

/* PlayerStats::allin() *******************************************************/

void PlayerStats::allin()
{                                         
    vraise++;
    active=false;
	isallin=true;
    VERBOS("%s: raise %i.\n",player,vraise);

    if (havechecked)
    {                        
        checkraise++;
        VERBOS("%s: check-raise %i.\n",player,checkraise);
    }
}

/* PlayerStats::check() *******************************************************/

void PlayerStats::check()
{      
    tight      += .01;
    aggressive -= .01;
                                           
    vcheck++;
    VERBOS("%s: check %i.\n",player,vcheck);
    havechecked=true;
}

/* PlayerStats::fold() ********************************************************/

void PlayerStats::fold()
{                                            
    vfold++;
    VERBOS("%s: fold %i.\n",player,vfold);
    active=false; 

    tight      += .4;
    aggressive -= .01;

    if (havechecked)
    {            
        aggressive -= .1;
        checkfold++;
        VERBOS("%s: check-fold %i.\n",player,checkfold);
    }
}

/* PlayerStats::Flop() ********************************************************/

void PlayerStats::Flop()
{
    flopped++;

    if (flopped>1)
    {
        printf("ERROR: DOUBLE FLOP %s, %i, %i\n",player,flop,totalhands);
    }
    else
    {          
        flop++;
        havechecked=false;
        VERBOS("%s: flop %i.\n",player,flop);
    }
}

/* PlayerStats::Turn() ********************************************************/

void PlayerStats::Turn()
{
    turn++; 
    havechecked=false;
    VERBOS("%s: turn %i.\n",player,turn);
}

/* PlayerStats::River() ********************************************************/

void PlayerStats::River()
{
    river++;     
    havechecked=false;   
    VERBOS("%s: river %i.\n",player,river);
}

/* PlayerStats::win() *********************************************************/

void PlayerStats::win()
{
    potswon++;
}

/* PlayerStats::isAlive() *****************************************************/

bool PlayerStats::isAlive()
{
    return alive;
}

/* PlayerStats::isActive() ****************************************************/

bool PlayerStats::isActive()
{
    if (alive) return active;
    return false;
}
/* PlayerStats::isAllin() ****************************************************/

bool PlayerStats::isAllin()
{
	return isallin;
}

/* PlayerStats::eliminated() **************************************************/

void PlayerStats::eliminated(int p)
{
	place_eliminated=p;
    if (p<=10&&p>0)   place[p-1]++;
    else printf("\nERROR: elimination\n\n");
    alive=false;
}
