#ifndef _UI_GAME_MENU_H_
#define _UI_GAME_MENU_H_

#include "../ui_button.h"

class cUiNamePlates;

class cUiGameMenu
{
public:
	
	cUiButton* ButtonCamControlTop;
	cUiButton* ButtonCamControlBottom;
	
	cUiBase* GameInfoLayer;
	cTexture* GameInfoLayerBackgroundTexture;
	cUiNamePlates* NamePlates;
			
	cUiBase* DebugBackground;
	cTexture* pDebugBackgroundTexture;
	int TextDebugLinesCount;
	cUiText** TextDebugLines;
	
	cUiBase* DBG_Cursor;
	
	cUiGameMenu();
	~cUiGameMenu();
	
	void Create();
	void SetFormatText(int line,char* format,...);
};
#endif