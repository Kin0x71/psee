#ifndef _INPUT_H_
#define _INPUT_H_

#include "input_keys.h"
#include <engine/common/base_types.h>
#include <engine/events/tevents.h>

#if defined(LINUX_SPACE)
#include <cstring>
#include <cstdio>
#elif defined(ANDROID_SPACE) || defined(WINDOWS_SPACE)
#include <string.h>
#include <stdio.h>
#endif

#define CURSOR_DOWN 1
#define CURSOR_UP 2
#define CURSOR_DOUBLE_CLICK 3
#define CURSOR_MOUSEWHEEL 4

#define KEY_BUFFER_SIZE _KEY_MAX

#if defined(ANDROID_SPACE)
enum ANDROID_INPUT_EVENT
{
	ANDROID_INPUT_EVENT_EMPTY=0,
	ANDROID_INPUT_EVENT_TOUCH_DOWN=1,
	ANDROID_INPUT_EVENT_TOUCH_UP=2,
	ANDROID_INPUT_EVENT_TOUCH_MOVE=3
};

struct _ANDROID_INPUT
{
	ANDROID_INPUT_EVENT Event;
	float x,y;
	uint time;
	
	_ANDROID_INPUT()
	{
		Event=ANDROID_INPUT_EVENT_EMPTY;
		x=y=0.0f;
		time=0;
	}
};

struct _android_input_event_item
{
	_ANDROID_INPUT AndroidInput;
	_android_input_event_item* next;
	
	_android_input_event_item()
	{
		next=0;
	}
	~_android_input_event_item()
	{
		
	}
};
#endif

struct _INPUT_KEY
{
	bool pressed;
	uint time;
};

struct _CHARACTER_KEY_TABLE
{
	char lower;
	char upper;
	_CHARACTER_KEY_TABLE()
	{
		lower=0;
		upper=0;
	}
};

struct _INPUT_CURSOR
{
	struct _CURSOR_POS
	{
		float x,y;
		float last_x,last_y;
		
		float move_control_x;
		float move_control_y;
		bool changed;
		
#ifdef ANDROID_SPACE
		ANDROID_INPUT_EVENT android_Event;
#endif
		_CURSOR_POS()
		{
			x=y=last_x=last_y=0.0f;
			move_control_x=0.0f;
			move_control_y=0.0f;
			changed=false;
		}
	};
	
	bool CursorButton1;
	uint CursorButton1Time;
	bool CursorButton1DownChecked;
	bool CursorButton1Down;
	bool CursorButton1Up;
	bool CursorButton1UpChecked;
	bool CursorButton1Click;
	uint CursorButton1ClickTime;
	uint CursorButton1ClickCount;
	bool CursorButton1DoubleClick;
	
#ifdef ANDROID_SPACE
	ANDROID_INPUT_EVENT android_last_event;
	bool long_tap;
#else
	bool CursorButton2;
	uint CursorButton2Time;
	bool CursorButton2DownChecked;
	bool CursorButton2Down;
	bool CursorButton2Up;
	bool CursorButton2UpChecked;
	bool CursorButton2Click;
	uint CursorButton2ClickTime;
	uint CursorButton2ClickCount;
	bool CursorButton2DoubleClick;
#endif
	
	_CURSOR_POS Pos;
	
	_INPUT_CURSOR()
	{
		CursorButton1=false;
		CursorButton1Time=0;
		CursorButton1DownChecked=false;
		CursorButton1UpChecked=false;
		CursorButton1Down=false;
		CursorButton1Up=false;
		CursorButton1Click=false;
		CursorButton1ClickTime=0;
		CursorButton1ClickCount=0;
		CursorButton1DoubleClick=false;
		
#ifdef ANDROID_SPACE
		android_last_event=ANDROID_INPUT_EVENT_EMPTY;
		long_tap=false;
#else
		CursorButton2=false;
		CursorButton2Time=0;
		CursorButton2DownChecked=false;
		CursorButton2UpChecked=false;
		CursorButton2Down=false;
		CursorButton2Up=false;
		CursorButton2Click=false;
		CursorButton2ClickTime=0;
		CursorButton2ClickCount=0;
		CursorButton2DoubleClick=false;
#endif
	}
	/*
	void SetCursor1DownCallback(_Cursor1DownCallback Func);
	void SetCursor2DownCallback(_Cursor1DownCallback Func);
	void SetCursor1UpCallback(_Cursor1DownCallback Func);
	void SetCursor2UpCallback(_Cursor1DownCallback Func);
	*/
};

class cInput
{
public:
	_INPUT_KEY KeyBuffer[KEY_BUFFER_SIZE];
	_CHARACTER_KEY_TABLE KeyCharTab[KEY_BUFFER_SIZE];
	_INPUT_CURSOR Cursor;

	struct EventKeyArg{
		_KEY_KODE_ kode;
		int stat;

		EventKeyArg(){
			kode = _KEY_NON;
			stat = -1;
		}

		EventKeyArg(_KEY_KODE_ k, int s){
			kode = k;
			stat = s;
		}
	};

	struct EventCursorButtonArg{
		int button;
		uint event;

		EventCursorButtonArg(){
			button=-1;
			event = -1;
		}

		EventCursorButtonArg(int b, uint e){
			button = b;
			event = e;
		}
	};

	struct EventCursorMoveArg{
		float x;
		float y;

		EventCursorMoveArg(){
			x = -1.0f;
			y = -1.0f;
		}

		EventCursorMoveArg(float X, float Y){
			x = X;
			y = Y;
		}
	};

	typedef void(EventKeyCallback)(const EventKeyArg&);
	typedef void(EventCursorButtonCallback)(const EventCursorButtonArg&);
	typedef void(EventCursorMoveCallback)(const EventCursorMoveArg&);

	static cTEvent_System<EventKeyCallback, EventKeyArg> EventKey;
	static cTEvent_System<EventCursorButtonCallback, EventCursorButtonArg> EventCursorButton;
	static cTEvent_System<EventCursorMoveCallback, EventCursorMoveArg> EventCursorMove;
	
#ifdef ANDROID_SPACE
	_android_input_event_item* AndroidInputListRoot;
	_android_input_event_item* AndroidInputItemLast;
#endif
	
	cInput()
	{
		memset(KeyBuffer,0,sizeof(_INPUT_KEY)*KEY_BUFFER_SIZE);
		InitKeys();
		
#ifdef ANDROID_SPACE
		AndroidInputListRoot=AndroidInputItemLast=new _android_input_event_item();
#endif
	}
		
void InitKeys();

bool Update();
void ResetStats();
_KEY_KODE_ CodeToKey(int key_code);
void KeyDown(uint key);
bool IsKeyDown(uint key);
};

#endif