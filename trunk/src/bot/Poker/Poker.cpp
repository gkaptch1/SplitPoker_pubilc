// Poker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/*
int main()
{
	ofstream out("c:\\poker\\pf-rank-4player.stat");

	int cards[13][13];
	memset(cards,0,sizeof(cards));

	const int hands=4;
	PokerHand b;
	vector<PokerHand> h;
	for (int i=0; i<hands; i++) {
		h.push_back(PokerHand());
	}

	for (int i=0; i<100000; i++) {
		Deck d;
		d.Shuffle();

		b=d.Deal(5);
		for (int j=0; j<hands; j++) {
			h[j]=d.Deal(2)+b;
		}

		sort(h.begin(),h.end());
		for (int j=0; j<hands; j++) {
			cards[(h[j])[0].GetRank()][(h[j])[1].GetRank()]+=j;
			cards[(h[j])[1].GetRank()][(h[j])[0].GetRank()]+=j;
		}

		if (i%10==0)cout << i << endl;
	}

	cout << endl << endl;

	for (int i=0; i<13; i++) {
		for (int j=0; j<13; j++) {
			int max=-1,locx=-1,locy=-1;
		
			for (int x=0; x<13; x++) {
				for (int y=0; y<13; y++) {
					if (cards[x][y]>max) {
						max=cards[x][y];
						locx=x;
						locy=y;
					}
				}
			}

			PokerHand q;
			q += Card(locx);
			q += Card(locy);

			q.print();
			cards[locx][locy]=-2;
			cout << endl;
		
		}
	}

	while (1) ;

	return 0;
}

*/