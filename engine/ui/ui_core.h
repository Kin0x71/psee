#ifndef _UI_CORE_H_
#define	_UI_CORE_H_
#include "text.h"
#include "ui_base.h"
#include "ui_moving_wrap.h"
#include "ui_button.h"
#include "ui_text.h"
#include "ui_text_edit.h"
#include "ui_scr_float_text.h"
#include "windows/ui_scroll.h"
#include "windows/ui_console.h"
#include <stdarg.h>

enum _KEY_KODE_;

class cUiCore
{
public:
	
enum _UI_EVENT_
{
	_UI_EVENT_ONCURSORMOVE,
	_UI_EVENT_ONCURSORDOWN,
	_UI_EVENT_ONCURSORUP
};

struct _ui_event_stack_
{
	_ui_event_stack_* next;
	_ui_event_stack_* back;
	cUiBase* win;
	vec2 cursor;
	
	_ui_event_stack_()
	{
		next=0;
		back=0;
		win=0;
		
	}
	~_ui_event_stack_()
	{
		if(next)delete next;
	}
};

struct _tex_page_item
{
#define _TEXT_PAGE_SIZE 3
	_tex_page_item* prev;
	_tex_page_item* next;

	cTexture* tex_array[_TEXT_PAGE_SIZE];

	_tex_page_item()
	{
	}
	~_tex_page_item()
	{
	}
};

	_ui_event_stack_* EventSteck;
	_ui_event_stack_* EventSteckLast;
	cUiBase* UiWindowRoot;

	shaders_container* pUiShadersContainer;

	//cIShaderMeshControl* pIShaders;
	//c_shader *pshader;
	//mx4 ProjMx;
	int pIShaderTechBase;
	int pIShaderTechText;
	int pIShaderTechButton;
	int pIShaderTechSizingTexture;

	_tex_page_item* TextureList;

	cMesh PlaneBorderMesh;
	
	bool CursorAboveUi;
	cUiBase* CursorAboveWindow;
	cUiBase* FocusedWindow;
	
	vec2 LastCursorPos;
	
	cUiBase* pUiBase_Event_Post_Caller;
	_UiBase_OnCursorMove_Callback pUiBase_OnCursorMove_PostCallback;
	_UiBase_OnCursorDown_Callback pUiBase_OnCursorDown_PostCallback;
	_UiBase_OnCursorUp_Callback pUiBase_OnCursorUp_PostCallback;
	_UiBase_OnCursorOut_Callback pUiBase_OnCursorOut_PostCallback;
	
	cUiCore()
	{
		UiWindowRoot=0;
		//pIShaders=0;
		//pshader=0;
		pUiShadersContainer = 0;
		CursorAboveUi=false;
		CursorAboveWindow=0;
		FocusedWindow=0;
		pIShaderTechBase=0;
		pIShaderTechText=0;
		pIShaderTechButton=0;
		pIShaderTechSizingTexture=0;
		TextureList=new _tex_page_item();
		pUiBase_Event_Post_Caller=0;
		pUiBase_OnCursorMove_PostCallback=0;
		pUiBase_OnCursorDown_PostCallback=0;
		pUiBase_OnCursorUp_PostCallback=0;
		EventSteck=new _ui_event_stack_();
		EventSteckLast=0;
	}
	~cUiCore()
	{
		if(EventSteck)delete EventSteck;
	}
	
	//cTexture* GetTextureByHash(_md5hash* pmd5hash);
	uint CreateTextureJpg(char* fname_c,char* fname_a,int wrap_w=GL_REPEAT,int wrap_h=GL_REPEAT);

	void _ui_event_stack_Push(cUiBase* win,float x,float y);
	
	void Init();
	void Free();
	void PrintToScreen(uint FontId,uint Xofs,uint Yofs,char* format,...);

	void UpdateWindows();
	void DrawWindows();
	void uibase_update(cUiBase* win);
	void uibase_redraw(cUiBase* win,vec2* parent_pos=0);

	cUiBase* CreateUiWindow(UI_WINDOW_CLASS win_class,char* name,const win_reg& rect,cUiBase* parent=0,bool visible=true);
	
	void ShowWindow(cUiBase* win);
	void UnlinkWindow(cUiBase* win);
	void UpTopWindow(cUiBase* win);

	bool uibase_cursordown_event(cUiBase* win,vec2* cursor_pos,vec2* parent_pos=0);
	bool UiBaseCursorDownEvent(int x,int y);
	
	bool uibase_cursorup_event(cUiBase* win,vec2* cursor_pos,vec2* parent_pos=0);
	bool UiBaseCursorUpEvent(int x,int y);

	void uibase_cursormove_event(cUiBase* win,vec2* cursor_pos,vec2* parent_pos=0);
	bool UiBaseCursorMoveEvent(int x,int y);

	void UiBaseKeyDownEvent(_KEY_KODE_ key);
	
	void uibase_reset_cursor(cUiBase* win);

	void uibase_free_focus(cUiBase* win,cUiBase* exception_win);
};

#endif

