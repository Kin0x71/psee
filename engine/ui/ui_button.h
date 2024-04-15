#ifndef _UI_BUTTON_H_
#define	_UI_BUTTON_H_

#include "ui_base.h"
#include "ui_text.h"

typedef void (*_UiButton_OnCursorDown_Callback)(cUiBase*,int);

class cUiButton: public cUiBase
{
public:
	cTexture* pCurrentTexture;
	cTexture* pNormalTexture;
	cTexture* pOverTexture;
	cTexture* pPushedTexture;
	
	cUiText* Caption;
	
	int FontId;
	vec2 TextPos;
	vec2 PushedOffset;
	char* TextNormal;
	char* TextOver;
	char* TextPushed;
	POINTUI3 ColorNormal;
	POINTUI3 ColorOver;
	POINTUI3 ColorPushed;
	
//	_UiButton_OnCursorDown_Callback pUiButton_OnCursorDown_Callback;
//	_UiBase_OnCursorDown_Callback pUiBase_OnCursorDown_Callback;
	
	cUiButton(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	
	void Draw(vec2& pos);
	
	void OnUpdate();
	void OnCursorMove(float x,float y);
	void OnCursorOver();
	void OnCursorOut();
	void OnCursorDown(float x,float y);
	void OnCursorUp(float x,float y);
	
	bool SetNormalParam(cTexture* pTexture,char* str,uchar r=0,uchar g=0,uchar b=0);
	bool SetOverParam(cTexture* pTexture,char* str,uchar r=0,uchar g=0,uchar b=0);
	bool SetPushedParam(cTexture* pTexture,char* str,uchar r=0,uchar g=0,uchar b=0);
};

#endif