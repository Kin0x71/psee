#ifndef _UI_MOVING_WRAP_H_
#define _UI_MOVING_WRAP_H_

#include "ui_base.h"
#include "ui_text.h"

//#define GL_CLAMP_TO_BORDER		0x812D

class cUiMovingWrap:public cUiBase
{
public:
	//cTexture* pTopTexture;
	vec4 TextureRec;
	cUiText* Caption;
	
	float CursorDownPosX;
	float CursorDownPosY;
	bool CursorCapture;
	
	float BorderLeft;
	float BorderTop;
	float BorderRight;
	float BorderBottom;
	
	vec4 VertBorder[16];
	vec4 VertBorderO[16];
	
	cUiMovingWrap(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	
	void Init();
	void Draw(vec2& pos);
	
	void OnUpdate();
	void OnCursorMove(float x,float y);
	void OnCursorOut();
	void OnCursorDown(float x,float y);
	void OnCursorUp(float x,float y);
};

#endif