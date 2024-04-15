#include "ui_scroll.h"
#include "../ui.h"
#include "../../input.h"
#include "../../event.h"

extern cUi Ui;
extern cInput Input;

void cUiScroll_EventCursorMove(float x,float y,cUiBase* win)
{
	cUiScroll* SelfScrollBar=(cUiScroll*)win;
	
	if(SelfScrollBar->ButtonControl_LastCursorPos!=-1.0f)
	{
		float offset=y-SelfScrollBar->ButtonControl_LastCursorPos;
		
		if(SelfScrollBar->ButtonControl->reg.y+SelfScrollBar->ButtonControl->reg.h+offset>SelfScrollBar->ButtonB->reg.y || SelfScrollBar->ButtonControl->reg.y+offset<SelfScrollBar->ButtonA->reg.y+SelfScrollBar->ButtonA->reg.h)return;
				
		SelfScrollBar->ButtonControl->reg.y+=offset;
		
//L		SelfScrollBar->ButtonControl_LastCursorPos=Input.Cursor.Pos.y;
		
		if(SelfScrollBar->pUiScroll_OnScroll_Callback)SelfScrollBar->pUiScroll_OnScroll_Callback(SelfScrollBar,(SelfScrollBar->reg.y/SelfScrollBar->reg.h)*100.0f);
	}
}

void cUiScroll_EventCursorButton(int bid,uint event,cUiBase* win)
{
//L	if(bid==1 && event==CURSOR_UP){
//L		cEvent::EventCursorMove.Unsubscribe(win);
//L		cEvent::EventCursorButton.Unsubscribe(win);
//L	}
}

void cUiScroll_CursorDown_ScrollUp(cUiBase* self,int x,int y)
{
	//cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ScrollUp:%s",TextView->name);
	
	//TextView->ScrollOffsetV(1);
}

void cUiScroll_CursorDown_ScrollDown(cUiBase* self,int x,int y)
{
	//cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ScrollDown:%s",TextView->name);
	
	//TextView->ScrollOffsetV(-1);
}

void cUiScroll_CursorDown_ButtonControl(cUiBase* self,int x,int y)
{
	//cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ButtonControl:%s",TextView->name);
//L	((cUiScroll*)self->parent)->ButtonControl_LastCursorPos=Input.Cursor.Pos.y;
//L	cEvent::EventCursorMove.Subscribe(cUiScroll_EventCursorMove,self->parent);
//L	cEvent::EventCursorButton.Subscribe(cUiScroll_EventCursorButton,self->parent);
}

void cUiScroll_CursorUp_ButtonControl(cUiBase* self,int x,int y)
{
	((cUiScroll*)self->parent)->ButtonControl_LastCursorPos=-1.0f;
}

void cUiScroll_CursorOut_ButtonControl(cUiBase* self)
{
	//printf("cUiConsole_CursorOut_ButtonControl");
	((cUiScroll*)self->parent)->ButtonControl_LastCursorPos=-1.0f;
}

void cUiScroll_CursorMove_ButtonControl(cUiBase* self,int x,int y)
{		
	cUiScroll* SelfScrollBar=(cUiScroll*)self->parent;
	
/*L	if(SelfScrollBar->ButtonControl_LastCursorPos!=-1.0f)
	{
		float offset=Input.Cursor.Pos.y-SelfScrollBar->ButtonControl_LastCursorPos;
		
		if(self->reg.y+self->reg.h+offset>SelfScrollBar->ButtonB->reg.y || self->reg.y+offset<SelfScrollBar->ButtonA->reg.y+SelfScrollBar->ButtonA->reg.h)return;
				
		self->reg.y+=offset;
		
		SelfScrollBar->ButtonControl_LastCursorPos=Input.Cursor.Pos.y;
		
		if(SelfScrollBar->pUiScroll_OnScroll_Callback)SelfScrollBar->pUiScroll_OnScroll_Callback(SelfScrollBar,(SelfScrollBar->reg.y/SelfScrollBar->reg.h)*100.0f);
	}*/
}

void cUiScroll::OnCreate()
{
	cTexture* psvbtn=TextureCreateFromFilePNG("data/ui/scroll/svbtn.png");
	cTexture* psvbto=TextureCreateFromFilePNG("data/ui/scroll/svbto.png");
	cTexture* psvbtp=TextureCreateFromFilePNG("data/ui/scroll/svbtp.png");
	
	ButtonA=(cUiButton*)Ui.UiCore.CreateUiWindow(UI_BUTTON,"ButtonA",win_reg(2.0f,0.0f,psvbtn->surf.w*2.0f,psvbtn->surf.h*2.0f),this);
	ButtonA->FontId=1;
	ButtonA->SetNormalParam(psvbtn,"",0,255,0);
	ButtonA->SetOverParam(psvbto,"",128,255,128);
	ButtonA->SetPushedParam(psvbtp,"",255,255,0);

	ButtonA->TextPos.x=0.0f;
	ButtonA->TextPos.y=0.0f;
	ButtonA->PushedOffset.x=1.0f;
	ButtonA->PushedOffset.y=1.0f;

	cTexture* psvbbn=TextureCreateFromFilePNG("data/ui/scroll/svbbn.png");
	cTexture* psvbbo=TextureCreateFromFilePNG("data/ui/scroll/svbbo.png");
	cTexture* psvbbp=TextureCreateFromFilePNG("data/ui/scroll/svbbp.png");

	ButtonB=(cUiButton*)Ui.UiCore.CreateUiWindow(UI_BUTTON,"ButtonB",win_reg(2.0f,reg.h-(psvbbn->surf.h),psvbbn->surf.w*2.0f,psvbbn->surf.h*2.0f),this);

	ButtonB->FontId=1;
	ButtonB->SetNormalParam(psvbbn,"",0,255,0);
	ButtonB->SetOverParam(psvbbo,"",128,255,128);
	ButtonB->SetPushedParam(psvbbp,"",255,255,0);
	ButtonB->TextPos.x=0.0f;
	ButtonB->TextPos.y=0.0f;
	ButtonB->PushedOffset.x=1.0f;
	ButtonB->PushedOffset.y=1.0f;

	cTexture* psvcn=TextureCreateFromFilePNG("data/ui/scroll/svcn.png");
	cTexture* psvco=TextureCreateFromFilePNG("data/ui/scroll/svco.png");
	cTexture* psvcp=TextureCreateFromFilePNG("data/ui/scroll/svcp.png");

	ButtonControl=(cUiButton*)Ui.UiCore.CreateUiWindow(UI_BUTTON,"ButtonControl",win_reg(0.0f,ButtonA->reg.y+ButtonA->reg.h+1.0f,psvcn->surf.w*1.7f,psvcn->surf.h*1.7f),this);

	ButtonControl->FontId=1;
	ButtonControl->focus_keeper_flag=true;
	ButtonControl->SetNormalParam(psvcn,"",0,255,0);
	ButtonControl->SetOverParam(psvco,"",128,255,128);
	ButtonControl->SetPushedParam(psvcp,"",255,255,0);
	ButtonControl->TextPos.x=0.0f;
	ButtonControl->TextPos.y=0.0f;
	ButtonControl->PushedOffset.x=1.0f;
	ButtonControl->PushedOffset.y=1.0f;
		
	ButtonA->pUiBase_OnCursorDown_Callback=cUiScroll_CursorDown_ScrollUp;
	ButtonB->pUiBase_OnCursorDown_Callback=cUiScroll_CursorDown_ScrollDown;
	ButtonControl->pUiBase_OnCursorDown_Callback=cUiScroll_CursorDown_ButtonControl;
	//ButtonControl->pUiBase_OnCursorUp_Callback=cUiScroll_CursorUp_ButtonControl;
	//ButtonControl->pUiBase_OnCursorOut_Callback=cUiScroll_CursorOut_ButtonControl;
	//ButtonControl->pUiBase_OnCursorMove_Callback=cUiScroll_CursorMove_ButtonControl;
}