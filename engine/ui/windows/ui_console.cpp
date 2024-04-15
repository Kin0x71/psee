#include "ui_console.h"
#include "../ui.h"
#include "../../input.h"

extern cTextBase TextBase;
extern cUi Ui;
extern cInput Input;

#define CONSOLE_FONT_ID 1

void cUiConsole_CursorDown_ScrollUp(cUiBase* self,int x,int y)
{
	cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ScrollUp:%s",TextView->name);
	
	TextView->ScrollOffsetV(1);
}

void cUiConsole_CursorDown_ScrollDown(cUiBase* self,int x,int y)
{
	cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ScrollDown:%s",TextView->name);
	
	TextView->ScrollOffsetV(-1);
}

void cUiConsole_CursorDown_ButtonControl(cUiBase* self,int x,int y)
{
	//cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	//printf("cUiConsole_CursorDown_ButtonControl:%s",TextView->name);
	//ButtonControl_LastCursorY=Input.Cursor.Pos.y;
}

void cUiConsole_CursorUp_ButtonControl(cUiBase* self,int x,int y)
{
	//ButtonControl_LastCursorY=-1.0f;
}

void cUiConsole_CursorOut_ButtonControl(cUiBase* self)
{
//	printf("cUiConsole_CursorOut_ButtonControl");
	//ButtonControl_LastCursorY=-1.0f;
}

void cUiConsole_OnScroll(cUiBase* self,float ofs)
{
	cUiTextEdit* TextView=(cUiTextEdit*)self->parent->parent->GetSubWinByName("TextView");
	cUiButton* ButtonControl=(cUiButton*)((cUiScroll*)self)->ButtonControl;
	cUiButton* ButtonA=(cUiButton*)((cUiScroll*)self)->ButtonA;
	cUiButton* ButtonB=(cUiButton*)((cUiScroll*)self)->ButtonB;
	
	TextView->ScrollSetV(
		-(
			(((ButtonControl->reg.y-(ButtonA->reg.y+ButtonA->reg.h))/(self->reg.h-ButtonB->reg.h)))*TextView->lines_count*int(TextBase.ppFonts[CONSOLE_FONT_ID]->h)
		)
	);
	/*
	printf("%f %d %f",
			TextView->reg.h,
			TextView->lines_count,
			(((ButtonControl->reg.y-(ButtonA->reg.y+ButtonA->reg.h))/(self->reg.h-ButtonB->reg.h)))*TextView->lines_count*int(TextBase.ppFonts[CONSOLE_FONT_ID]->h)
			);*/
}

bool cUiConsole_TextInput_OnKeyDown(cUiBase* self,_KEY_KODE_ key)
{
	switch(key)
	{
		case _KEY_ENTER:
		{
			cUiConsole* pUiConsole=(cUiConsole*)self->parent;
			cUiTextEdit* TextInput=(cUiTextEdit*)self;
			cUiTextEdit* TextView=pUiConsole->TextView;
			//TextInput->OnUpdate();

			if(pUiConsole->pUiConsole_OnEnter_Callback)pUiConsole->pUiConsole_OnEnter_Callback(pUiConsole,TextInput->CurentLine->buffer);

			TextView->InputStr(TextInput->CurentLine->buffer);
			TextView->InputStr("\n");

			pUiConsole->CommandLast->set(TextInput->CurentLine->buffer);
			pUiConsole->CommandCur=pUiConsole->CommandLast;
			
			pUiConsole->CommandLast->next=new cUiConsole::_command_item();
			pUiConsole->CommandLast->next->prev=pUiConsole->CommandLast;
			pUiConsole->CommandLast=pUiConsole->CommandLast->next;
			
			ClearRect(&TextInput->pTexture->surf,TextInput->caret_x,TextInput->caret_y+TextInput->cursor_offsety,TextInput->cursor_w,TextInput->cursor_h);

			TextInput->ClearLinesTexture(0);
			TextInput->caret_x=TextInput->XStartOffset;
			TextInput->caret_char=0;
			TextInput->CurentLine->length=0;

			return false;
		}
		break;
		case _KEY_UP:
		{
			if(Input.IsKeyDown(_KEY_ALTR)){
				cUiConsole* pUiConsole=(cUiConsole*)self->parent;
				
				cUiTextEdit* TextInput=(cUiTextEdit*)self;
				ClearRect(&TextInput->pTexture->surf,TextInput->caret_x,TextInput->caret_y+TextInput->cursor_offsety,TextInput->cursor_w,TextInput->cursor_h);

				TextInput->ClearLinesTexture(0);
				TextInput->caret_x=TextInput->XStartOffset;
				TextInput->caret_char=0;
				TextInput->CurentLine->length=0;
				
				TextInput->InputStr(pUiConsole->CommandCur->buff);

				if(pUiConsole->CommandCur->prev){
					pUiConsole->CommandCur=pUiConsole->CommandCur->prev;
				}
			}
		}
		break;
		case _KEY_DOWN:
		{
			if(Input.IsKeyDown(_KEY_ALTR)){
				cUiConsole* pUiConsole=(cUiConsole*)self->parent;
				
				if(pUiConsole->CommandCur->next && pUiConsole->CommandCur->next->next){
					pUiConsole->CommandCur=pUiConsole->CommandCur->next;
				
					cUiTextEdit* TextInput=(cUiTextEdit*)self;
					ClearRect(&TextInput->pTexture->surf,TextInput->caret_x,TextInput->caret_y+TextInput->cursor_offsety,TextInput->cursor_w,TextInput->cursor_h);

					TextInput->ClearLinesTexture(0);
					TextInput->caret_x=TextInput->XStartOffset;
					TextInput->caret_char=0;
					TextInput->CurentLine->length=0;

					TextInput->InputStr(pUiConsole->CommandCur->buff);
				}
			}
		}
		break;
	}
	
	return true;
}

void cUiConsole::OnCreate()
{
//	printf("cUiConsole::OnCreate:####################################################");
	//reg=win_reg(x,y,w,h);

	//Wrap=Ui.UiCore.CreateUiWindow(UI_BASE,"ConsoleFrame",reg,this);
	SetBackground(TextureCreateFromFilePNG("data/ui/px-3-black.png"));

	TextView=(cUiTextEdit*)Ui.UiCore.CreateUiWindow(UI_TEXT_EDIT,"TextView",win_reg(5.0f,5.0f,reg.w-20.0f,reg.h-37.0f),this);
	TextView->SetFont(CONSOLE_FONT_ID);
	TextView->r=50;
	TextView->g=255;
	TextView->b=50;
	TextView->focus_keeper_flag=false;

	SeparatorLine=(cUiText*)Ui.UiCore.CreateUiWindow(UI_TEXT,"SeparatorLine",win_reg(5.0f,reg.h-44.0f,reg.w-5.0f,17.0f),this);
	SeparatorLine->SetFont(CONSOLE_FONT_ID);

	int cid=uchar('_')-32;
	int line_char_w=TextBase.ppFonts[CONSOLE_FONT_ID]->pCharacters[cid].w+TextBase.ppFonts[CONSOLE_FONT_ID]->pCharacters[cid].ofsx+TextBase.ppFonts[CONSOLE_FONT_ID]->CharSpace;
	int lines_count=int(reg.w-10.0f)/line_char_w;
	char* line_str=new char[lines_count+1];
	memset(line_str,'_',lines_count);
	line_str[lines_count]=0;

	SeparatorLine->SetText(line_str,50,255,50);

	delete[] line_str;

	TextInput=(cUiTextEdit*)Ui.UiCore.CreateUiWindow(UI_TEXT_EDIT,"TextInput",win_reg(5.0f,reg.h-24.0f,reg.w-5.0f,17.0f),this);
	TextInput->SetFont(CONSOLE_FONT_ID);
	TextInput->r=50;
	TextInput->g=255;
	TextInput->b=50;
	TextInput->pUiTextEdit_OnKeyDown_Callback=cUiConsole_TextInput_OnKeyDown;

	ScrollBar=(cUiScroll*)Ui.UiCore.CreateUiWindow(UI_SCROLL,"ScrollBar",win_reg(reg.w-25.0f,0.0f,24.0f,reg.h-37.0f),this);
	
	ScrollBar->pUiScroll_OnScroll_Callback=cUiConsole_OnScroll;
	/*
	FILE* pf=fopen("data/shaders/Main.fx","rb");
	fseek(pf,0,SEEK_END);
	uint fsize=ftell(pf);
	fseek(pf,0,0);
	char* fdata=new char[fsize+1];
	fread(fdata,1,fsize,pf);
	fclose(pf);
	fdata[fsize/2]=0;
	TextView->InputStr(fdata);
	TextView->ScrollSetV(TextView->lines_count/int(TextBase.ppFonts[CONSOLE_FONT_ID]->h));*/
	
//	TextInput->InputStr("/create_zone 1 1 test_zone 24 32");
	TextInput->InputStr("/teleport_by_gui 0 1 1 12 1 2");
}

void cUiConsole::OnKeyDown(_KEY_KODE_ key)
{
//	printf("cUiConsole::OnKeyDown(%d)",key);
}