#ifndef _UI_TEXT_H_
#define	_UI_TEXT_H_
#include "ui_base.h"
#include "text.h"
#include "ui_text_base.h"

class cUiText: public cUiTextBase
{
public:
	int text_len;
	uint FontId;
	
	cUiText(){}
	cUiText(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	~cUiText();
	
	void SetFont(int fid);
	void AllockBuff(int size);
	void SetText(char* str,uchar r=0,uchar g=0,uchar b=0);
	void SetTextColor(uchar r=0,uchar g=0,uchar b=0);
	void SetFormatText(uchar r,uchar g,uchar b,char* format,...);

	//void Draw(vec2& pos);
};

#endif