#ifndef _PlayerStats_H_
#define _PlayerStats_H_

/* class PlayerStats **********************************************************/

class PlayerStats {
private:
      // perm data
      int vbet,vraise,vcall,vcheck,vfold;
      int totalhands,flop,turn,river,potswon,showdownlost,showdown,totalgames;
      int checkraise,checkcall,checkfold;
      int place[10];
	  int place_eliminated;

      double tight,aggressive;

      char player[14];

      // tmp data

      bool havechecked;
      bool alive,active,isallin;
      int flopped;

public:
      int stack;

    // FIXME: Apparently there's a major bug with the player variable's data space being overwritten.
    // Player's names change midgame on some compiliers.

    // FIXME: PlayerStats can only handle about 20 more bytes of data for some reason???  Any more variables --> crash.

public:
      PlayerStats();
      PlayerStats(const char* name);
      ~PlayerStats();

      void Combine(PlayerStats* p);

      /* File Based I/O */
      int LoadPlayerFile(char* p);
      int WriteFile();

      /* Realtime I/O */
      void ScreenStatsDumpTitle();
      void ScreenStatsDump();

      char* BuffStatsDumpTitle();
      char* BuffStatsDump();

      /* Variables */
      int GetBet()      { return vbet;  }
      int GetRaise()    { return vraise;}
      int GetCall()     { return vcall; }
      int GetCheck()    { return vcheck;}
      int GetFold()     { return vfold; }

      int GetFlop()     { return flop;  }
      int GetTurn()     { return turn;}
      int GetRiver()    { return river; }
      int GetPotsWon()  { return potswon;}
//      int GetShowdownWon()     { return (showdown-showdownlost)/showdown; }    // TODO: IMPLIMENT!

      double GetTight()      { return tight;}
      double GetAggressive() { return aggressive; }

//      int totalhands,flop,turn,river,potswon,showdownlost,showdown,totalgames;
//      int checkraise,checkcall,checkfold;

      /* Communication */
      char* GetName();
	  int GetPlace();

	  int GetCheckAct(int act);

      void NewRound();
      void Flop();
      void Turn();
      void River();

      void Showdown();
      void win();

      void bet();
      void raise();
      void allin();
      void call();
      void check();
      void fold();

      bool isAlive();
      bool isActive();
	  bool isAllin();
      void eliminated(int p);
};

#endif
