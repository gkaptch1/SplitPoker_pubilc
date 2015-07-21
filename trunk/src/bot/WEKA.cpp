#include "StdAfx.h"
#include "WEKA.h"

/* Constructor / Destructor ***************************************************/
WEKA::WEKA(void)
{
	arffopen("C:\\poker\\arff.arff",ios::app);
	arff << "test-weka" << endl;
}

WEKA::~WEKA(void)
{
	arff.close();
}

/* WEKA::wekaRegisterGame() ***************************************************/
void WEKA::wekaRegisterGame()
{
	arff << "game" << endl;
}

/* WEKA::wekaRegisterHand() ***************************************************/
void WEKA::wekaRegisterHand()
{
//	arff << "  hand" << endl;
}