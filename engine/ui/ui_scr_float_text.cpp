#include "ui_scr_float_text.h"
#include "../screen.h"
#include "../camera.h"

extern cCamera Camera;
extern cScreen Screen;

cUiScrFloatText::cUiScrFloatText(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiText(winname,x,y,w,h,parent)
{
	UiWindowClass=UI_SCR_FLOAT_TEXT;
	
//	this->base_constructor(winname,x,y,w,h,parent);

	FontId=0;
	
	buff=0;
	text_len=0;
	
	Create(int(w),int(h));
}

cUiScrFloatText::~cUiScrFloatText()
{
	//DBG_LOG("cUiScrFloatText::~cUiScrFloatText %x %s",this,name);
}

int cUiScrFloatText::SetPosScreen(float x,float y)
{
	if(x-(reg.w/2.0f)<0.0f)reg.x=0.0f;
	else if(x+(reg.w/2.0f)>float(Screen.width))reg.x=float(Screen.width)-reg.w;	else reg.x=x-(reg.w/2.0f);
	
	if(y<0.0f)reg.y=0.0f;
	else if(y+reg.h>float(Screen.height))reg.y=float(Screen.height)-reg.h;
	else reg.y=y;
	
	return 0;
}

int cUiScrFloatText::SetPosWorld(float x,float y,float z)
{
	vec2 viewSize=vec2(float(Screen.width),float(Screen.height));
	vec2 viewOffset=vec2(0.0f,-float(Screen.height));
		
	vec4 clipSpacePos = vec4(x,y,z, 1.0f)*Camera.ViewProjMx;
	vec3 ndcSpacePos = vec3(clipSpacePos.x,clipSpacePos.y,clipSpacePos.z) / clipSpacePos.w;
	vec2 windowSpacePos = (vec2(ndcSpacePos.x+1.0f,ndcSpacePos.y+1.0f) / 2.0f) * viewSize + viewOffset;
	
	SetPosScreen(
			windowSpacePos.x,
			-windowSpacePos.y
			);
	return 0;
}