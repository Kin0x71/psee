#include "ui.h"

extern cScreen* pScreen;
extern cUi Ui;
extern uint OPT_SCREEN_W;
extern uint OPT_SCREEN_H;


#include "../camera.h"
//#include "../md5.h"

#include "windows/ui_game_menu.h"

#include "windows/ui_console.h"
#include "windows/ui_uett.h"

//extern cTexturesControl TexturesControl;
extern cCamera Camera;
int camstat=1;
extern cTextBase TextBase;

//extern cDgAllockWrapp DgAllockWrapp;

extern cUiText* pUiWinCaption1;

cUiConsole* MainConsole;

void cUi::Init()
{
	UiCore.Init();
}

void cUi::CreateLoadScreen()
{
	LoadScreen=new cUiUett();
	LoadScreen->Create();
}

void cUi::DeleteLoadScreen()
{
	delete LoadScreen;
	LoadScreen=0;
}

void cUi::CreateGameMenu()
{
	GameMenu=new cUiGameMenu();
	GameMenu->Create();
	//MainConsole=new cUiConsole("Console",10.0f,float(OPT_SCREEN_H)-210.0f,300.0f,200.0f);
	/*
	cUiBase* pUiWin=Ui.UiCore.CreateUiWindow(UI_BASE,"042",win_reg(10.0f,float(OPT_SCREEN_H)-210.0f,300.0f,200.0f));
	pUiWin->SetBackground(TexturesControl.TextureCreateFromFilePNG("data/ui/px-3-blackpng"));

	TextEdit=(cUiTextEdit*)Ui.UiCore.CreateUiWindow(UI_TEXT_EDIT,"TextEdit",win_reg(5.0f,5.0f,290.0f,190.0f),pUiWin);
	TextEdit->SetFont(1);
	TextEdit->r=50;
	TextEdit->g=255;
	TextEdit->b=50;
	*/
	/*
	//TextEdit->InputStr("1234567890987654321\nabcdefg\nABSDEFG");
	TextEdit->InputStr("1abcdefg\n2abcdefg\n3abcdefgewq\n4abcdefg\n5abcdefg\n6abcdefgqwe\n7abcdefg\n8abcdefgA1\n9abcdefgABCDEF");
	
	cUiBase* pUiWin1=Ui.UiCore.CreateUiWindow(UI_BASE,"043",win_reg(320.0f,float(OPT_SCREEN_H)-210.0f,100.0f,200.0f));
	pUiWin1->SetBackground(TexturesControl.TextureCreateFromFilePNG("data/ui/px-3-blackpng"));

	cUiTextEdit* TextEdit1=(cUiTextEdit*)Ui.UiCore.CreateUiWindow(UI_TEXT_EDIT,"TextEdit",win_reg(5.0f,5.0f,90.0f,190.0f),pUiWin1);
	TextEdit1->SetFont(1);
	TextEdit1->r=50;
	TextEdit1->g=255;
	TextEdit1->b=50;

	TextEdit1->InputStr("123456789\nabcdefg\nABSDEFG1qwe");

	cUiBase* pUiWin2=Ui.UiCore.CreateUiWindow(UI_BASE,"044",win_reg(430.0f,float(OPT_SCREEN_H)-210.0f,100.0f,200.0f));
	pUiWin2->SetBackground(TexturesControl.TextureCreateFromFilePNG("data/ui/px-3-blackpng"));

	cUiTextEdit* TextEdit2=(cUiTextEdit*)Ui.UiCore.CreateUiWindow(UI_TEXT_EDIT,"TextEdit",win_reg(5.0f,5.0f,90.0f,190.0f),pUiWin2);
	TextEdit2->SetFont(1);
	TextEdit2->r=50;
	TextEdit2->g=255;
	TextEdit2->b=50;

	TextEdit2->InputStr("123456789\nabcdefg\nABSDEFG");
	*/
}