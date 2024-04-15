#include "ui_button.h"
#include "ui.h"
#include "../shaders/ShaderSemantics.h"

extern c_shader *G_SHADER;
extern cScreen* pScreen;
extern uint IVertexesTexSprite;
extern uint IIndexesTexSprite;
extern cCamera Camera;
extern cUi Ui;

cUiButton::cUiButton(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiBase(winname,x,y,w,h,parent)
{
	UiWindowClass=UI_BUTTON;
	//this->base_constructor(winname,x,y,w,h,parent);
	FontId=0;
	pCurrentTexture=0;
	pNormalTexture=0;
	pOverTexture=0;
	pPushedTexture=0;
	Caption=0;
	TextNormal=0;
	TextOver=0;
	TextPushed=0;
//	overlapper_flag=true;
	//pUiBase_OnCursorDown_Callback=0;
}

void cUiButton::OnUpdate()
{
	if(!cursor_capture)
	{
		pCurrentTexture=pNormalTexture;
		
		if(Caption)
		{
			Caption->reg.x=TextPos.x;
			Caption->reg.y=TextPos.y;
		}
	}
}

void cUiButton::OnCursorMove(float x,float y)
{
	cursorx=x;
	cursory=y;
	
	if(!cursor_capture)
	{
		OnCursorOver();
	}
}

void cUiButton::OnCursorOver()
{//printf("over b %s\n",name);
	pCurrentTexture=pOverTexture;
	
	if(Caption)
	{
		Caption->SetText(TextOver,ColorOver.x,ColorOver.y,ColorOver.z);
	}
}

void cUiButton::OnCursorOut()
{//printf("out b %s\n",name);
	pCurrentTexture=pNormalTexture;
	
	if(Caption)
	{
		Caption->SetText(TextNormal,ColorNormal.x,ColorNormal.y,ColorNormal.z);
	}
	
	if(pUiBase_OnCursorOut_Callback)
	{
		Ui.UiCore.pUiBase_Event_Post_Caller=this;
		Ui.UiCore.pUiBase_OnCursorOut_PostCallback=pUiBase_OnCursorOut_Callback;
	}
}

void cUiButton::OnCursorDown(float x,float y)
{
	//printf("cUiButton::OnCursorDown:%x %s",this,name);
	pCurrentTexture=pPushedTexture;
	
	if(Caption)
	{
		Caption->reg.x=TextPos.x+PushedOffset.x;
		Caption->reg.y=TextPos.y+PushedOffset.y;
		
		Caption->SetText(TextPushed,ColorPushed.x,ColorPushed.y,ColorPushed.z);
	}
	
	if(pUiBase_OnCursorDown_Callback)
	{
		Ui.UiCore.pUiBase_Event_Post_Caller=this;
		Ui.UiCore.pUiBase_OnCursorDown_PostCallback=pUiBase_OnCursorDown_Callback;
	}
}

void cUiButton::OnCursorUp(float x,float y)
{
	pCurrentTexture=pOverTexture;
	
	if(Caption)
	{
		Caption->reg.x=TextPos.x;
		Caption->reg.y=TextPos.y;
		
		Caption->SetText(TextNormal,ColorNormal.x,ColorNormal.y,ColorNormal.z);
	}
}

bool cUiButton::SetNormalParam(cTexture* pTexture,char* str,uchar r,uchar g,uchar b)
{
	pNormalTexture=pTexture;
	pCurrentTexture=pTexture;
	
	if(str && !Caption)
	{
		TextNormal=str;
		ColorNormal=POINTUI3(r,g,b);
		
		Caption=(cUiText*)Ui.UiCore.CreateUiWindow(UI_TEXT,"Caption",win_reg(TextPos.x,TextPos.y,reg.w,reg.h),this);
		//Caption->focus_keeper_flag=false;
		//Caption->overlapper_flag=false;
		Caption->FontId=FontId;
		Caption->SetText(TextNormal,ColorNormal.x,ColorNormal.y,ColorNormal.z);
	}
	return true;
}

bool cUiButton::SetOverParam(cTexture* pTexture,char* str,uchar r,uchar g,uchar b)
{
	pOverTexture=pTexture;
	
	ColorOver=POINTUI3(r,g,b);
	
	if(str)
	{
		TextOver=str;
	}else{
		TextOver=TextNormal;
	}
	
	return true;
}

bool cUiButton::SetPushedParam(cTexture* pTexture,char* str,uchar r,uchar g,uchar b)
{
	pPushedTexture=pTexture;
	
	ColorPushed=POINTUI3(r,g,b);
	
	if(str)
	{
		TextPushed=str;
	}else{
		TextPushed=TextNormal;
	}
	
	return true;
}

void cUiButton::Draw(vec2& pos)
{
	vec2 vout;
	ConvertToScreenCoord(pos.x,pos.y,&vout);
	
//	float x=vout.x;
//	float y=vout.y;

	ConvertToScreenCoord(pos.x+reg.w,pos.y+reg.h,&vout);

//	float xw=vout.x;
//	float yh=vout.y;
	
//	vec4 v4Rec=vec4(x,y,xw,yh);
//	vec4 v4rect=v4Rec;
	vec4 v4rect=vec4(pos.x,pos.y,reg.w+pos.x,reg.h+pos.y);
//	printf("ui_button: %f,%f,%f,%f",pos.x,pos.y,reg.w+pos.x,reg.h+pos.y);
	
	vec2 TexSize;
	if(pCurrentTexture){
		TexSize=vec2(float(pCurrentTexture->surf.w),float(pCurrentTexture->surf.w));
	}
		
	_shader_select_values_list *cursel_meshval=pIShaderTech->select_mesh_values;
	
	while(cursel_meshval->next)
	{
		c_shader_value* pval=cursel_meshval->pval;
		if(pval)
		{
			switch(pval->Semantic)
			{
			case VALSEM_SAMPLER0:
				if(pCurrentTexture)pshader->SetTexture(pval->GId,pCurrentTexture);
				break;
			case VALSEM_VECTOR2_A:
				pshader->SetVector(pval->GId,&TexSize);
				break;
			case VALSEM_MESH_UIREC_VEC4:
				pshader->SetVector(pval->GId,(vec4*)&v4rect);
				break;
			default:
				printf("ui_button: switch default: TechID:%d valuesL_mesh:%d %s",pIShaderTech->TechID,pval->Semantic,pval->Name);
			}
		}
		cursel_meshval=cursel_meshval->next;
	}
	
	pshader->SetVertexBuffer(IVertexesTexSprite);
	pshader->SetIndexBuffer(IIndexesTexSprite);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	pshader->Begin(pIShaderTech->TechID,4,6,_VERTTYPE_C);
}