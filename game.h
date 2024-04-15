#ifndef _GAME_H_
#define _GAME_H_

#include <engine/engine.h>

class cGame : public cGameBase{
public:
	int PlayerStartPositionX = -1;
	int PlayerStartPositionY = -1;

	void Create();
	void UpdateBeforeCalculations();
	void UpdateAfterCalculations();
	void UpdateAfterRender();
};

#endif