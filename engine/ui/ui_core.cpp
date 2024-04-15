#include "ui_core.h"
#include <engine/input/input.h>
/*
extern c_shader *G_SHADER;
extern _VERTEX_DECLARATION* pVERTDECL[];
extern cScreen* pScreen;
extern cTextBase TextBase;
extern cTextFly TextFly;
*/
extern uint OPT_SCREEN_W;
extern uint OPT_SCREEN_H;

uint IVertexesTexSprite=0;
uint IIndexesTexSprite=0;
uint IVertexesResizableTexSprite=0;
uint IIndexesResizableTexSprite=0;
uint IVertexesBorderSprite=0;
uint IIndexesBorderSprite=0;
/*
uint cUiCore::CreateTextureJpg(char* fname_c,char* fname_a,int wrap_w,int wrap_h)
{
	//TexturesControl.TextureCreateFromFileJPG(fname_c,fname_a,GL_REPEAT,GL_REPEAT);
}*/
/*
void cUiCore::PrintToScreen(uint FontId,uint Xofs,uint Yofs,char* format,...)
{
	char buff[256];
	va_list marker;
	va_start( marker, format );
	uint len=vsprintf(buff,format, marker);
	va_end( marker );
	
	TextBase.PrintToTexturePixelBufferColorMask(FontId,ScreenPlane.pScreenTexture,Xofs,Yofs,buff,len,0,255,0);
}*/

void cUiCore::_ui_event_stack_Push(cUiBase* win,float x,float y)
{
	_ui_event_stack_* cur=EventSteck;
	while(cur->next)
	{
		cur=cur->next;
	}
	
	cur->win=win;
	cur->cursor=vec2(x,y);
	cur->next=new _ui_event_stack_();
	cur->next->back=cur;
	EventSteckLast=cur;
}

void cUiCore::Init()
{
	//if((pIShaderTechBase= pUiShadersContainer->GetTechniqueByName(G_SHADER[UI_SHADER].GetTechniqueByName("tech_window_base")))==0){printf("tech shader not found\n");}
	//if((pIShaderTechText=pIShader->GetTech(G_SHADER[UI_SHADER].GetTechniqueByName("tech_window_text")))==0){printf("tech shader not found\n");}
	//if((pIShaderTechButton=pIShader->GetTech(G_SHADER[UI_SHADER].GetTechniqueByName("tech_window_button")))==0){printf("tech shader not found\n");}
	//if((pIShaderTechSizingTexture=pIShader->GetTech(G_SHADER[UI_SHADER].GetTechniqueByName("tech_sizing_texture")))==0){printf("tech shader not found\n");}
	
	//pshader=&G_SHADER[pIShader->ShaderID];

	UiWindowRoot=new cUiBase("UiWindowRoot",0.0f,0.0f,float(OPT_SCREEN_W),float(OPT_SCREEN_H));
	
	_VERTEXC VertexesTexSprite[]={
		{vec3(-0.5f,-0.5f,0.0f),vec2(0.0f,0.0f)},
		{vec3(-0.5f,0.5f,0.0f),vec2(0.0f,1.0f)},
		{vec3(0.5f,0.5f,0.0f),vec2(1.0f,1.0f)},
		{vec3(0.5f,-0.5f,0.0f),vec2(1.0f,0.0f)}
	};
	
	glGenBuffers(1, &IVertexesTexSprite);
	glBindBuffer(GL_ARRAY_BUFFER, IVertexesTexSprite);
	glBufferData(GL_ARRAY_BUFFER, 4*pVERTDECL[_VERTTYPE_C]->Size, VertexesTexSprite, GL_STATIC_DRAW);

	word IndexesTexSprite[]={2,1,0,2,0,3};

	glGenBuffers(1, &IIndexesTexSprite);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IIndexesTexSprite);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(word), IndexesTexSprite, GL_STATIC_DRAW);

	_VERTEXD VertexesRTS[]={
		{vec3(-0.5f,-0.5f,0.0f)},
		{vec3(-0.5f,0.5f,0.0f)},
		{vec3(0.5f,0.5f,0.0f)},
		{vec3(0.5f,-0.5f,0.0f)}
	};

	glGenBuffers(1, &IVertexesResizableTexSprite);
	//checkOpenGLerror("Particle Vertexes A");
	glBindBuffer(GL_ARRAY_BUFFER, IVertexesResizableTexSprite);
	//checkOpenGLerror("Particle Vertexes B");
	glBufferData(GL_ARRAY_BUFFER, 4*pVERTDECL[_VERTTYPE_D]->Size, VertexesRTS, GL_STATIC_DRAW);
	//checkOpenGLerror("Particle Vertexes C");

	word IndexesRTS[]={2,1,0,2,0,3};

	glGenBuffers(1, &IIndexesResizableTexSprite);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IIndexesResizableTexSprite);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(word), IndexesRTS, GL_STATIC_DRAW);

	_VERTEXE Vertexes[]={
		{0.000000f},
		{1.000000f},
		{2.000000f},
		{3.000000f},
		{4.000000f},
		{5.000000f},
		{6.000000f},
		{7.000000f},
		{8.000000f},
		{9.000000f},
		{10.000000f},
		{11.000000f},
		{12.000000f},
		{13.000000f},
		{14.000000f},
		{15.000000f}
	};

	glGenBuffers(1, &IVertexesBorderSprite);
//	checkOpenGLerror("Particle Vertexes A");
	glBindBuffer(GL_ARRAY_BUFFER, IVertexesBorderSprite);
//	checkOpenGLerror("Particle Vertexes B");
	glBufferData(GL_ARRAY_BUFFER, 16*pVERTDECL[_VERTTYPE_E]->Size, Vertexes, GL_STATIC_DRAW);
//	checkOpenGLerror("Particle Vertexes C");

	word Indexes[]={
		0 ,1 ,3 ,1 ,2 ,3 ,
		4 ,5 ,7 ,5 ,6 ,7 ,
		8 ,9 ,11,9 ,10,11,
		12,13,15,13,14,15,
	};

	glGenBuffers(1, &IIndexesBorderSprite);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IIndexesBorderSprite);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*sizeof(word), Indexes, GL_STATIC_DRAW);

	ProjMx=MakeOrthogonal(
		0.0f,
		float(OPT_SCREEN_W),
		0.0f,
		float(OPT_SCREEN_H),
		0.0f,
		1.0f
		);
	//ProjMx.Identity();
	
	//ProjMx = MakePerspective(40.0f, float(pScreen->width)/float(pScreen->height), 0.1f, 50.0f );
}

void cUiCore::Free()
{
	glDeleteBuffers(1, &IVertexesResizableTexSprite);
	glDeleteBuffers(1, &IIndexesResizableTexSprite);
	glDeleteBuffers(1, &IVertexesBorderSprite);
	glDeleteBuffers(1, &IIndexesBorderSprite);
	
	if(pIShaders){
		delete pIShaders;
		pIShaders=0;
	}
	
	if(UiWindowRoot){
		delete UiWindowRoot;
		UiWindowRoot=0;
	}
}

cUiBase* cUiCore::CreateUiWindow(UI_WINDOW_CLASS win_class,char* name,const win_reg& rect,cUiBase* parent,bool visible)
{
	cUiBase* win=0;
	
	switch(win_class)
	{
		case UI_BASE:
			win=new cUiBase(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechBase;
			((cUiBase*)win)->OnCreate();
			break;
		case UI_BUTTON:
			win=(cUiBase*)new cUiButton(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechButton;
			((cUiButton*)win)->OnCreate();
			break;
		case UI_TEXT:
			win=(cUiBase*)new cUiText(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechText;
			((cUiText*)win)->OnCreate();
			break;
		case UI_TEXT_EDIT:
			win=(cUiBase*)new cUiTextEdit(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechText;
			((cUiTextEdit*)win)->OnCreate();
			break;
		case UI_SCR_FLOAT_TEXT:
			win=(cUiBase*)new cUiScrFloatText(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechText;
			((cUiScrFloatText*)win)->OnCreate();
			break;
		case UI_MOVING_WRAP:
			win=(cUiBase*)new cUiMovingWrap(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechSizingTexture;
			((cUiMovingWrap*)win)->OnCreate();
			break;
		case UI_SCROLL:
			win=(cUiBase*)new cUiScroll(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechBase;
			((cUiScroll*)win)->OnCreate();
			break;
		case UI_CONSOLE:
			win=(cUiConsole*)new cUiConsole(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechBase;
			((cUiConsole*)win)->OnCreate();
			break;
		default:
			win=new cUiBase(name,rect.x,rect.y,rect.w,rect.h);
			win->pshader=pshader;
			win->pIShaderTech=pIShaderTechBase;
			((cUiBase*)win)->OnCreate();
	}

	if(!parent)
	{
		if(!visible)return win;
		
		win->parent=UiWindowRoot;

		if(!UiWindowRoot->child){
			UiWindowRoot->child=win;
		}else{
			cUiBase* cur=UiWindowRoot->child;
			while(cur->sibling)cur=cur->sibling;

			cur->sibling=win;
		}

		return win;
	}
	
	win->parent=parent;
	
	if(!parent->child)
	{
		parent->child=win;
	}else{
		cUiBase* cur=parent->child;
		
		while(cur->sibling)cur=cur->sibling;

		cur->sibling=win;
	}
	
	return win;
}

void cUiCore::ShowWindow(cUiBase* win)
{
	win->parent=UiWindowRoot;

	if(!UiWindowRoot->child){
		UiWindowRoot->child=win;
	}else{
		cUiBase* cur=UiWindowRoot->child;
		while(cur->sibling)cur=cur->sibling;

		cur->sibling=win;
	}
}

void cUiCore::UnlinkWindow(cUiBase* win)
{
	if(UiWindowRoot->child==win)
	{
		UiWindowRoot->child=UiWindowRoot->child->sibling;
		win->sibling=0;
		return;
	}

	cUiBase* cur=UiWindowRoot->child;
	cUiBase* last=0;

	do
	{		
		if(cur==win)
		{
			if(last)
			{
				last->sibling=cur->sibling;
			}
		}
		last=cur;
		cur=cur->sibling;
	}while(cur);

	/*
	if(win->parent && win->parent->child){
		if(win->parent->child==win){
			win->parent->child=win->sibling;
		}else{
			cUiBase* cur=win->parent->child->sibling;
			cUiBase* last=0;
			while(cur->sibling){
				if(cur==win){
					if(last)
					{
						last->sibling=cur->sibling;
					}
					break;
				}
				last=cur;
				cur=cur->sibling;
			}
		}
	}*/
	
	win->sibling=0;
	//win->parent=0;
}

void cUiCore::UpTopWindow(cUiBase* win)
{
	cUiBase* cur=UiWindowRoot->child;
	cUiBase* last=0;

	while(cur->sibling)
	{
		if(cur==win)
		{
			if(last)
			{
				last->sibling=cur->sibling;
			}else{
				UiWindowRoot->child=cur->sibling;
			}
		}
		last=cur;
		cur=cur->sibling;
	}
	
	cur->sibling=win;
	win->sibling=0;
}

void cUiCore::uibase_update(cUiBase* win)
{
	win->OnUpdate();

	if(win->child)uibase_update(win->child);
	if(win->sibling)uibase_update(win->sibling);
}

void cUiCore::uibase_redraw(cUiBase* win,vec2* parent_pos)
{
	if(!win->name){
		printf("%x:%d",win,win->UiWindowClass);
	}
	//printf(" %x",win->pIShaderTech);
	//printf("  %x",win->child);
	//if(win->child)printf("  [%s]",win->child->name);
	//printf("  %x",win->sibling);
	//if(win->sibling)printf("  [%s]",win->sibling->name);
	vec2 pos=vec2(win->reg.x,win->reg.y);

	if(parent_pos){
		pos+=(*parent_pos);
	}

	//win->OnUpdate();
	win->Draw(pos);

	if(win->child){
		if(!win->child->name)printf("c:%s",win->name);
		uibase_redraw(win->child,&pos);
	}
	if(win->sibling){
		if(!win->sibling->name)printf("s:%s",win->name);
		uibase_redraw(win->sibling,parent_pos);
	}
}

void cUiCore::UpdateWindows()
{
	glDisable(GL_DEPTH_TEST);
//	glDepthMask(GL_FALSE);
	//glDepthFunc(GL_ALWAYS);
	//glClearDepth(0.0f);
	if(UiWindowRoot->child){
		//printf("cUiCore::UpdateWindows()");
		//printf("%x\n",UiWindowRoot->child);
		uibase_update(UiWindowRoot->child);
	}

//	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	
	//glDepthFunc(GL_LEQUAL);
	//glClearDepth(1.0f);
}

void cUiCore::DrawWindows()
{
	glDisable(GL_DEPTH_TEST);
//	glDepthMask(GL_FALSE);
	//glDepthFunc(GL_ALWAYS);
	//glClearDepth(0.0f);
	
	if(UiWindowRoot->child){
		//printf("cUiCore::UpdateWindows()");
		//printf("%x\n",UiWindowRoot->child);
		uibase_redraw(UiWindowRoot->child);
	}

//	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	
	//glDepthFunc(GL_LEQUAL);
	//glClearDepth(1.0f);
}

bool cUiCore::uibase_cursordown_event(cUiBase* win,vec2* cursor_pos,vec2* parent_pos)
{
	//printf("%s",win->name);
	//printf("(%f,%f)(%f %f %f %f)",mouse_pos->x,mouse_pos->y,win->Rec.x,win->Rec.y,(win->Rec.x+win->Rec.w),(win->Rec.y+win->Rec.h));
	//printf("%s (%f,%f)<-",win->name,mouse_pos->x,mouse_pos->y);
	vec2 pos=vec2(win->reg.x,win->reg.y);

	if(parent_pos){
		pos+=(*parent_pos);
	}
	
	if(cursor_pos->x>=pos.x && cursor_pos->y>=pos.y && cursor_pos->x<=(pos.x+win->reg.w) && cursor_pos->y<=(pos.y+win->reg.h))
	{
#ifndef ANDROID_SPACE
		if(win->cursor_capture){
			//printf("CursorDown %s:%x %x",win->name,win,pUiBase_Event_Post_Caller);
			_ui_event_stack_Push(win,cursor_pos->x-pos.x,cursor_pos->y-pos.y);
		}
#else
		//printf("_ui_event_stack_Push:%s",win->name);
		_ui_event_stack_Push(win,cursor_pos->x-pos.x,cursor_pos->y-pos.y);
#endif
		if(win->child)uibase_cursordown_event(win->child,cursor_pos,&pos);
		//printf("TRUE %s\n",win->name);
		//return true;
	}
	
	if(win->sibling)uibase_cursordown_event(win->sibling,cursor_pos,parent_pos);

	return false;
}

bool cUiCore::UiBaseCursorDownEvent(int x,int y)
{
//	printf("UiBaseCursorDownEvent");
	bool ret=false;
	FocusedWindow=0;
	
	if(UiWindowRoot->child)
	{
		pUiBase_Event_Post_Caller=0;
		pUiBase_OnCursorDown_PostCallback=0;
		
		vec2 vcursor=vec2(float(x),float(y));
		ret=uibase_cursordown_event(UiWindowRoot->child,&vcursor);

		if(EventSteckLast)
		{
			_ui_event_stack_* cur=EventSteckLast;
			do
			{
				//printf("OnCursorDown:%x %s fkf:%d",cur->win,cur->win->name,cur->win->focus_keeper_flag);
				cur->win->OnCursorDown(cur->cursor.x,cur->cursor.y);

				if(!FocusedWindow && cur->win->focus_keeper_flag)
				{
					if(!cur->win->focus_keep){
						cur->win->OnFocusKeep();
						cur->win->focus_keep=true;
					}
					FocusedWindow=cur->win;
				}

				cur->win->cursor_capture=true;
				
				if(cur->win->pUiBase_OnCursorDown_Callback)cur->win->pUiBase_OnCursorDown_Callback(cur->win,int(cur->cursor.x),int(cur->cursor.y));
				
				//if(!cur->win->overlapper_flag)break;
				cur=cur->back;
			}while(cur);
			
			EventSteckLast=0;
			delete EventSteck;
			EventSteck=new _ui_event_stack_();
		}
	}

	if(UiWindowRoot->child){
		uibase_free_focus(UiWindowRoot->child,FocusedWindow);
	}

	return ret;
}

bool cUiCore::uibase_cursorup_event(cUiBase* win,vec2* corsor_pos,vec2* parent_pos)
{
	//printf("%s\n",win->name);
	//printf("(%f,%f)(%f %f %f %f)",mouse_pos->x,mouse_pos->y,win->Rec.x,win->Rec.y,(win->Rec.x+win->Rec.w),(win->Rec.y+win->Rec.h));
	//printf("%s (%f,%f)<-",win->name,mouse_pos->x,mouse_pos->y);
	vec2 pos=vec2(win->reg.x,win->reg.y);

	if(parent_pos){
		pos+=(*parent_pos);
	}
	
	if(corsor_pos->x>=pos.x && corsor_pos->y>=pos.y && corsor_pos->x<=(pos.x+win->reg.w) && corsor_pos->y<=(pos.y+win->reg.h))
	{
		if(win->cursor_capture){
			_ui_event_stack_Push(win,corsor_pos->x-pos.x,corsor_pos->y-pos.y);
		}
		
		if(win->child)uibase_cursorup_event(win->child,corsor_pos,&pos);
		//printf("TRUE %s\n",win->name);
		//return true;
	}
	
	if(win->sibling)uibase_cursorup_event(win->sibling,corsor_pos,parent_pos);

	return false;
}

bool cUiCore::UiBaseCursorUpEvent(int x,int y)
{
	bool ret=false;

	if(UiWindowRoot->child){

		pUiBase_Event_Post_Caller=0;
		pUiBase_OnCursorUp_PostCallback=0;

		vec2 vcursor=vec2(float(x),float(y));
		ret=uibase_cursorup_event(UiWindowRoot->child,&vcursor);

		if(EventSteckLast)
		{
			_ui_event_stack_* cur=EventSteckLast;
			do
			{
				//printf("%s\n",cur->win->name);
				cur->win->OnCursorUp(cur->cursor.x,cur->cursor.y);
				cur->win->cursor_capture=true;
				
				if(cur->win->pUiBase_OnCursorUp_Callback)cur->win->pUiBase_OnCursorUp_Callback(cur->win,int(cur->cursor.x),int(cur->cursor.y));
				
//				if(cur->win->overlapper_flag)break;
				cur=cur->back;
			}while(cur);
			
			EventSteckLast=0;
			delete EventSteck;
			EventSteck=new _ui_event_stack_();
		}
	}

	return ret;
}

void cUiCore::uibase_cursormove_event(cUiBase* win,vec2* cursor_pos,vec2* parent_pos)
{
	//printf("uibase_cursormove_event:%s",win->name);
	//printf("(%f,%f)(%f %f %f %f)",mouse_pos->x,mouse_pos->y,win->Rec.x,win->Rec.y,(win->Rec.x+win->Rec.w),(win->Rec.y+win->Rec.h));
	//printf("%s (%f,%f)<-",win->name,mouse_pos->x,mouse_pos->y);
	vec2 pos=vec2(win->reg.x,win->reg.y);

	if(parent_pos){
		pos+=(*parent_pos);
	}
	//if(!strcmp(win->name,"ButtonControl"))printf("out -> %s",win->name);
//if(!strcmp(win->name,"EditorButtonA"))printf("(%f,%f)<-",mouse_pos->y,pos.y+win->reg.h);
	
	//if(win->overlapper_flag)
	{
		if(cursor_pos->x>=pos.x && cursor_pos->y>=pos.y && cursor_pos->x<=(pos.x+win->reg.w) && cursor_pos->y<=(pos.y+win->reg.h))
		{
	//printf("set %s %f>=%f && %f>=%f && %f<=%f && %f<=%f",win->name,cursor_pos->x,pos.x,cursor_pos->y,pos.y,cursor_pos->x,(pos.x+win->reg.w),cursor_pos->y,(pos.y+win->reg.h));
		//	printf("uibase_cursormove_event:%s",win->name);
			_ui_event_stack_Push(win,cursor_pos->x-pos.x,cursor_pos->y-pos.y);
	//exit(0);

			CursorAboveUi=true;
			CursorAboveWindow=win;

			//if(win->child)uibase_mousemove_event(win->child,mouse_pos,&pos);

		}else{
			//printf("out -> %s %d",win->name,win->mouse_capture);
			//printf("unset %s",win->name);
			if(win->cursor_capture){
				//printf("out -> %s",win->name);
				win->OnCursorOut();
				
				if(win->pUiBase_OnCursorOut_Callback)win->pUiBase_OnCursorOut_Callback(win);
				//if(win->child)uibase_reset_mouse(win->child);
			}
			win->cursor_capture=false;
		}
	}
	
	if(win->child)uibase_cursormove_event(win->child,cursor_pos,&pos);
	if(win->sibling)uibase_cursormove_event(win->sibling,cursor_pos,parent_pos);
}

void cUiCore::uibase_reset_cursor(cUiBase* win)
{
	win->cursor_capture=false;
	
	if(win->child)  uibase_reset_cursor(win->child);
	if(win->sibling)uibase_reset_cursor(win->sibling);
}

bool cUiCore::UiBaseCursorMoveEvent(int x,int y)
{
	if(UiWindowRoot->child)
	{//printf("%d %d",x,y);
		pUiBase_Event_Post_Caller=0;
		pUiBase_OnCursorMove_PostCallback=0;

		vec2 vcursor=vec2(float(x),float(y));
		
		bool last_cursor_above_ui=CursorAboveUi;
		CursorAboveUi=false;
		CursorAboveWindow=0;
		
		uibase_cursormove_event(UiWindowRoot->child,&vcursor);
		
		if(EventSteckLast)
		{
			_ui_event_stack_* cur=EventSteckLast;
			do
			{
				cur->win->OnCursorMove(cur->cursor.x,cur->cursor.y);
				cur->win->cursor_capture=true;
				
				if(cur->win->pUiBase_OnCursorMove_Callback)cur->win->pUiBase_OnCursorMove_Callback(cur->win,int(cur->cursor.x),int(cur->cursor.y));
				
				//if(cur->win->overlapper_flag)break;
				cur=cur->back;
			}while(cur);
			
			EventSteckLast=0;
			delete EventSteck;
			EventSteck=new _ui_event_stack_();
		}
		
		if(last_cursor_above_ui && !CursorAboveUi){
			uibase_reset_cursor(UiWindowRoot->child);
		}
		
		//printf("CursorAboveUi:%d",CursorAboveUi);
		
		if(pUiBase_OnCursorMove_PostCallback)
		{
			pUiBase_OnCursorMove_PostCallback(pUiBase_Event_Post_Caller,x,y);
		}
		
		LastCursorPos=vec2(float(x),float(y));
	}

	return 0;
}

void cUiCore::UiBaseKeyDownEvent(_KEY_KODE_ key)
{
	if(FocusedWindow)
	{
		FocusedWindow->OnKeyDown(key);
	}
}

void cUiCore::uibase_free_focus(cUiBase* win,cUiBase* exception_win)
{
	if(win!=exception_win && win->focus_keeper_flag && win->focus_keep)
	{
		win->OnFocusLost();
		win->focus_keep=false;
	}

	if(win->child)  uibase_free_focus(win->child,exception_win);
	if(win->sibling)uibase_free_focus(win->sibling,exception_win);
}