#ifndef _UI_CONSOLE_H_
#define _UI_CONSOLE_H_

#include "ui_scroll.h"

class cUiTextEdit;
typedef void (*_UiConsole_OnEnter_Callback)(cUiBase*,char*);

class cUiConsole:public cUiBase
{
public:
	//cUiBase* Wrap;
	cUiTextEdit* TextView;
	cUiTextEdit* TextInput;
	cUiText* SeparatorLine;
	cUiScroll* ScrollBar;
	
	_UiConsole_OnEnter_Callback pUiConsole_OnEnter_Callback;

	struct _command_item{
		_command_item* next;
		_command_item* prev;
		char* buff;
		int len;
		
		_command_item()
		{
			next=0;
			prev=0;
			buff=0;
			len=0;
		}
		~_command_item()
		{
			if(buff){
				delete[] buff;
				buff=0;
			}
			
			if(next){
				delete next;
				next=0;
			}
		}
		
		void set(char* str){
			len=strlen(str);
			buff=new char[len+1];
			memcpy(buff,str,len);
			buff[len]=0;
		}
	};
	
	_command_item* CommandRoot;
	_command_item* CommandCur;
	_command_item* CommandLast;
	
	cUiConsole(char* winname,float x,float y,float w,float h,cUiBase* parent=0):cUiBase(winname,x,y,w,h,parent)
	{
		UiWindowClass=UI_CONSOLE;
		
		//this->base_constructor(winname,x,y,w,h,parent);
		
		//Wrap=0;
		TextView=0;
		TextInput=0;
		SeparatorLine=0;
		ScrollBar=0;
		pUiConsole_OnEnter_Callback=0;
		CommandRoot=CommandLast=CommandCur=new _command_item();
	}

	~cUiConsole()
	{
		if(TextView){
			delete TextView;
			TextView=0;
		}

		if(TextInput){
			delete TextInput;
			TextInput=0;
		}

		if(SeparatorLine){
			delete SeparatorLine;
			SeparatorLine=0;
		}

		if(ScrollBar){
			delete ScrollBar;
			ScrollBar=0;
		}

		if(CommandRoot){
			delete CommandRoot;
			CommandRoot=0;
		}
	}

	void OnCreate();
	void OnKeyDown(_KEY_KODE_ key);
};

#endif