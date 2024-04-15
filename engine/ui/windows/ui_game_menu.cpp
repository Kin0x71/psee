#include "ui_game_menu.h"

#include "../ui.h"
#include "ui_name_plates.h"
#include "ui_console.h"

#include "../../input_control.h"

extern cUi Ui;
extern cUiConsole* MainConsole;

extern uint OPT_SCREEN_W;
extern uint OPT_SCREEN_H;

cUiGameMenu::cUiGameMenu()
{
	ButtonCamControlTop=0;
	ButtonCamControlBottom=0;
	
	GameInfoLayer=0;
	GameInfoLayerBackgroundTexture=0;
	NamePlates=0;
	
	DebugBackground=0;
	pDebugBackgroundTexture=0;
		
	TextDebugLinesCount=0;
	TextDebugLines=0;
	DBG_Cursor=0;
}
	
cUiGameMenu::~cUiGameMenu()
{//DBG_LOG("~cUiGameMenu()");
	if(ButtonCamControlTop){
		delete ButtonCamControlTop;
		ButtonCamControlTop=0;
	}
		
	if(ButtonCamControlBottom){
		delete ButtonCamControlBottom;
		ButtonCamControlBottom=0;
	}

	delete GameInfoLayer;
	
	if(GameInfoLayerBackgroundTexture){
		delete GameInfoLayerBackgroundTexture;
		GameInfoLayerBackgroundTexture=0;
	}
	
	delete NamePlates;

	delete DebugBackground;
	delete pDebugBackgroundTexture;
	
	delete[] TextDebugLines;
	/*
	if(TextDebugLines){
		DBG_LOG("TextDebugLinesCount:%d",TextDebugLinesCount);
		for(int i=0;i<TextDebugLinesCount;++i)
		{
			DBG_LOG("TextDebugLines[%d]:%x",i,TextDebugLines[i]);
			if(TextDebugLines[i]){
				DBG_LOG("T1:%x",TextDebugLines[i]);
//				DBG_LOG("T2:%x",TextDebugLines[i]->pTexture);
//				DBG_LOG("T3:%x",&TextDebugLines[i]->pTexture->surf);
//				DBG_LOG("T4:%x",TextDebugLines[i]->pTexture->surf.buff);
				delete TextDebugLines[i];
				DBG_LOG("delete TextDebugLines[%d] ok",i);
				TextDebugLines[i]=0;
			}
		}
		delete[] TextDebugLines;
		TextDebugLines=0;
	}*/
}

void cUiGameMenu::Create()
{
	MainConsole=(cUiConsole*)Ui.UiCore.CreateUiWindow(UI_CONSOLE,"MainConsole",win_reg(10.0f,float(OPT_SCREEN_H)-210.0f,300.0f,200.0f));
	//MainConsole->Create(10.0f,float(OPT_SCREEN_H)-210.0f,300.0f,200.0f);
	MainConsole->pUiConsole_OnEnter_Callback=input_control_UiConsole_OnEnter;
	/*
	cTexture** ppsvbtn=TexturesControl.TextureCreateFromFileJPG("data/ui/scroll/svbtn.jpg");
	cTexture** ppsvbto=TexturesControl.TextureCreateFromFileJPG("data/ui/scroll/svbto.jpg");
	cTexture** ppsvbtp=TexturesControl.TextureCreateFromFileJPG("data/ui/scroll/svbtp.jpg");
	
	ButtonCamControlTop=(cUiButton*)Ui.UiCore.CreateUiWindow(
			UI_BUTTON,
			"ButtonCamControlTop",
			win_reg(float(OPT_SCREEN_W)/2.0f,0.0f,200.0f,100.0f)
			);
	
	ButtonCamControlTop->FontId=1;
	ButtonCamControlTop->SetNormalParam(*ppsvbtn,"",0,255,0);
	ButtonCamControlTop->SetOverParam(*ppsvbto,"",128,255,128);
	ButtonCamControlTop->SetPushedParam(*ppsvbtp,"",255,255,0);
	ButtonCamControlTop->TextPos.x=0.0f;
	ButtonCamControlTop->TextPos.y=0.0f;
	ButtonCamControlTop->PushedOffset.x=1.0f;
	ButtonCamControlTop->PushedOffset.y=1.0f;*/
	/*
	ButtonCamControlBottom=(cUiButton*)Ui.UiCore.CreateUiWindow(
			UI_BUTTON,
			"ButtonCamControlBottom",
			win_reg(float(OPT_SCREEN_W)/2.0f,float(OPT_SCREEN_H)-300.0f,300.0f,300.0f)
			);
	
	ButtonCamControlBottom->FontId=1;
	ButtonCamControlBottom->SetNormalParam(*ppsvbtn,"",0,255,0);
	ButtonCamControlBottom->SetOverParam(*ppsvbto,"",128,255,128);
	ButtonCamControlBottom->SetPushedParam(*ppsvbtp,"",255,255,0);
	ButtonCamControlBottom->TextPos.x=0.0f;
	ButtonCamControlBottom->TextPos.y=0.0f;
	ButtonCamControlBottom->PushedOffset.x=1.0f;
	ButtonCamControlBottom->PushedOffset.y=1.0f;*/
/*
	GameInfoLayer=Ui.UiCore.CreateUiWindow(UI_BASE,"GameInfoLayer",win_reg(0.0f,0.0f,float(OPT_SCREEN_W),float(OPT_SCREEN_H)));
	
	GameInfoLayerBackgroundTexture=new cTexture();
	
	glGenTextures(1,&GameInfoLayerBackgroundTexture->textureID);
	GameInfoLayerBackgroundTexture->surf.w=1;
	GameInfoLayerBackgroundTexture->surf.h=1;
	GameInfoLayerBackgroundTexture->surf.pixel_size=4;
	GameInfoLayerBackgroundTexture->surf.buff=new uchar[GameInfoLayerBackgroundTexture->surf.pixel_size];
	memset(GameInfoLayerBackgroundTexture->surf.buff,0,GameInfoLayerBackgroundTexture->surf.pixel_size);
	GameInfoLayerBackgroundTexture->Init(true);
	
	GameInfoLayer->SetBackground(&GameInfoLayerBackgroundTexture);
*/
	NamePlates=new cUiNamePlates();
	
	float w=250.0f;
	float h=20.0f;

	pDebugBackgroundTexture=TextureCreateFromFilePNG("data/ui/px-3-black.png");

	TextDebugLinesCount=7;
	TextDebugLines=new cUiText*[TextDebugLinesCount];

	DebugBackground=Ui.UiCore.CreateUiWindow(UI_BASE,"DebugBackground",win_reg(float(OPT_SCREEN_W)-w,0,w,h*TextDebugLinesCount));

	DebugBackground->SetBackground(pDebugBackgroundTexture);

	float x=0.0f;
	float y=0.0f;
	
	for(int i=0;i<TextDebugLinesCount;++i)
	{
		TextDebugLines[i]=(cUiText*)Ui.UiCore.CreateUiWindow(
			UI_TEXT,
			"TextDebugLine",
			win_reg(x,y,w,h),
			DebugBackground
			);
		y+=h;
	
		TextDebugLines[i]->SetFont(1);
		TextDebugLines[i]->AllockBuff(1024);
	}

	TextDebugLines[0]->SetText("XXXXXXXXXX",50,255,50);
	/*
	DBG_Cursor=(cUiBase*)Ui.UiCore.CreateUiWindow(
			UI_BASE,
			"DBG_Cursor",
			win_reg(0.0f,0.0f,50.0f,50.0f)
			);
	
	DBG_Cursor->SetBackground(&pDbgUettTexture);*/
}

void cUiGameMenu::SetFormatText(int line,char* format,...)
{
	if(TextDebugLines)
	{
		char buff[1024];
		va_list marker;
		va_start( marker, format );
		vsprintf(buff,format, marker);
		va_end( marker );
	
		TextDebugLines[line]->SetFormatText(50,255,50,buff);
	}
}