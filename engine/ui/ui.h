#ifndef _UI_H_
#define _UI_H_

#include "ui_core.h"
#include "windows/ui_game_menu.h"
#include "windows/ui_uett.h"

class cUi
{
public:
	cUiCore UiCore;

	cUiUett* LoadScreen;

	cUiGameMenu* GameMenu;
	cUiTextEdit* TextEdit;

	cUi()
	{
		LoadScreen=0;
		GameMenu=0;
		TextEdit=0;
	}

	void Init();

	void CreateLoadScreen();
	void DeleteLoadScreen();
	void CreateGameMenu();
};

#endif