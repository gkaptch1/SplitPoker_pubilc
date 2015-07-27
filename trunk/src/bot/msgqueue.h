#ifndef _MsgQueue_H_
#define _MsgQueue_H_
    
/* #includes ******************************************************************/

#include <vector>
#include <iostream>
#include "../poker/Card.h"
#include "playerstats.h"

using namespace std;

class HoldemTournamentAIBase;
class TournamentData;
class PlayerStats;

struct AIStat;

#define VIEW_LIGHT			0
#define VIEW_DEFAULT		1
#define VIEW_PLAYER			2

/* struct EventList ***********************************************************/
struct EventList
{
    bool newgame;
    bool newhand;
    bool newstage;
    bool blindsup;
    bool action;
};

struct StatusList
{
    char stage;
	char lastaction;
    int handcount;
    int bet[10],extrabet,allins;
    int pot;
    int active;
    int livingplayers;
    int dealer;
    int bb;
    int sb;

//    vector<Card> cards;
    // cards[0] and cards[1] are actually hole cards
    // cards[2], cards[3] and cards[4] are the flop
    // cards[5] iss the turn and cards[6] is the river
    // Other spaces seem to never be referenced... still not sure why they are there...
    Card cards[20];  
};

/* class MsgQueueBase *********************************************************/
class MsgQueueBase
{
protected:
    StatusList			status;
    EventList			events;

    PlayerStats			*players[20];  // WTF?
	PokerHand			holeCards[20];

	TournamentData			*db;
    HoldemTournamentAIBase	*ai;
	int gameid;

	int view;

	int starttime;
	int rounds;
	int totaltime;

	bool allowPlay;
	bool dryRun;

	string filename;
    char blinds[100];
    char buyin[100];
	int buyint;

    FILE *filedesc;

	string table;

public:
	MsgQueueBase();
	~MsgQueueBase();

	/* poker interface */
	virtual void loadTable()					= 0;
	virtual void closeTable()					= 0;
	virtual void update()						= 0;
	virtual void updateHoleCards()				= 0;
	virtual int totalCashOnTable()				= 0;
	virtual int GetNameIndexTrue(int n)			= 0;

    virtual int DoCheck()						= 0;
    virtual int DoCall()						= 0;
    virtual int DoRaise()						= 0;
    virtual int DoBet()							= 0;
    virtual int DoBet(int num)					= 0;
    virtual int DoFold()						= 0;

	/* display */
	ostream& cur(int x, int y);

	/* setters for StatusList and EventList */
	void setStatusStage(char s) 				{ status.stage = s; 						}
	void setStatusLastAction(char la)			{ status.lastAction = la;					}
	void setStatushandCount(int hc)				{ status.handcount = hc;					}
	void setStatusExtraBet(int eb)				{ status.extrabet = eb;						}
	void setStatusAllIns(int alls)				{ status.allins = alls;						}
	// TODO void setStatusbet(int[])
	void setStatusPots(int pt)					{ status.pot = pt;							}
	void setStatusActive(int a)					{ status.active = a;						}
	void setStatusLivingPlayers(int lp)			{ status.livingplayers = lp;				}
	void setStatusDealer(int dlr)				{ status.dealer = dlr;						}
	void setStatusBigBlind(int blind)			{ status.bb = blind;						} 
	void setStatusSmallBline(int blind)			{ status.sb = blind;						}

	void setStatusHoleCards(Card c1, Card c2)   { setCard(c1, 0); setCard(c2,1);			}
	void setStatusFlopCards(Card c1, Card c2, Card c3) {setCard(c1, 2); setCard(c2,3); setCard(c3,4);}
	void setStatusTurnCard(Card c1)				{ setCard(c1, 5);							}
	void setStatusRiverCard(Card c1)			{ setCard(c1, 6);							}

	void print();
	void printLight();
	void printLightHeader();
	void printDefaultView();
	void printDefaultBG();
	void printPlayerView();
	void printPlayers(int x, int y);
	void printPlayer(int player, int x, int y);

	void setPrintStyle(int style)				{ view=style;								}

	/* game info */
	HoldemTournamentAIBase* getAI()				{ return ai;								}

	bool getDryRun()							{ return dryRun;							}
	void setDryRun(bool x)						{ dryRun=x;									}

	bool getNewGame()							{ return events.newgame;					}
	bool getNewHand()							{ return events.newhand;					}
	bool getNewStage()							{ return events.newstage;					}
	bool getBlindsUp()							{ return events.blindsup;					}
	bool getAction()							{ return events.action;						}

	int getGameID()								{ return gameid;							}
	string getTable()							{ return table;								}

	int getNumHands()							{ return status.handcount;					}
	int getStage()								{ return status.stage;						}
	int getAveStack()							{ return 10000/status.livingplayers;		}
	int getPot()								{ return status.pot;						}
	int getBB()									{ return status.bb;							}
	int getBet(int player)						{ return status.bet[player];				}
	int getDealer()								{ return status.dealer;						}
	int getActionPlayer()						{ return status.active;						}
	int getPlayersDead()						{ return 10-status.livingplayers;			}
	int getPlayersAlive()						{ return status.livingplayers;				}
	int getPlayersActive();

	char getLastAction()						{ return status.lastaction;					}
	Card getCard(int n)							{ return status.cards[n];					}
	double getPotOdds()							{ return getPotOdds(getCostToCall());		}
	double getPotOdds(int cost)					{ return status.pot/(double)cost;			}
	int getCostToCall()							{ return getCostToCall(getActionPlayer());	}
    int getCostToCall(int dude);
	bool isEveryPlayerAllin();

	PokerHand getHand()							{ return status.cards[0]+status.cards[1];  }
	PokerHand getBoard()						{ return status.cards[2]+status.cards[3]+status.cards[4]+status.cards[5]+status.cards[6];  }
	PokerHand getFlop()							{ return status.cards[2]+status.cards[3]+status.cards[4]; }
	Card getTurn()								{ return status.cards[5]; }
	Card getRiver()								{ return status.cards[6]; }

	/* player info */
	bool isAlive(int player);
	bool isActive(int player);
	bool isAllin(int player);
    int getNameIndex(const char* name, bool addPlayer=true);
	int getStack(int player)					{ return (players[player]==NULL)?(0):(players[player]->stack);	}
	PlayerStats getPlayer(int n)				{ return *players[n];				}
	char* getName(int index)					{ return (players[index]==NULL)?(""):(players[index]->GetName());	}  
	char* getPlayerPlace(int place);

	int getCheckAct(int player, int act=0);

	int getUTGPlayer()							{ return getNextActivePlayer(getNextActivePlayer(getNextActivePlayer(status.dealer))); }
    int getNextActivePlayer(int p);
    int getNextAllivePlayer(int p);
    int getLastActivePlayer(int p);
    int getPositionBeforeActive(int dude);
    int getPositionBeforeAbs(int dude);
    int getPositionAfterActive(int dude);
    int getPositionAfterAbs(int dude);
    int getPositionBeforeAbs()					{ return getPositionBeforeAbs(getActionPlayer());		}
	int getPositionBeforeActive()				{ return getPositionBeforeActive(getActionPlayer());	}
	int getPositionAfterAbs()					{ return getPositionAfterAbs(getActionPlayer());		}
	int getPositionAfterActive()				{ return getPositionAfterActive(getActionPlayer());		}
	int getHandsUntilBB(int player);
	int getHandsUntilBBInc()					{ return 10-status.handcount%10;			}

	PokerHand GetHoleCards(int player)			{ return holeCards[player];			}

	/* debug and testing */
	void regAIStat(const AIStat& ai);
	void checkCashIntegrity();
	void setCard(Card c, int n)					{ status.cards[n]=c;				}
	void toggleAllowPlay(bool in)				{ allowPlay=in;						}
};

/* class MsgQueue *************************************************************/
class MsgQueue : public MsgQueueBase
{
private:

    /* Windows Admin */
    int msg_rec;
    bool file_input;
    HWND RVPhwnd;
    HWND textbox;
	HWND hwndFront;
	HDC RVPhdc;
            
    int nextline;

    /* Table Admin */
    int eliminations;
	int playerOffset;

	string gameTime;
	string gameDate;

	bool seated;

private:
	/* processing */
    int ParseNewMessages();
    int ProcessMessage(char* msg, char* nextline);
	void parseTableInfoFromFile(const char* file);
	void processErrorMessage(const char* file);
	void CalcPlayerOffset();
	void reset();

public:
    MsgQueue();
    ~MsgQueue();

    /* File Based Input */
    int ParseFile(const char* file);
    int ParseFileNextLine();
	void ClearSNGGames();

	string static getCurrentTime();
	string static getCurrentDate();
	string getGameTime()						{ return gameTime;					}
	string getGameDate()						{ return gameDate;					}

    /* Realtime Input */
    int LoadTable(char* buy, char* b, char* t);
    int LoadDumpFile();
    int CloseDumpFile();

	int RegisterAction(char* command, char* name);
	void PushAction(int loc, int player, int amt=0, bool analyzed=false);

	void virtual loadTable();
	void virtual closeTable();
	void virtual update();
	void virtual updateHoleCards();
	void ackPlace();
	void takeSeat();

	bool tableGood();

	int virtual totalCashOnTable()				{ return 10000;				 		}
	int virtual DoCheck()						{ return MouseClick(230,330); 		}
    int virtual DoCall()          				{ return MouseClick(350,330); 		}
    int virtual DoRaise()         				{ return MouseClick(460,330); 		}
    int virtual DoBet()           				{ return MouseClick(460,330); 		}
    int virtual DoBet(int num);
	int virtual DoFold()          				{ return MouseClick(570,330);		}

	void stayActive();

    /* Variables */
    HWND GetWindowHandle() { return RVPhwnd; }
	int GetNameIndexTrue(int n);
};

#endif
