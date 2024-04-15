#include "ui_text_base.h"
#include "../shaders/ShaderSemantics.h"
#include "../../headers/gl_head.h"

extern c_shader *G_SHADER;
extern cScreen* pScreen;
extern cCamera Camera;

extern cTextBase TextBase;
extern cTextFly TextFly;

cUiTextBase::cUiTextBase()
{
		
}
	
cUiTextBase::cUiTextBase(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiBase(winname,x,y,w,h,parent)
{
//	printf("cUiTextBase (%s) %x",winname,this);
	
//	this->base_constructor(winname,x,y,w,h,parent);
		
	FontId=0;
	pTexture=0;
		
	buff=0;
	text_len=0;
		
	Create(int(w),int(h));
}

cUiTextBase::~cUiTextBase()
{
//	printf("~cUiTextBase() (%s) %x pTexture:%x",name,this,pTexture);
	if(pTexture){
		//printf("delete pTexture");
		delete pTexture;
		//printf("delete pTexture ok");
		pTexture=0;
	}
	
	if(buff){
		delete[] buff;
		buff=0;
	}
	//printf("~cUiTextBase() end");
}
	
void cUiTextBase::SetBuffer(char* str,int len,int x,int y,uint FontId,uchar r,uchar g,uchar b)
{
	TextBase.PrintToTexturePixelBufferColorMask(FontId,pTexture,x,y,str,len,r,g,b);
}

void cUiTextBase::UpdateTexture()
{
	glBindTexture(GL_TEXTURE_2D,pTexture->textureID);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,pTexture->surf.w,pTexture->surf.h,0,GL_RGBA,GL_UNSIGNED_BYTE,pTexture->surf.buff);

	glBindTexture(GL_TEXTURE_2D,0);
}

bool cUiTextBase::Create(int w,int h)
{
	pTexture=new cTexture();

	pTexture->surf.w=w;
	pTexture->surf.h=h;
#ifndef ANDROID_SPECE
	pTexture->WrapW=GL_CLAMP_TO_EDGE;
	pTexture->WrapH=GL_CLAMP_TO_EDGE;
#else
	ppTexture->WrapW=GL_CLAMP;
	ppTexture->WrapH=GL_CLAMP;
#endif
	pTexture->surf.pixel_size=4;
	pTexture->surf.buff=new uchar[(pTexture->surf.w*pTexture->surf.h)*pTexture->surf.pixel_size];
	memset(pTexture->surf.buff,0x0,(pTexture->surf.w*pTexture->surf.h)*pTexture->surf.pixel_size);
	
	glGenTextures( 1, &pTexture->textureID );
	glBindTexture( GL_TEXTURE_2D, pTexture->textureID );
	//GL_NEAREST GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->surf.w, pTexture->surf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->surf.buff );

	glBindTexture( GL_TEXTURE_2D, 0 );
	return true;
}

void cUiTextBase::OnUpdate()
{
	
}

void cUiTextBase::Draw(vec2& pos)
{
	vec4 v4rect=vec4(pos.x,pos.y,reg.w+pos.x,reg.h+pos.y);
	vec4 v4tex=vec4(tex.x,tex.y,tex.w,tex.h);

	vec2 TexSize(float(pTexture->surf.w),float(pTexture->surf.h));
		
	_shader_select_values_list *cursel_meshval=pIShaderTech->select_mesh_values;
	
	while(cursel_meshval->next)
	{
		c_shader_value* pval=cursel_meshval->pval;
		if(pval)
		{
			switch(pval->Semantic)
			{
			case VALSEM_SAMPLER0:
				if(pTexture)pshader->SetTexture(pval->GId,pTexture);
				break;
			case VALSEM_VECTOR2_A:
				pshader->SetVector(pval->GId,&TexSize);
				break;
			case VALSEM_MESH_UIREC_VEC4:
				pshader->SetVector(pval->GId,(vec4*)&v4rect);
				break;
			case VALSEM_MESH_TEX_VEC4:
				pshader->SetVector(pval->GId,(vec4*)&v4tex);
				break;
			default:
				printf("switch default: valuesL_mesh:%d %s\n",pval->Semantic,pval->Name);
			}
		}
		cursel_meshval=cursel_meshval->next;
	}
	
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	
	pshader->SetVertexBuffer(TextBase.IVertexesQuad);
	pshader->SetIndexBuffer(TextBase.IIndexesQuad);
	
	pshader->Begin(pIShaderTech->TechID,4,6,_VERTTYPE_C);
}