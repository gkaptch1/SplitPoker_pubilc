
/* #includes ******************************************************************/

#include "stdafx.h"
#include "TournamentData.h"

MYSQL* mysql=NULL;
int numMysqls=0;

/* strcat() *******************************************************************/

void strcat(char* s, int i)
{
	char buff[40];
	memset(buff,0,sizeof(buff));
	itoa(i,buff,10);
	strcat(s,buff);
}

void strcat(char* s, double i)
{
	char buff[40];
	memset(buff,0,sizeof(buff));
	sprintf(buff,"%d",i);
	strcat(s,buff);
}

void strcat(char* s, char i)
{
	char buff[2]={i,0};
	strcat(s,buff);
}

void strcat(char* s, Card c)
{
	char buff[3]={c.GetIndex(),c.GetSuit(),0};
	strcat(s,buff);
}

/* Constructer / Destructer ***************************************************/
TournamentData::TournamentData(MsgQueue* t)
{
	memset(this,0,sizeof(this));
	action_id=0;
	table=t;
	allowWrite=false;
	memset(errors,0,sizeof(errors));

	if (numMysqls==0) {
		numMysqls++;
		cout << "Connecting to mysql database... ";
		mysql=mysql_init(NULL); 
/*
		if (mysql_real_connect(	mysql,conf.getStrValue("mysql_ip").c_str(),conf.getStrValue("mysql_user").c_str(),conf.getStrValue("mysql_pass").c_str(),conf.getStrValue("mysql_db").c_str(),(int)conf.getRealValue("mysql_port"),NULL,0)==NULL) {
			//MessageBoxA(0,mysql_error(mysql),"ERROR",0);
			cout << "failed." << endl;
		}
		else {
			cout << "done." << endl;
		}
*/
		cout << "aborted." << endl;
	}
	
}

TournamentData::~TournamentData()
{
/*	removed because mysql would unexpectedly open/close connections
	numMysqls--;
	if (numMysqls==0) {
		mysql_close(mysql);
	}*/
}         

/* TournamentData::getBuyinFromName() *****************************************/
void TournamentData::eraseDB()
{
	string sql;
	
	sql="TRUNCATE TABLE `sng_ai`;";
	mysql_query(mysql,sql.c_str());
	
	sql="TRUNCATE TABLE `sng_aigames`;";
	mysql_query(mysql,sql.c_str());
	
	sql="TRUNCATE TABLE `sng_aistats`;";
	mysql_query(mysql,sql.c_str());
	
	sql="TRUNCATE TABLE `sng_games`;";
	mysql_query(mysql,sql.c_str());
	
	sql="TRUNCATE TABLE `sng_hands`;";
	mysql_query(mysql,sql.c_str());
}

/* TournamentData::getBuyinFromName() *****************************************/
const string TournamentData::getBuyinFromName(const string& name)
{
	string buyin="unknown";

	if (name=="Imperial Beach")				{ buyin="20"; }
	if (name=="Tourmaline")					{ buyin="20"; }

	if (name=="Puerto Escondido")			{ buyin="10"; }
	if (name=="Big Carona")					{ buyin="10"; }
	if (name=="Cabo San Lucas")				{ buyin="10"; }
	if (name=="Costa Azul")					{ buyin="10"; }
	if (name=="Galapagos")					{ buyin="10"; }
	if (name=="Laguna Beach")				{ buyin="10"; }
	if (name=="Little Corona")				{ buyin="10"; }
	if (name=="Nusa Tenggara")				{ buyin="10"; }
	if (name=="Playa Kandahar")				{ buyin="10"; }
	if (name=="PokerBreath")				{ buyin="10"; }
	if (name=="True North")					{ buyin="10"; }
	if (name=="Tuamotu Atolls")				{ buyin="10"; }

	if (name=="Turbo Bonkers")				{ buyin="t10"; }
	if (name=="Turbo Bonkers")				{ buyin="t10"; }
	if (name=="Turbo Jam")					{ buyin="t10"; }
	if (name=="Turbo Smooch")				{ buyin="t10"; }

	if (name=="Long Ranger")				{ buyin="5"; }
	if (name=="Sea Ghost")					{ buyin="5"; }
	if (name=="Gleeming the Cube")			{ buyin="5"; }
	if (name=="Blunt Side")					{ buyin="5"; }
	if (name=="Low Middle High")			{ buyin="5"; }
	if (name=="Sandy Beach Cove")			{ buyin="5"; }
	if (name=="St James` Park")				{ buyin="5"; }
	if (name=="Breezy Sun")					{ buyin="5"; }

	if (name=="Boxaringo")					{ buyin="2"; }
	if (name=="Bambalio")					{ buyin="2"; }
	if (name=="Bibulus")					{ buyin="2"; }
	if (name=="Balbilus")					{ buyin="2"; }
	if (name=="Banquerius")					{ buyin="2"; }
	if (name=="Balbus")						{ buyin="2"; }
	if (name=="Balduinus")					{ buyin="2"; }
	if (name=="Brickonly")					{ buyin="2"; }
	if (name=="Baro")						{ buyin="2"; }
	if (name=="Bartinus")					{ buyin="2"; }

	if (name=="Crusty Clam")				{ buyin="1"; }
	if (name=="Grumpy Sand")				{ buyin="1"; }
	if (name=="J9 Supreme")					{ buyin="1"; }
	if (name=="Scattered Sands")			{ buyin="1"; }
	if (name=="Wager Crest")				{ buyin="1"; }
	if (name=="Battery Beach")				{ buyin="1"; }
	if (name=="Big Catch")					{ buyin="1"; }
	if (name=="Steaming Barrel")			{ buyin="1"; }
	if (name=="Rolling Thunder")			{ buyin="1"; }
	if (name=="Rocky Ridge")				{ buyin="1"; }
	if (name=="Somewhere Rainbow")			{ buyin="1"; }

	if (name=="Turbo Crazy")				{ buyin="t1"; }
	if (name=="Turbo Mania")				{ buyin="t1"; }
	if (name=="Turbo Mobster")				{ buyin="t1"; }
	if (name=="Turbo Monster")				{ buyin="t1"; }
	if (name=="Turbo Power")				{ buyin="t1"; }

	return buyin;
}

/* TournamentData::RegisterGame() *********************************************/
void TournamentData::RegisterGame(const char* file)
{
	if (!allowWrite) return;

	int buyin=0;
/*    if (strcmp(t,"\\Imperial Beach")==0)	{ buyin=20; }
    if (strcmp(t,"\\Puerto Escondido")==0)	{ buyin=10; }
    if (strcmp(t,"\\Turbo Bonkers")==0)		{ buyin=10; }
    if (strcmp(t,"\\Big Carona")==0)		{ buyin=10; }
    if (strcmp(t,"\\Cabo San Lucas")==0)	{ buyin=10; }
    if (strcmp(t,"\\Costa Azul")==0)		{ buyin=10; }
    if (strcmp(t,"\\Galapagos")==0)			{ buyin=10; }
    if (strcmp(t,"\\Laguna Beach")==0)		{ buyin=10; } 
    if (strcmp(t,"\\Little Corona")==0)		{ buyin=10; } 
    if (strcmp(t,"\\Nusa Tenggara")==0)		{ buyin=10; } 
    if (strcmp(t,"\\Playa Kandahar")==0)	{ buyin=10; } 
	if (strcmp(t,"\\PokerBreath")==0)		{ buyin=10; }
    if (strcmp(t,"\\True North")==0)		{ buyin=10; }
    if (strcmp(t,"\\Tuamotu Atolls")==0)	{ buyin=10; }
    if (strcmp(t,"\\Turbo Bonkers")==0)		{ buyin=10; }
    if (strcmp(t,"\\Turbo Jam")==0)			{ buyin=10; }
    if (strcmp(t,"\\Turbo Smooch")==0)		{ buyin=10; }
    if (strcmp(t,"\\Long Rager")==0)		{ buyin=5;  }
    if (strcmp(t,"\\Sea Ghost")==0)			{ buyin=5;  }
    if (strcmp(t,"\\Gleeming The Cube")==0)	{ buyin=5;  }
    if (strcmp(t,"\\Blunt Side")==0)		{ buyin=5;  }
    if (strcmp(t,"\\Low High Middle")==0)	{ buyin=5;  }
    if (strcmp(t,"\\Sandy Beach Cove")==0)	{ buyin=5;  }
    if (strcmp(t,"\\St James` Park")==0)	{ buyin=5;  }
    if (strcmp(t,"\\Crusty Clam")==0)       { buyin=1;  }
    if (strcmp(t,"\\Grumpy Sand")==0)       { buyin=1;  }
    if (strcmp(t,"\\J9 Supreme")==0)		{ buyin=1;  }
    if (strcmp(t,"\\Scattered Sands")==0)   { buyin=1;  }
    if (strcmp(t,"\\Turbo Crazy")==0)       { buyin=1;  }
    if (strcmp(t,"\\Turbo Mania")==0)       { buyin=1;  }
    if (strcmp(t,"\\Turbo Mobster")==0)		{ buyin=1;  }
    if (strcmp(t,"\\Turbo Monster")==0)     { buyin=1;  }
    if (strcmp(t,"\\Turbo Power")==0)       { buyin=1;  }
    if (strcmp(t,"\\Wager Crest")==0)       { buyin=1;  }
*/
	char cmd[512];
	memset(cmd,0,sizeof(cmd));
	strcat(cmd,"INSERT INTO `sng_games` (name,date,time,player0,player1,player2,player3,player4,player5,player6,player7,player8,player9,buyin) VALUES ('");
	strcat(cmd,table->getTable().c_str());		strcat(cmd,"','");
	strcat(cmd,table->getGameDate().c_str());	strcat(cmd,"','");
	strcat(cmd,table->getGameTime().c_str());	strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(1));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(2));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(3));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(4));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(5));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(6));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(7));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(8));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(9));		strcat(cmd,"','");
	strcat(cmd,table->getPlayerPlace(10));		strcat(cmd,"','");
	char buff[40]; 
	itoa(buyin,buff,10);
	strcat(cmd,buff); strcat(cmd,"');");
	mysql_query(mysql,cmd); 

}

/* TournamentData::RegisterHand() *********************************************/
void TournamentData::RegisterHand()
{
	if (!allowWrite) return;

	char cmd[5000];
	memset(cmd,0,sizeof(cmd));
	strcat(cmd,"INSERT INTO `sng_hands` (card_h1,card_h2,card_f1,card_f2,card_f3,card_t,card_r,game_id,hand_id,action_id,player,action,stage,action_amt,pot,bb,players_before,handpos,players_before_abs,players_after,players_alive,stack) VALUES ('");
//		scare_big,scare_flush,scare_straight,scare_pair) VALUES ('";
	strcat(cmd,table->getCard(0));											strcat(cmd,"','");
	strcat(cmd,table->getCard(1));											strcat(cmd,"','");
	strcat(cmd,table->getCard(2));											strcat(cmd,"','");
	strcat(cmd,table->getCard(3));											strcat(cmd,"','");
	strcat(cmd,table->getCard(4));											strcat(cmd,"','");
	strcat(cmd,table->getCard(5));											strcat(cmd,"','");
	strcat(cmd,table->getCard(6));											strcat(cmd,"','");
	strcat(cmd,table->getGameID());											strcat(cmd,"','");
	strcat(cmd,table->getNumHands());										strcat(cmd,"','");
	strcat(cmd,action_id++);												strcat(cmd,"','");
	strcat(cmd,table->getActionPlayer());									strcat(cmd,"','");
	strcat(cmd,table->getStage());											strcat(cmd,"','");
	strcat(cmd,table->getCostToCall());										strcat(cmd,"','");
	strcat(cmd,table->getPot());											strcat(cmd,"','");
	strcat(cmd,table->getBB());												strcat(cmd,"','");
	strcat(cmd,table->getPositionBeforeActive());							strcat(cmd,"','");
	strcat(cmd,table->getPositionBeforeAbs());								strcat(cmd,"','");
	strcat(cmd,table->getPositionBeforeAbs());								strcat(cmd,"','");
	strcat(cmd,table->getPositionAfterActive());							strcat(cmd,"','");
	strcat(cmd,table->getPlayersAlive());									strcat(cmd,"','");
	strcat(cmd,table->getStack(table->getActionPlayer()));					strcat(cmd,"') ");
/*	cmd+=hand->d.players_before;	cmd+="','";
	cmd+=hand->d.players_after;		cmd+="','";
	cmd+=hand->d.handpos;			cmd+="','";
	cmd+=hand->d.players_alive;		cmd+="')";
*/
//	printf(cmd.data());
	mysql_query(mysql,cmd); 

	/* WEKA **********************************/
	if (!arff.is_open()) {
		string wekafile=string("C:\\poker\\WEKA\\super.arff");
		//string wekafile=(string("C:\\poker\\WEKA\\")+table->getTable()+" - "+table->getGameDate()+" @ "+table->getGameTime().substr(0,2)+"-"+table->getGameTime().substr(3,2)+"-"+table->getGameTime().substr(6,2)+".arff");
		//cout << wekafile << endl;
		arff.open(wekafile.c_str(),ios::app);

/*		arff << "@RELATION poker" << endl;
		arff << "@ATTRIBUTE card0index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card0suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card1index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card1suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card2index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card2suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card3index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card3suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card4index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card4suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card5index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card5suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE card6index			{v,0,1,2,3,4,5,6,7,8,9,T,J,Q,K,A}" << endl;
		arff << "@ATTRIBUTE card6suit			{v,h,d,c,s}" << endl;
		arff << "@ATTRIBUTE gameID				numeric" << endl;
		arff << "@ATTRIBUTE numHands			numeric" << endl;
		arff << "@ATTRIBUTE actionID			numeric" << endl;
		arff << "@ATTRIBUTE actionPlayer		numeric" << endl;
		arff << "@ATTRIBUTE stage				{p,f,t,r,s}" << endl;
		arff << "@ATTRIBUTE costToCall			numeric" << endl;
		arff << "@ATTRIBUTE pot					numeric" << endl;
		arff << "@ATTRIBUTE bb					numeric" << endl;
		arff << "@ATTRIBUTE posBeforeActive		numeric" << endl;
		arff << "@ATTRIBUTE posBeforeAbs		numeric" << endl;
		arff << "@ATTRIBUTE posAfterActive		numeric" << endl;
		arff << "@ATTRIBUTE posAfterAbs			numeric" << endl;
		arff << "@ATTRIBUTE playersAlive		numeric" << endl;
		arff << "@ATTRIBUTE playersActive		numeric" << endl;
		arff << "@ATTRIBUTE stack				numeric" << endl;
		arff << "@ATTRIBUTE action				{f,c,p,b,a,s,r}" << endl;
		arff << "@DATA" << endl;
*/	}

#define CAST(x) (((x)==' ')?('v'):(x))
	arff << CAST(table->getCard(0).GetIndex())			<< ","	
		 << CAST(table->getCard(0).GetSuit())			<< ","	
		 << CAST(table->getCard(1).GetIndex())			<< ","	
		 << CAST(table->getCard(1).GetSuit())			<< ","	
		 << CAST(table->getCard(2).GetIndex())			<< ","	
		 << CAST(table->getCard(2).GetSuit())			<< ","	
		 << CAST(table->getCard(3).GetIndex())			<< ","	
		 << CAST(table->getCard(3).GetSuit())			<< ","	
		 << CAST(table->getCard(4).GetIndex())			<< ","	
		 << CAST(table->getCard(4).GetSuit())			<< ","	
		 << CAST(table->getCard(5).GetIndex())			<< ","	
		 << CAST(table->getCard(5).GetSuit())			<< ","	
		 << CAST(table->getCard(6).GetIndex())			<< ","	
		 << CAST(table->getCard(6).GetSuit())			<< ","		
		 << table->getGameID()							<< ","			
		 << table->getNumHands()						<< ","		
		 << action_id									<< ","		
		 << table->getActionPlayer()					<< ","		
		 << char(table->getStage())						<< ","		
		 << table->getCostToCall()						<< ","			
		 << table->getPot()								<< ","		
		 << table->getBB()								<< ","			
		 << table->getPositionBeforeActive()			<< ","		
		 << table->getPositionBeforeAbs()				<< ","		
		 << table->getPositionAfterActive()				<< ","		
		 << table->getPositionAfterAbs()				<< ","		
		 << table->getPlayersAlive()					<< ","		
		 << table->getPlayersActive()					<< ","		
		 << table->getStack(table->getActionPlayer())	<< endl;
}
		
/* TournamentData::RegisterAIAct() ********************************************/
void TournamentData::RegisterAIAct(AIStat in)
{
	if (!allowWrite) return;

	in.game_id=table->getGameID();
	in.hand_id=table->getNumHands();
	in.action_id=action_id;
	vecAIStat.push_back(in);
}

/* TournamentData::UpdateAI() *************************************************/
void TournamentData::UpdateAI(int winnings)
{
	if (!allowWrite) return;

	for (unsigned int i=0; i<vecAIStat.size(); i++) {
		char cmd[5000];
		memset(cmd,0,sizeof(cmd));
		strcat(cmd,"INSERT INTO `sng_ai` (version,version_id,moniker,game_id,hand_id,action_id,func,subfunc,action,stage,bet,pot,finalpot,stack,bb,players_alive,players_active,card_h1,card_h2,activeafter,activebefore,activetotal,dangercard,desperate,facecards,flushdraw,flushscare,inrow2,inrow3,inrow4,inrow5,medAgg,overcard,pair,paircard,potodds,straightdraw,straightscare,suited2,suited3,suited4,suited5,totalaction,tripcard,trips,uberAgg,uberDesperate) VALUES ('");

		strcat(cmd, table->getAI()->getAIVer().c_str());				strcat(cmd,"','");
		strcat(cmd, table->getAI()->getAIVerMinor());					strcat(cmd,"','");
		strcat(cmd, table->getAI()->getAIMoniker().c_str());			strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].game_id);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].hand_id);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].action_id);							strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].func.c_str());							strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].sub.c_str());							strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].action);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].stage);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].bet);									strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].pot);									strcat(cmd,"','");
		strcat(cmd, winnings);											strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].stack);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].bb);									strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].players_alive);						strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].players_active);						strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].card_h1);								strcat(cmd,"','");
		strcat(cmd, vecAIStat[i].card_h2);								strcat(cmd,"','");

		TableTexture t=vecAIStat[i].t;
		strcat(cmd, t.activeafter);										strcat(cmd,"','");
		strcat(cmd, t.activebefore);									strcat(cmd,"','");
		strcat(cmd, t.activetotal);										strcat(cmd,"','");
		strcat(cmd, t.dangercard);										strcat(cmd,"','");
		strcat(cmd, t.desperate);										strcat(cmd,"','");
		strcat(cmd, t.facecards);										strcat(cmd,"','");
		strcat(cmd, t.flushdraw);										strcat(cmd,"','");
		strcat(cmd, t.flushscare);										strcat(cmd,"','");
		strcat(cmd, t.inrow2);											strcat(cmd,"','");
		strcat(cmd, t.inrow3);											strcat(cmd,"','");
		strcat(cmd, t.inrow4);											strcat(cmd,"','");
		strcat(cmd, t.inrow5);											strcat(cmd,"','");
		strcat(cmd, t.medAgg);											strcat(cmd,"','");
		strcat(cmd, t.overcard);										strcat(cmd,"','");
		strcat(cmd, t.pair);											strcat(cmd,"','");
		strcat(cmd, t.paircard);										strcat(cmd,"','");
		strcat(cmd, t.potodds);											strcat(cmd,"','");
		strcat(cmd, t.straightdraw);									strcat(cmd,"','");
		strcat(cmd, t.straightscare);									strcat(cmd,"','");
		strcat(cmd, t.suited2);											strcat(cmd,"','");
		strcat(cmd, t.suited3);											strcat(cmd,"','");
		strcat(cmd, t.suited4);											strcat(cmd,"','");
		strcat(cmd, t.suited5);											strcat(cmd,"','");
		strcat(cmd, t.totalaction);										strcat(cmd,"','");
		strcat(cmd, t.tripcard);										strcat(cmd,"','");
		strcat(cmd, t.trips);											strcat(cmd,"','");
		strcat(cmd, t.uberAgg);											strcat(cmd,"','");
		strcat(cmd, t.uberDesperate);									strcat(cmd,"') ");
		mysql_query(mysql,cmd); 
	}

	vecAIStat.clear();
}

/* TournamentData::RegisterGameAI() *******************************************/
void TournamentData::RegisterGameAI(int place)
{
	if (!allowWrite) return;
	cout << "    Added to database sng_aigames" << endl;

	char cmd[5000];
	memset(cmd,0,sizeof(cmd));
	strcat(cmd,"UPDATE `sng_ai` SET `finalplace`='");
	strcat(cmd,place);
	strcat(cmd,"' where game_id=");
	strcat(cmd,table->getGameID());
	strcat(cmd,";");
	mysql_query(mysql,cmd); 

	memset(cmd,0,sizeof(cmd));
 	strcat(cmd,"INSERT INTO `sng_aigames` (`game_id`,`err_minus`,`err_double`,`err_noplay`,`moniker`,`table`,`tablepos`,`date`,`time`,`place`) VALUES ('");
	strcat(cmd,table->getGameID());											strcat(cmd,"','");
	strcat(cmd,errors[ERR_MINUS]);											strcat(cmd,"','");
	strcat(cmd,errors[ERR_DOUBLE]);											strcat(cmd,"','");
	strcat(cmd,errors[ERR_NOPLAY]);											strcat(cmd,"','");
	strcat(cmd,table->getAI()->getAIMoniker().c_str());						strcat(cmd,"','");
	strcat(cmd,table->getTable().c_str());									strcat(cmd,"','");
	strcat(cmd,table->GetNameIndexTrue(table->getAI()->getAIPlayer()));		strcat(cmd,"','");
	strcat(cmd,table->getGameDate().c_str());								strcat(cmd,"','");
	strcat(cmd,table->getGameTime().c_str());								strcat(cmd,"','");
	strcat(cmd,place);														strcat(cmd,"') ");
	if (mysql_query(mysql,cmd)==0) {
//		MessageBox(NULL,"mysql_query 2 error","mysql",0);
	}
	else {
	}

}