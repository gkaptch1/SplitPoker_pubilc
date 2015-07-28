#ifndef _poker_h_
#define _poker_h_

/* Generic Poker Constants ****************************************************/

#define DECKSIZE							52
#define SUITS								4
#define RANKS								13

#define LOOSE_NO							0
#define LOOSE_LITTLE						1
#define LOOSE_SOME							3
#define LOOSE_VERY							4						

#define AGGRESSION_NO						0
#define AGGRESSION_LITTLE					1
#define AGGRESSION_SOME						3
#define AGGRESSION_VERY						4						

/* Hold'em Constants **********************************************************/

#define HOLDEM_PLAYERS						10

#define PF_GRP_A						    1
#define PF_GRP_B							2
#define PF_GRP_C							2*2
#define PF_GRP_D							2*2*2
#define PF_GRP_E							2*2*2*2
#define PF_GRP_F							2*2*2*2*2
#define PF_GRP_G							2*2*2*2*2*2
#define PF_PAIR								2*2*2*2*2*2*2
#define PF_SUITED							2*2*2*2*2*2*2*2
#define PF_CONNECTED						2*2*2*2*2*2*2*2*2
#define PF_SUITCONN							2*2*2*2*2*2*2*2*2*2
#define PF_ALLIN_A							2*2*2*2*2*2*2*2*2*2*2
#define PF_ALLIN_B							2*2*2*2*2*2*2*2*2*2*2*2
#define PF_ALLIN_C							2*2*2*2*2*2*2*2*2*2*2*2*2

#define STAGE_PREFLOP                       'p'
#define STAGE_FLOP                          'f'
#define STAGE_TURN                          't'
#define STAGE_RIVER                         'r'
#define STAGE_SHOWDOWN                      's'

#define FOLD                                'f'
#define CHECK                               'p'
#define CALL                                'c'
#define ALLIN                               'a'
#define BET                                 'b'
#define RAISE                               'r'
#define SHOW							    's'
#define STAGE_CHANGE						'd'

#endif