#ifndef _UI_TEXT_BASE_H_
#define	_UI_TEXT_BASE_H_

#include "ui_base.h"
#include "text.h"

class cUiTextBase: public cUiBase
{
public:
	char* buff;
	int text_len;
	uint FontId;
	cTexture* pTexture;
	
	cUiTextBase();	
	cUiTextBase(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	~cUiTextBase();
	
	bool Create(int w,int h);
	void OnUpdate();
	void Draw(vec2& pos);
	
	void SetBuffer(char* str,int len,int x,int y,uint FontId,uchar r=0,uchar g=0,uchar b=0);
	void UpdateTexture();
};

#endif