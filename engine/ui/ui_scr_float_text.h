#ifndef _UI_SCR_FLOAT_TEXT_H_
#define _UI_SCR_FLOAT_TEXT_H_

#include "ui_text.h"

class cUiScrFloatText;

class cUiScrFloatText:public cUiText
{
public:
	
	cUiScrFloatText(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	~cUiScrFloatText();
	
	int SetPosScreen(float x,float y);
	int SetPosWorld(float x,float y,float z);
};

#endif