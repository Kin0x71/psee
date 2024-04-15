#ifndef _UI_SCROLL_H_
#define _UI_SCROLL_H_

#include "../ui_base.h"
#include "../ui_button.h"

typedef void (*_UiScroll_OnScroll_Callback)(cUiBase*,float);

class cUiScroll: public cUiBase
{
public:
	enum SCROLL_TYPE
	{
		SCROLL_TYPE_H,
		SCROLL_TYPE_V
	};

	SCROLL_TYPE ScrollType;

	cUiButton* ButtonA;
	cUiButton* ButtonB;
	cUiButton* ButtonControl;
	
	float ButtonControl_LastCursorPos;
	_UiScroll_OnScroll_Callback pUiScroll_OnScroll_Callback;

	cUiScroll(char* winname,float x,float y,float w,float h,cUiBase* parent=0):cUiBase(winname,x,y,w,h,parent)
	{
		UiWindowClass=UI_SCROLL;
		
		//this->base_constructor(winname,x,y,w,h,parent);
		
		ScrollType=SCROLL_TYPE_H;

		ButtonA=0;
		ButtonB=0;
		ButtonControl=0;
		
		ButtonControl_LastCursorPos=-1.0f;
		pUiScroll_OnScroll_Callback=0;
	}

	~cUiScroll()
	{
		if(ButtonA){
			delete ButtonA;
			ButtonA=0;
		}

		if(ButtonB){
			delete ButtonB;
			ButtonB=0;
		}

		if(ButtonControl){
			delete ButtonControl;
			ButtonControl=0;
		}
	}

	void OnCreate();
};

#endif