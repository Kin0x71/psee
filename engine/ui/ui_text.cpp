#include "ui_text.h"
#include "../shaders/ShaderSemantics.h"

extern uint IVertexesResizableTexSprite;
extern uint IIndexesResizableTexSprite;
extern cTextBase TextBase;

cUiText::cUiText(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiTextBase(winname,x,y,w,h,parent)
{
//	printf("cUiText (%s) %x",winname,this);
	UiWindowClass=UI_TEXT;
	
//	this->base_constructor(winname,x,y,w,h,parent);

	FontId=0;
	
	buff=0;
	text_len=0;
		
	Create(int(w),int(h));
}

cUiText::~cUiText()
{
	//printf("~cUiText() (%s) %x pTexture:%x",name,this,pTexture);
	/*if(pTexture){
		delete pTexture;
		pTexture=0;
	}*/
}

void cUiText::SetFont(int fid)
{
	FontId=fid;
}

void cUiText::AllockBuff(int size)
{
	buff=new char[size];
}

void cUiText::SetText(char* str,uchar r,uchar g,uchar b)
{
	int len=strlen(str);
	
	if(!len)return;
	
	if(text_len<len)
	{
		text_len=len;
		if(buff){
			delete[] buff;
		}
		
		buff=new char[text_len+1];
	}
	
	memcpy(buff,str,text_len);
	buff[text_len]=0;
	
	SetBuffer(buff,text_len,0,0,FontId,r,g,b);
	UpdateTexture();
}

void cUiText::SetTextColor(uchar r,uchar g,uchar b)
{
	SetBuffer(buff,text_len,0,0,FontId,r,g,b);
	UpdateTexture();
}

void cUiText::SetFormatText(uchar r,uchar g,uchar b,char* format,...)
{
	va_list marker;
	va_start( marker, format );

	TextBase.ClearTexturePixelBufferColorMask(FontId,pTexture,0,0,buff,text_len);
	
	vsprintf(buff,format, marker);
	va_end( marker );

	text_len=strlen(buff);
	TextBase.PrintToTexturePixelBufferColorMask(FontId,pTexture,0,0,buff,text_len,r,g,b,false);
	
	UpdateTexture();
}
/*
void cUiText::Draw(vec2& pos)
{
	vec4 v4rect=vec4(pos.x,pos.y,reg.w+pos.x,reg.h+pos.y);
	vec4 v4tex=vec4(tex.x,tex.h,tex.w,tex.y);
	
	_shader_select_values_list *cursel_meshval=pIShaderTech->select_mesh_values;
	
	while(cursel_meshval->next)
	{
		c_shader_value* pval=cursel_meshval->pval;
		if(pval)
		{
			switch(pval->Semantic)
			{
			case VALSEM_SAMPLER0:
				if(ppBackgroundTexture && *ppBackgroundTexture){
					pshader->SetTexture(pval->GId,*ppBackgroundTexture);
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
				printf("cUiText switch default: valuesL_mesh:%d %s\n",pval->Semantic,pval->Name);
			}
		}
		cursel_meshval=cursel_meshval->next;
	}

	pshader->SetVertexBuffer(IVertexesResizableTexSprite);
	pshader->SetIndexBuffer(IIndexesResizableTexSprite);

	pshader->Begin(pIShaderTech->TechID,4,6,_VERTTYPE_D);
}
*/