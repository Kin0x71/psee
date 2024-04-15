#include "ui_moving_wrap.h"
#include "ui.h"
#include "../shaders/ShaderSemantics.h"
#include "../input.h"
#include "../../headers/gl_head.h"

//#include "../common.h"

extern c_shader *G_SHADER;
//extern cTexturesControl TexturesControl;
extern cScreen* pScreen;
extern uint IVertexes4PSprite;
extern uint IIndexes4PSprite;
extern uint IVertexesBorderSprite;
extern uint IIndexesBorderSprite;
extern cCamera Camera;
extern cInput Input;
extern cUi Ui;

extern cTexture* pTexUiet;

cUiMovingWrap::cUiMovingWrap(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiBase(winname,x,y,w,h,parent)
{
	UiWindowClass=UI_MOVING_WRAP;
//	this->base_constructor(winname,x,y,w,h,parent);
//	overlapper_flag=true;
	CursorDownPosX=0;
	CursorDownPosY=0;
	CursorCapture=false;
	BorderLeft=0.0f;
	BorderTop=0.0f;
	BorderRight=0.0f;
	BorderBottom=0.0f;
	Init();
}

void cUiMovingWrap::Init()
{
	pBackgroundTexture=TextureCreateFromFilePNG("data/ui/moving_wrap_repeat_t_test.png");
	TextureRec=vec4(0.0f,1.0f,1.1f,-6.4f);
	
	BorderLeft=20.0f;
	BorderTop=25.0f;
	BorderRight=20.0f;
	BorderBottom=20.0f;
	//(0.997262x0.000511:0.995740x0.988977)
	//(0.002739x0.999489:0.004261x0.011023)
	//0.000000,0.500000,0.500000,0.000000
//left
VertBorder[0 ]=vec4( reg.x,-(reg.y+reg.h)+BorderBottom,
	1.0f,0.0f);
VertBorder[1 ]=vec4( reg.x+BorderLeft,-(reg.y+reg.h)+BorderBottom,
	1.0f,0.5f);
VertBorder[2 ]=vec4( reg.x+BorderLeft, reg.y-BorderTop,
	2.5f,0.5f);
VertBorder[3 ]=vec4( reg.x, reg.y-BorderTop,
	2.5f,0.0f);
//(-0.050000,0.950000,0.950000,-0.050000)
//top
VertBorder[4 ]=vec4( reg.x,reg.y-BorderTop,
	-0.05f, 0.5f);
VertBorder[5 ]=vec4( reg.x+reg.w,reg.y-BorderTop,
	0.95f, 0.5f);
VertBorder[6 ]=vec4( reg.x+reg.w, reg.y,
	0.95f, 1.0f);
VertBorder[7 ]=vec4( reg.x, reg.y,
	-0.05f, 1.0f);

//right
VertBorder[8 ]=vec4( (reg.x+reg.w)-BorderRight,(reg.y-reg.h)+BorderBottom,
	1.0f,0.5f);
VertBorder[9 ]=vec4( reg.x+reg.w,(reg.y-reg.h)+BorderBottom,
	1.0f,0.0f);
VertBorder[10]=vec4( reg.x+reg.w, reg.y-BorderTop,
	2.5f,0.0f);
VertBorder[11]=vec4( (reg.x+reg.w)-BorderRight, reg.y-BorderTop,
	2.5f,0.5f);
//bottom
VertBorder[12]=vec4( reg.x,reg.y-reg.h, 0.0f, 0.0f);
VertBorder[13]=vec4( reg.x+reg.w,reg.y-reg.h, 1.0f, 0.0f);
VertBorder[14]=vec4( reg.x+reg.w,(reg.y-reg.h)+BorderBottom, 1.0f, 1.0f);
VertBorder[15]=vec4( reg.x,(reg.y-reg.h)+BorderBottom, 0.0f, 1.0f);

	/*_IVERTEXES PlaneBorderMesh;
	PlaneGenerateV(1,1,0.05f,&PlaneBorderMesh);
	*/
	//printf("nv:%d ni:%d\n",PlaneBorderMesh.vnum,PlaneBorderMesh.inum);
	
	/*for(uint i=0;i<PlaneBorderMesh.inum;++i)
	{
		//printf("%d,",PlaneBorderMesh.indexes[i]);
	}*/
	//printf("");
	
	/*_VERTEXAN* pv=(_VERTEXAN*)PlaneBorderMesh.vertexes;
	for(uint i=0;i<PlaneBorderMesh.vnum;++i)
	{
		printf("VertBorder[%d]=vec4(%ff,%ff,%ff,%ff);",i,pv[i].pos.x,pv[i].pos.y,pv[i].uv.x,pv[i].uv.y);
	}*/
	
	/*for(uint i=0;i<PlaneBorderMesh.vnum;++i)
	{
		//printf("{vec2(%ff,%ff),%ff},",float(i));
	}*/
	
	/*for(int i=0;i<16;++i)
	{
		//VertBorder[i].x+=(0.001250f*3.0f);
		//VertBorder[i].y-=(0.001250f*3.0f);
		//printf("VertBorder[%d]=vec4( %ff, %ff, 0.0f, 0.0f);",i,VertBorder[i].x,VertBorder[i].y);
		//VertBorder[i].x=VertBorder[i].x*3.0f;
		//VertBorder[i].y=VertBorder[i].y*3.0f;
	}*/
}

void cUiMovingWrap::OnCursorMove(float x,float y)
{
}

void cUiMovingWrap::OnCursorOut()
{
}

void cUiMovingWrap::OnUpdate()
{
	if(CursorCapture && Input.Cursor.CursorButton1)
	{
		reg.x=reg.x+(float(Input.Cursor.Pos.x)-CursorDownPosX);
		reg.y=reg.y+(float(Input.Cursor.Pos.y)-CursorDownPosY);

		CursorDownPosX=float(Input.Cursor.Pos.x);
		CursorDownPosY=float(Input.Cursor.Pos.y);		
	}
}

void cUiMovingWrap::OnCursorDown(float x,float y)
{
	if(y>25.0f)return;
	
	CursorDownPosX=float(Input.Cursor.Pos.x);
	CursorDownPosY=float(Input.Cursor.Pos.y);
	CursorCapture=true;
	
	Ui.UiCore.UpTopWindow(this);
}

void cUiMovingWrap::OnCursorUp(float x,float y)
{
	CursorCapture=false;
}

void cUiMovingWrap::Draw(vec2& pos)
{
	for(int i=0;i<16;++i)
	{
		vec2 tpos;
		ConvertToScreenCoord(VertBorder[i].x+pos.x,pos.y-VertBorder[i].y,&tpos);
		VertBorderO[i]=VertBorder[i];
		//VertBorderO[i].x=VertBorder[i].x+rec_feft_top.x;
		//VertBorderO[i].y=VertBorder[i].y+rec_feft_top.y;
		VertBorderO[i].x=tpos.x;
		VertBorderO[i].y=tpos.y;
		//printf("%d %f %f\n",i,tpos.x,VertBorderO[i].x);
	}

	mx4 WorldMx;
	MatrixInverse(&WorldMx,&Camera.ViewMx);
	//WorldMx.x30=0.0f;
	//WorldMx.x31=0.0f;
	//WorldMx.x32=0.0f;
	mx4 WorldViewProjMx=Camera.ProjMx;

	_shader_select_values_list *cursel_modelval=pIShaderTech->select_model_values;

	while(cursel_modelval->next)
	{
		c_shader_value* pval=cursel_modelval->pval;
		if(pval)
		{
			switch(pval->Semantic)
			{
			case VALSEM_WORLDMX:
				pshader->SetMatrix(pval->GId,&WorldViewProjMx);
				break;
			default:
				printf("switch default: valuesL_mesh:%d %s\n",pval->Semantic,pval->Name);
			}
		}
		cursel_modelval=cursel_modelval->next;
	}
	
	_shader_select_values_list *cursel_meshval=pIShaderTech->select_mesh_values;

	while(cursel_meshval->next)
	{
		c_shader_value* pval=cursel_meshval->pval;
		if(pval)
		{
			switch(pval->Semantic)
			{
			case VALSEM_WORLDMX:
				pshader->SetMatrix(pval->GId,&WorldViewProjMx);
				break;
			case VALSEM_SAMPLER0:
				if(pBackgroundTexture && pBackgroundTexture)pshader->SetTexture(pval->GId,pBackgroundTexture);
				//if(pTexUiet)pshader->SetTexture(pval->GId,pTexUiet);
				break;
			case VALSEM_MESH_VEC4_A:
				pshader->SetVectorArray(pval->GId,(vec4*)VertBorderO,16);
				break;
			//case VALSEM_WEIGHTS:
				//pshader->SetVector(pval->GId,(vec4*)&v4rect);
			//	break;
			default:
				printf("switch default: valuesL_mesh:%d %s\n",pval->Semantic,pval->Name);
			}
		}
		cursel_meshval=cursel_meshval->next;
	}
	
	/*
	pshader->SetVertexBuffer(IVertexesBorderSprite);
	pshader->SetIndexBuffer(IIndexesBorderSprite);
	pshader->Begin(pIShaderTech->TechID,16,64,_VERTTYPE_E);
	*/
	
	//glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	pshader->SetVertexBuffer(IVertexesBorderSprite);
	pshader->SetIndexBuffer(IIndexesBorderSprite);
	
	pshader->Begin(pIShaderTech->TechID,16,24,_VERTTYPE_E);
	
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//glEnable(GL_CULL_FACE);
}