
#ifndef _UI_WINDOWS_H_
#define	_UI_WINDOWS_H_

#include "text.h"
#include <engine/input/input.h>

class cUiBase;

typedef void (*_UiBase_OnCursorMove_Callback)(cUiBase*,int,int);
typedef void (*_UiBase_OnCursorDown_Callback)(cUiBase*,int,int);
typedef void (*_UiBase_OnCursorUp_Callback)(cUiBase*,int,int);
typedef void (*_UiBase_OnCursorOut_Callback)(cUiBase*);

//extern cTexturesControl TexturesControl;

struct win_reg
{
	float x,y,w,h;

	win_reg()
	{
		x=0.0f;
		y=0.0f;
		w=0.0f;
		h=0.0f;
	}

	win_reg(float ix,float iy,float iw,float ih)
	{
		x=ix;
		y=iy;
		w=iw;
		h=ih;
	}
};

enum UI_WINDOW_CLASS
{
	UI_BASE,
	UI_BUTTON,
	UI_TEXT,
	UI_TEXT_EDIT,
	UI_SCR_FLOAT_TEXT,
	UI_MOVING_WRAP,
	UI_SCROLL,
	UI_CONSOLE
};

class cUiBase
{
public:
	cUiBase* child;
	cUiBase* sibling;
	cUiBase* parent;
	UI_WINDOW_CLASS UiWindowClass;
	win_reg reg;
	win_reg tex;
	char* name;
	
	c_shader *pshader;
	_shader_tech_list* pIShaderTech;
	cTexture* pBackgroundTexture;
	
	bool focus_keeper_flag;
	
	float cursorx;
	float cursory;
	bool cursor_capture;
	bool focus_keep;

	_UiBase_OnCursorMove_Callback pUiBase_OnCursorMove_Callback;
	_UiBase_OnCursorDown_Callback pUiBase_OnCursorDown_Callback;
	_UiBase_OnCursorUp_Callback pUiBase_OnCursorUp_Callback;
	_UiBase_OnCursorOut_Callback pUiBase_OnCursorOut_Callback;
	
	cUiBase();
	cUiBase(const char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	~cUiBase();
	
private: void base_constructor(const char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	void base_destructor();
public:
	
	void ConvertToScreenCoord(float x, float y,vec2 *vret);
	virtual void Draw(vec2& pos);
	
	bool SetBackground(cTexture* pTexture);
	bool SetBackground(cTexture* pTexture,float tex_x,float tex_y,float tex_w,float tex_h);

private:
	cUiBase* get_sub_win_by_name_r(cUiBase* win,char* name);
public:
	cUiBase* GetSubWinByName(char* name);
	
	virtual void OnCreate(){};
	virtual void OnUpdate(){};
	virtual void OnCursorMove(float x,float y){/*printf("CursorMove BASE %s",name);*/};
	virtual void OnCursorOver(){};
	virtual void OnCursorOut(){};
	virtual void OnCursorDown(float x,float y);//{/*printf("BASE:OnCursorDown %s",name);*/};
	virtual void OnCursorUp(float x,float y){};
	virtual void OnFocusKeep(){};
	virtual void OnFocusLost(){};
	virtual void OnKeyDown(_KEY_KODE_ key){};
};

#endif