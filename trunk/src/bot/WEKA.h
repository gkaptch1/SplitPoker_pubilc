#pragma once

/* class WEKA *****************************************************************/

class WEKA
{
private:
	static ofstream *arff;

public:
	WEKA(void);
	~WEKA(void);

	void wekaRegisterGame();
	void wekaRegisterHand();
};
