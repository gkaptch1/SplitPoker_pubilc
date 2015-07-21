#ifndef _TournamentData_H_
#define _TournamentData_H_

#include <vector>
#include <string>
#include <mysql.h>
#include "../poker/Card.h"
#include "MsgQueue.h"
#include "HoldemTournamentAI.h"
#include "WEKA.h"

using namespace std;

class MsgQueue;
struct StatusList;

// controls whether or not we should update the database midgame, or wait till it's over
#define _DB_STORE_ON_SECOND_PASS

/* Error Codes ****************************************************************/
#define ERR_MINUS				0
#define ERR_DOUBLE				1
#define ERR_NOPLAY				2
#define MAX_ERRORS				10

/* MySQL table structs ********************************************************/
struct AIStat
{
	string func,sub,version;

	char stage,
		action;

	int game_id,
		hand_id,
		action_id;

	int players_alive,
		players_active;

	Card card_h1,
		card_h2;

	int bet,
		pot,
		finalpot,
		stack,
		bb;

	int winnings;

	TableTexture t;
};

/* class TournamentData *******************************************************/
class TournamentData {

private:
	MsgQueue*		table;

	int action_id;
	bool allowWrite;

	int errors[MAX_ERRORS];

	vector<AIStat> vecAIStat;

	ofstream arff;

public:
    TournamentData(MsgQueue* t);
    ~TournamentData();

	const static string getBuyinFromName(const string& name);

	void enableWrite()	{ allowWrite=true; }
	void eraseDB();

	void registerError(int err) { errors[err]++; }

	void RegisterGame(const char* file);
	void RegisterHand();

	void RegisterAIAct(AIStat in);
	void UpdateAI(int winnings);
	void RegisterGameAI(int place);
};

#endif
