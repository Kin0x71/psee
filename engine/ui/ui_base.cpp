
#include "ui_base.h"
#include "ui_button.h"
#include "ui_text.h"
#include "ui_text_edit.h"
#include "ui_scr_float_text.h"
#include "ui_moving_wrap.h"
#include "windows/ui_scroll.h"
#include <engine/shaders/ShaderSemantics.h>
#include "text.h"

extern c_shader *G_SHADER;
extern cScreen Screen;
extern cInput Input;

extern uint IVertexesResizableTexSprite;
extern uint IIndexesResizableTexSprite;
extern uint IVertexes4PSprite;
extern uint IIndexes4PSprite;

extern cCamera Camera;

extern cTexture* pTexUiet;

cUiBase::cUiBase()
{
	UiWindowClass=UI_BASE;
	base_constructor(0,0.0f,0.0f,0.0f,0.0f,0);
}

cUiBase::cUiBase(const char* winname,float x,float y,float w,float h,cUiBase* parent)
{
	UiWindowClass=UI_BASE;
	base_constructor(winname,x,y,w,h,parent);
}

cUiBase::~cUiBase()
{
//	printf("~cUiBase:%x:%s",this,name);
	
	if(pBackgroundTexture){
		delete pBackgroundTexture;
		pBackgroundTexture=0;
	}
	/*
	if(this->sibling)
	{
		UI_WINDOW_CLASS ui_window_class=this->sibling->UiWindowClass;
		switch(ui_window_class)
		{
			case UI_BASE:
				delete (cUiBase*)this->sibling;
				break;
			case UI_BUTTON:
				delete (cUiButton*)this->sibling;
				break;
			case UI_TEXT:
				delete (cUiText*)this->sibling;
				break;
			case UI_TEXT_EDIT:
				delete (cUiTextEdit*)this->sibling;
				break;
			case UI_SCR_FLOAT_TEXT:
				delete (cUiScrFloatText*)this->sibling;
				break;
			case UI_MOVING_WRAP:
				delete (cUiMovingWrap*)this->sibling;
				break;
			default:
			delete this->sibling;
		}
		this->sibling=0;
	}*/
	/*
	if(this->sibling)
	{
		cUiBase* cur=this->sibling;
		while(cur){
			delete cur;
			cur=cur->sibling;
		}
	}*/

	if(this->child)
	{
		UI_WINDOW_CLASS ui_window_class=this->child->UiWindowClass;

		switch(ui_window_class)
		{
			case UI_BASE:
				delete (cUiBase*)this->child;
				break;
			case UI_BUTTON:
				delete (cUiButton*)this->child;
				break;
			case UI_TEXT:
				delete (cUiText*)this->child;
				break;
			case UI_TEXT_EDIT:
				delete (cUiTextEdit*)this->child;
				break;
			case UI_MOVING_WRAP:
				delete (cUiMovingWrap*)this->child;
				break;
			default:
			delete this->child;
		}
		this->child=0;
	}
	
	base_destructor();
	
	if(name){
		delete[] name;
		name=0;
	}
}
	
void cUiBase::base_constructor(const char* winname,float x,float y,float w,float h,cUiBase* parent)
{
	child=0;
	sibling=0;
	this->parent=parent;
	
	if(winname)
	{
		int nlen=strlen(winname);
		name=new char[nlen+1];
		memcpy(name,winname,nlen);
		name[nlen]=0;
	}
	
	reg.x=x;
	reg.y=y;
	reg.w=w;
	reg.h=h;

	//tex=win_reg(0.0f,0.0f,1.0f,1.0f);
	tex.x=0.0f;
	tex.y=0.0f;
	tex.w=reg.w/100.0f;
	tex.h=reg.h/100.0f;

	pshader=0;
	pIShaderTech=0;
	pBackgroundTexture=0;
		
//	overlapper_flag=true
	focus_keeper_flag=false;

	cursorx=0.0f;
	cursory=0.0f;
	cursor_capture=false;
	focus_keep=false;

	pUiBase_OnCursorMove_Callback=0;
	pUiBase_OnCursorDown_Callback=0;
	pUiBase_OnCursorUp_Callback=0;
	pUiBase_OnCursorOut_Callback=0;
}
	
void cUiBase::base_destructor()
{
//	printf("base_destructor() (%s)",name);
//	printf(" this:%x",this);
//	printf(" parent:%x",parent);
	
	if(parent)
	{
//		printf(" (%s)",parent->name);
		//printf(" parent->child:%x==this:%x",parent->child,this);
		if(parent->child)
		{
			if(parent->child==this){
				parent->child=parent->child->sibling;
			}else if(parent->child->sibling){
//				printf(" pparent->child->sibling->sibling:%x",parent->child->sibling->sibling);
				//parent->child->sibling=parent->child->sibling->sibling;
				
				cUiBase* cur=parent->child;
				cUiBase* last=0;
				while(cur){
					if(cur==this){
						if(last){
//							printf("  [%s]",last->sibling->name);
							last->sibling=cur->sibling;
							break;
						}
					}
					last=cur;
					cur=cur->sibling;
				}
			}
		}
		parent=0;
	}
}

void cUiBase::ConvertToScreenCoord(float x, float y,vec2 *vret)
{
	float *projection=(float*)&Camera.ProjMx;
	int viewport[4]={0,Screen.height,Screen.width,-Screen.height};
	
	float m[16];
	float in[4], out[4];

	/*if(glhInvertMatrixf2(projection, m) == 0)
		return;
	
	in[0]=(x-(float)viewport[0])/(float)viewport[2]*2.0f-1.0f;
	in[1]=(y-(float)viewport[1])/(float)viewport[3]*2.0f-1.0f;
	in[2]=-1.0f;
	in[3]=1.0f;
	
	MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
	if(out[3]==0.0f)
		return;
	out[3]=1.0f/out[3];
	vret->x=out[0]*out[3];
	vret->y=out[1]*out[3];*/
}

void cUiBase::Draw(vec2& pos)
{
	vec4 v4rect=vec4(pos.x,pos.y,reg.w+pos.x,reg.h+pos.y);
	vec4 v4tex=vec4(tex.x,tex.h,tex.w,tex.y);
	
	_shader_select_values_list *cursel_meshval=pIShaderTech->select_mesh_values;
	
	while(cursel_meshval->next)
	{
		c_shader_value* pval=cursel_meshval->pval;
		if(pval)
		{//VALSEM_PROJMX
			switch(pval->Semantic)
			{
			case VALSEM_SAMPLER0:
				if(pBackgroundTexture){
					pshader->SetTexture(pval->GId,pBackgroundTexture);
				}else{
					pshader->SetTexture(pval->GId,0);
				}
				break;
			case VALSEM_MESH_UIREC_VEC4:
				pshader->SetVector(pval->GId,(vec4*)&v4rect);
				break;
			case VALSEM_MESH_TEX_VEC4:
				pshader->SetVector(pval->GId,(vec4*)&v4tex);
				break;
			default:
				printf("switch default: valuesL_mesh:%d %s",pval->Semantic,pval->Name);
			}
		}
		cursel_meshval=cursel_meshval->next;
	}

	pshader->SetVertexBuffer(IVertexesResizableTexSprite);
	pshader->SetIndexBuffer(IIndexesResizableTexSprite);

	pshader->Begin(pIShaderTech->TechID,4,6,_VERTTYPE_D);
}

bool cUiBase::SetBackground(cTexture* pTexture)
{
	pBackgroundTexture=pTexture;
	return true;
}

bool cUiBase::SetBackground(cTexture* pTexture,float tex_x,float tex_y,float tex_w,float tex_h)
{
	pBackgroundTexture=pTexture;

	tex.x=tex_x;
	tex.y=tex_y;
	tex.w=tex_w;
	tex.h=tex_h;
	return true;
}

cUiBase* cUiBase::get_sub_win_by_name_r(cUiBase* win,char* name)
{
	if(!strcmp(win->name,name)){
		return win;
	}
		
	if(win->child){
		return get_sub_win_by_name_r(win->child,name);
	}
	
	if(win->sibling){
		return get_sub_win_by_name_r(win->sibling,name);
	}
	
	return 0;
}

cUiBase* cUiBase::GetSubWinByName(char* name)
{
	return get_sub_win_by_name_r(child,name);
}

void cUiBase::OnCursorDown(float x,float y)
{/*
	printf("cUiBase::OnCursorDown:%x %s",this,name);
	printf("UiWindowClass:%d",UiWindowClass);
	switch(UiWindowClass)
	{
		case UI_BUTTON:
			printf("redirect");
			((cUiButton*)this)->OnCursorDown(x,y);
			break;
		case UI_TEXT:
			//((cUiText*)this)->OnCursorDown(x,y);
			break;
		case UI_TEXT_EDIT:
			((cUiTextEdit*)this)->OnCursorDown(x,y);
			break;
		case UI_SCR_FLOAT_TEXT:
			((cUiScrFloatText*)this)->OnCursorDown(x,y);
			break;
		case UI_MOVING_WRAP:
			((cUiMovingWrap*)this)->OnCursorDown(x,y);
			break;
		case UI_SCROLL:
			((cUiScroll*)this)->OnCursorDown(x,y);
			break;
	}*/
}
