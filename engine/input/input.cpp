
#include <engine/common/screen.h>
#include <engine/common/timer.h>
#include "input.h"

extern cTimer Timer;

cTEvent_System<cInput::EventKeyCallback, cInput::EventKeyArg> cInput::EventKey;
cTEvent_System<cInput::EventCursorButtonCallback, cInput::EventCursorButtonArg> cInput::EventCursorButton;
cTEvent_System<cInput::EventCursorMoveCallback, cInput::EventCursorMoveArg> cInput::EventCursorMove;

void cInput::InitKeys()
{
	KeyCharTab[_KEY_1].lower = '1';
	KeyCharTab[_KEY_2].lower = '2';
	KeyCharTab[_KEY_3].lower = '3';
	KeyCharTab[_KEY_4].lower = '4';
	KeyCharTab[_KEY_5].lower = '5';
	KeyCharTab[_KEY_6].lower = '6';
	KeyCharTab[_KEY_7].lower = '7';
	KeyCharTab[_KEY_8].lower = '8';
	KeyCharTab[_KEY_9].lower = '9';
	KeyCharTab[_KEY_0].lower = '0';

	KeyCharTab[_KEY_1].upper = '!';
	KeyCharTab[_KEY_2].upper = '@';
	KeyCharTab[_KEY_3].upper = '#';
	KeyCharTab[_KEY_4].upper = '$';
	KeyCharTab[_KEY_5].upper = '%';
	KeyCharTab[_KEY_6].upper = '^';
	KeyCharTab[_KEY_7].upper = '&';
	KeyCharTab[_KEY_8].upper = '*';
	KeyCharTab[_KEY_9].upper = '(';
	KeyCharTab[_KEY_0].upper = ')';

	KeyCharTab[_KEY_SC4].lower = '-';
	KeyCharTab[_KEY_SC4].upper = '_';
	KeyCharTab[_KEY_SC2].lower = '=';
	KeyCharTab[_KEY_SC2].upper = '+';

	KeyCharTab[_KEY_A].upper = 'A';
	KeyCharTab[_KEY_B].upper = 'B';
	KeyCharTab[_KEY_C].upper = 'C';
	KeyCharTab[_KEY_D].upper = 'D';
	KeyCharTab[_KEY_E].upper = 'E';
	KeyCharTab[_KEY_F].upper = 'F';
	KeyCharTab[_KEY_G].upper = 'G';
	KeyCharTab[_KEY_H].upper = 'H';
	KeyCharTab[_KEY_I].upper = 'I';
	KeyCharTab[_KEY_J].upper = 'J';
	KeyCharTab[_KEY_K].upper = 'K';
	KeyCharTab[_KEY_L].upper = 'L';
	KeyCharTab[_KEY_M].upper = 'M';
	KeyCharTab[_KEY_N].upper = 'N';
	KeyCharTab[_KEY_O].upper = 'O';
	KeyCharTab[_KEY_P].upper = 'P';
	KeyCharTab[_KEY_Q].upper = 'Q';
	KeyCharTab[_KEY_R].upper = 'R';
	KeyCharTab[_KEY_S].upper = 'S';
	KeyCharTab[_KEY_T].upper = 'T';
	KeyCharTab[_KEY_U].upper = 'U';
	KeyCharTab[_KEY_V].upper = 'V';
	KeyCharTab[_KEY_W].upper = 'W';
	KeyCharTab[_KEY_X].upper = 'X';
	KeyCharTab[_KEY_Y].upper = 'Y';
	KeyCharTab[_KEY_Z].upper = 'Z';

	KeyCharTab[_KEY_A].lower = 'a';
	KeyCharTab[_KEY_B].lower = 'b';
	KeyCharTab[_KEY_C].lower = 'c';
	KeyCharTab[_KEY_D].lower = 'd';
	KeyCharTab[_KEY_E].lower = 'e';
	KeyCharTab[_KEY_F].lower = 'f';
	KeyCharTab[_KEY_G].lower = 'g';
	KeyCharTab[_KEY_H].lower = 'h';
	KeyCharTab[_KEY_I].lower = 'i';
	KeyCharTab[_KEY_J].lower = 'j';
	KeyCharTab[_KEY_K].lower = 'k';
	KeyCharTab[_KEY_L].lower = 'l';
	KeyCharTab[_KEY_M].lower = 'm';
	KeyCharTab[_KEY_N].lower = 'n';
	KeyCharTab[_KEY_O].lower = 'o';
	KeyCharTab[_KEY_P].lower = 'p';
	KeyCharTab[_KEY_Q].lower = 'q';
	KeyCharTab[_KEY_R].lower = 'r';
	KeyCharTab[_KEY_S].lower = 's';
	KeyCharTab[_KEY_T].lower = 't';
	KeyCharTab[_KEY_U].lower = 'u';
	KeyCharTab[_KEY_V].lower = 'v';
	KeyCharTab[_KEY_W].lower = 'w';
	KeyCharTab[_KEY_X].lower = 'x';
	KeyCharTab[_KEY_Y].lower = 'y';
	KeyCharTab[_KEY_Z].lower = 'z';

	KeyCharTab[_KEY_SC8].lower = '[';
	KeyCharTab[_KEY_SC8].upper = '{';
	KeyCharTab[_KEY_SC10].lower = ']';
	KeyCharTab[_KEY_SC10].upper = '}';

	KeyCharTab[_KEY_SC1].lower = ';';
	KeyCharTab[_KEY_SC1].upper = ':';

	KeyCharTab[_KEY_SC11].lower = '\'';
	KeyCharTab[_KEY_SC11].upper = '"';

	KeyCharTab[_KEY_SC7].lower = '`';
	KeyCharTab[_KEY_SC7].upper = '~';

	KeyCharTab[_KEY_SC9].lower = '\\';
	KeyCharTab[_KEY_SC9].upper = '|';

	KeyCharTab[_KEY_SC3].lower = ',';
	KeyCharTab[_KEY_SC3].upper = '<';

	KeyCharTab[_KEY_SC5].lower = '.';
	KeyCharTab[_KEY_SC5].upper = '>';

	KeyCharTab[_KEY_SC6].lower = '/';
	KeyCharTab[_KEY_SC6].upper = '?';

	KeyCharTab[_KEY_SPACE].lower = ' ';
	KeyCharTab[_KEY_TAB].lower = '	';
	KeyCharTab[_KEY_ENTER].lower = '\n';
	KeyCharTab[_KEY_BACK].lower = 0;
	KeyCharTab[_KEY_DELETE].lower = 0;
}

_KEY_KODE_ cInput::CodeToKey(int key_code)
{
#if defined(WINDOWS_SPACE)
	//DBG_LOG("->%d\n",key_code);
	switch(key_code)
	{
		case 8:
			return _KEY_BACK;
			break;
		case 13:
			return _KEY_ENTER;
			break;
		case 16:
			if(GetKeyState(VK_LSHIFT) & 0x8000){
				return _KEY_SHIFTL;
			}
			else{
				return _KEY_SHIFTR;
			}
			break;
		case 17:
			if(GetKeyState(VK_LCONTROL) & 0x8000){
				return _KEY_CTRLL;
			}
			else{
				return _KEY_CTRLR;
			}
			break;
		case 18:
			if(GetKeyState(VK_LMENU) & 0x8000){
				return _KEY_ALTL;
			}
			else{
				return _KEY_ALTR;
			}
			break;
		case 27:
			return _KEY_ESC;
			break;
		case 46:
			return _KEY_DELETE;
			break;
		case 91:
			return _KEY_WIN;
			break;
		case 93:
			return _KEY_MENU;
			break;
	}

	if(key_code > 31 && key_code < 41)
	{
		return _KEY_KODE_((key_code - 32) + _KEY_SPACE);
	}

	if(key_code > 47 && key_code < 58)
	{
		return _KEY_KODE_((key_code - 48) + _KEY_0);
	}

	if(key_code > 64 && key_code < 91)
	{
		return _KEY_KODE_((key_code - 65) + _KEY_A);
	}

	if(key_code > 111 && key_code < 124)
	{
		return _KEY_KODE_((key_code - 112) + _KEY_F1);
	}

	if(key_code > 185 && key_code < 193)
	{
		return _KEY_KODE_((key_code - 186) + _KEY_SC1);
	}

	if(key_code > 218 && key_code < 223)
	{
		return _KEY_KODE_((key_code - 219) + _KEY_SC8);
	}
#elif defined(LINUX_SPACE)
	if(key_code > 8 && key_code < 75)
	{
		return _KEY_KODE_((key_code - 9) + _KEY_ESC);
	}

	if(key_code > 109 && key_code < 120)
	{
		return _KEY_KODE_((key_code - 110) + _KEY_HOME);
	}

	switch(key_code)
	{
		case 95:
			return _KEY_F11;
			break;
		case 96:
			return _KEY_F12;
			break;
		case 105:
			return _KEY_CTRLR;
			break;
		case 108:
			return _KEY_ALTR;
			break;
		case 127:
			return _KEY_PAUSE;
			break;
		case 133:
			return _KEY_WIN;
			break;
		case 135:
			return _KEY_MENU;
			break;
	}
#endif

	return _KEY_MAX;
}

bool cInput::Update()
{
#if defined(WINDOWS_SPACE)

	Cursor.CursorButton1Click = false;
	Cursor.CursorButton2Click = false;
	Cursor.CursorButton1DoubleClick = false;
	Cursor.CursorButton2DoubleClick = false;

	MSG msg;

	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		//112-123 F1-F12
		if(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
		{
			_KEY_KODE_ key = CodeToKey(msg.wParam);

			if(!KeyBuffer[key].pressed)
			{
				EventKey.CallSubscribersEvent(EventKeyArg(key, 1));
			}
			else{
				EventKey.CallSubscribersEvent(EventKeyArg(key, 2));
			}

			KeyBuffer[key].pressed = true;

			return true;
		}

		if(msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP)
		{
			switch(msg.wParam)
			{
				case 16:
					if(KeyBuffer[_KEY_SHIFTL].pressed && !(GetKeyState(VK_LSHIFT) & 0x8000))KeyBuffer[_KEY_SHIFTL].pressed = false;
					if(KeyBuffer[_KEY_SHIFTR].pressed && !(GetKeyState(VK_RSHIFT) & 0x8000))KeyBuffer[_KEY_SHIFTR].pressed = false;
					break;
				case 17:
					if(KeyBuffer[_KEY_CTRLL].pressed && !(GetKeyState(VK_LCONTROL) & 0x8000))KeyBuffer[_KEY_CTRLL].pressed = false;
					if(KeyBuffer[_KEY_CTRLR].pressed && !(GetKeyState(VK_RCONTROL) & 0x8000))KeyBuffer[_KEY_CTRLR].pressed = false;
					break;
				case 18:
					if(KeyBuffer[_KEY_ALTL].pressed && !(GetKeyState(VK_LMENU) & 0x8000))KeyBuffer[_KEY_ALTL].pressed = false;
					if(KeyBuffer[_KEY_ALTR].pressed && !(GetKeyState(VK_RMENU) & 0x8000))KeyBuffer[_KEY_ALTR].pressed = false;
					break;
				default:
					KeyBuffer[CodeToKey(msg.wParam)].pressed = false;
			}

			EventKey.CallSubscribersEvent(EventKeyArg(CodeToKey(msg.wParam), 0));

			return true;
		}

		if(msg.message == WM_MOUSEMOVE)
		{
			POINT curpos;

			GetCursorPos(&curpos);

			if(ScreenToClient(Screen::MainWindow.hWnd, &curpos))
			{
				if(Cursor.Pos.last_x != Cursor.Pos.x || Cursor.Pos.last_y != Cursor.Pos.y){
					Cursor.Pos.changed = true;
				}

				Cursor.Pos.last_x = Cursor.Pos.x;
				Cursor.Pos.last_y = Cursor.Pos.y;

				Cursor.Pos.x = curpos.x;
				Cursor.Pos.y = curpos.y;

				EventCursorMove.CallSubscribersEvent(EventCursorMoveArg(Cursor.Pos.x, Cursor.Pos.y));
			}

			return true;
		}

		if(msg.message == WM_LBUTTONDOWN)
		{
			Cursor.CursorButton1 = true;
			Cursor.CursorButton1Down = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(1, CURSOR_DOWN));

			return true;
		}

		if(msg.message == WM_RBUTTONDOWN)
		{
			Cursor.CursorButton2 = true;
			Cursor.CursorButton2Down = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(2, CURSOR_DOWN));

			return true;
		}

		if(msg.message == WM_LBUTTONUP)
		{
			Cursor.CursorButton1 = false;
			Cursor.CursorButton1Click = true;
			Cursor.CursorButton1Down = false;
			Cursor.CursorButton1Up = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(1, CURSOR_UP));

			return true;
		}

		if(msg.message == WM_RBUTTONUP)
		{
			Cursor.CursorButton2 = false;
			Cursor.CursorButton2Click = true;
			Cursor.CursorButton2Down = false;
			Cursor.CursorButton2Up = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(2, CURSOR_UP));

			return true;
		}

		if(msg.message == WM_LBUTTONDBLCLK)
		{
			Cursor.CursorButton1DoubleClick = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(1, CURSOR_DOUBLE_CLICK));

			return true;
		}

		if(msg.message == WM_RBUTTONDBLCLK)
		{
			Cursor.CursorButton1DoubleClick = true;
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(2, CURSOR_DOUBLE_CLICK));

			return true;
		}

		if(msg.message == WM_MOUSEWHEEL)
		{
			EventCursorButton.CallSubscribersEvent(EventCursorButtonArg(msg.wParam, CURSOR_MOUSEWHEEL));
			return true;
		}
	}
#elif defined(LINUX_SPACE)

	Cursor.CursorButton1Click = false;
	Cursor.CursorButton2Click = false;
	Cursor.CursorButton1DoubleClick = false;
	Cursor.CursorButton2DoubleClick = false;

	uint EventCount = XPending(pScreen->win.display);

	XEvent Event;
	for(uint i = 0; i < EventCount; ++i)
	{
		XNextEvent(pScreen->win.display, &Event);
		//DBG_LOG("Event:%d\n",Event.type);

		int type = Event.type;
		//uint key=0;

		switch(type)
		{
			case MotionNotify:
				//DBG_LOG("%d %d\n",event.xmotion.x,event.xmotion.y);
				if(Cursor.Pos.last_x != Cursor.Pos.x || Cursor.Pos.last_y != Cursor.Pos.y){
					Cursor.Pos.changed = true;
				}

				Cursor.Pos.last_x = Cursor.Pos.x;
				Cursor.Pos.last_y = Cursor.Pos.y;

				Cursor.Pos.x = Event.xmotion.x;
				Cursor.Pos.y = Event.xmotion.y;

				Events::EventCursorMove.CallEvent(Cursor.Pos.x, Cursor.Pos.y);
				break;

			case KeyPress:
			{
				_KEY_KODE_ key = CodeToKey(Event.xkey.keycode);

				if(!KeyBuffer[key].pressed)
				{
					EventKey.CallSubscribersEvent(EventKeyArg(key, 1));
				}
				else{
					EventKey.CallSubscribersEvent(EventKeyArg(key, 2));
				}

				KeyBuffer[key].pressed = true;
			}
			break;

			case KeyRelease:
			{
				KeyBuffer[CodeToKey(Event.xkey.keycode)].pressed = false;

				EventKey.CallSubscribersEvent(EventKeyArg(CodeToKey(Event.xkey.keycode), 0));
			}
			break;

			case ButtonPress:
				switch(Event.xbutton.button)
				{
					case 1:
						Cursor.CursorButton1 = true;
						Cursor.CursorButton1Time = Event.xbutton.time;

						if(!Cursor.CursorButton1DownChecked)
						{
							Cursor.CursorButton1Down = true;
							Cursor.CursorButton1DownChecked = true;
							Cursor.CursorButton1UpChecked = false;

							Events::EventCursorButton.CallEvent(1, CURSOR_DOWN);
						}
						else{
							Cursor.CursorButton1Down = false;
						}
						break;
					case 3:
						Cursor.CursorButton2 = true;
						Cursor.CursorButton2Time = Event.xbutton.time;

						if(!Cursor.CursorButton2DownChecked)
						{
							Cursor.CursorButton2Down = true;
							Cursor.CursorButton2DownChecked = true;
							Cursor.CursorButton2UpChecked = false;

							Events::EventCursorButton.CallEvent(2, CURSOR_DOWN);
						}
						else{
							Cursor.CursorButton2Down = false;
						}
						break;
				}
				break;
			case ButtonRelease:
				switch(Event.xbutton.button)
				{
					case 1:
						if(Cursor.CursorButton1)Cursor.CursorButton1Up = true;
						Cursor.CursorButton1 = false;
						Cursor.CursorButton1Down = false;

						if(Event.xbutton.time - Cursor.CursorButton1Time <= 200){
							Cursor.CursorButton1Click = true;

							if(++Cursor.CursorButton1ClickCount >= 2 && Event.xbutton.time - Cursor.CursorButton1ClickTime < 200){
								Cursor.CursorButton1DoubleClick = true;
								Cursor.CursorButton1ClickCount = 0;
								Events::EventCursorButton.CallEvent(1, CURSOR_DOUBLE_CLICK);
							}
							else{
								Cursor.CursorButton1ClickTime = Event.xbutton.time;
							}
						}

						if(!Cursor.CursorButton1UpChecked)
						{
							Cursor.CursorButton1UpChecked = true;
							Cursor.CursorButton1DownChecked = false;

							Events::EventCursorButton.CallEvent(1, CURSOR_UP);
						}

						break;
					case 3:
						if(Cursor.CursorButton2)Cursor.CursorButton2Up = true;
						Cursor.CursorButton2 = false;
						Cursor.CursorButton2Down = false;

						if(Event.xbutton.time - Cursor.CursorButton2Time <= 200){
							Cursor.CursorButton2Click = true;

							if(++Cursor.CursorButton2ClickCount >= 2 && Event.xbutton.time - Cursor.CursorButton2ClickTime < 200){
								Cursor.CursorButton2DoubleClick = true;
								Cursor.CursorButton2ClickCount = 0;
								Events::EventCursorButton.CallEvent(2, CURSOR_DOUBLE_CLICK);
							}
							else{
								Cursor.CursorButton2ClickTime = Event.xbutton.time;
							}
						}

						if(!Cursor.CursorButton2UpChecked)
						{
							Cursor.CursorButton2UpChecked = true;
							Cursor.CursorButton2DownChecked = false;

							Events::EventCursorButton.CallEvent(2, CURSOR_UP);
						}
						break;
				}
				break;

			case ClientMessage:
				TerminateCollback();
				break;
		}
	}

#elif defined(ANDROID_SPACE)
	Cursor.CursorButton1Click = false;
	Cursor.CursorButton1DoubleClick = false;

	_android_input_event_item* cur = AndroidInputListRoot;

	while(cur->next)
	{
		Cursor.android_last_event = cur->AndroidInput.Event;

		switch(cur->AndroidInput.Event)
		{
			case ANDROID_INPUT_EVENT_TOUCH_DOWN:
				//DBG_LOG("DOWN %f",Timer.ProgrammTime);
				Cursor.CursorButton1 = true;
				Cursor.CursorButton1Time = cur->AndroidInput.time;

				if(!Cursor.CursorButton1DownChecked)
				{
					Cursor.CursorButton1Down = true;
					Cursor.CursorButton1DownChecked = true;
					Cursor.CursorButton1UpChecked = false;

					Cursor.Pos.changed = false;

					if(Cursor.Pos.last_x != Cursor.Pos.x || Cursor.Pos.last_y != Cursor.Pos.y){
						Cursor.Pos.changed = true;
					}

					Cursor.Pos.last_x = Cursor.Pos.x;
					Cursor.Pos.last_y = Cursor.Pos.y;
					//DBG_LOG("dwl %f %f",Cursor.Pos.last_x,Cursor.Pos.last_y);

					Cursor.Pos.x = cur->AndroidInput.x;
					Cursor.Pos.y = cur->AndroidInput.y;

					Cursor.Pos.move_control_x = cur->AndroidInput.x;
					Cursor.Pos.move_control_y = cur->AndroidInput.y;
				}
				else{
					Cursor.CursorButton1Down = false;
				}
				break;
			case ANDROID_INPUT_EVENT_TOUCH_UP:
				//DBG_LOG("UP %f",Timer.ProgrammTime);
				if(Cursor.CursorButton1)Cursor.CursorButton1Up = true;
				Cursor.CursorButton1 = false;
				Cursor.CursorButton1Down = false;

				if(cur->AndroidInput.time - Cursor.CursorButton1Time <= 200){
					Cursor.CursorButton1Click = true;

					if(++Cursor.CursorButton1ClickCount >= 2 && cur->AndroidInput.time - Cursor.CursorButton1ClickTime < 200){
						Cursor.CursorButton1DoubleClick = true;
						Cursor.CursorButton1ClickCount = 0;
					}
					else{
						Cursor.CursorButton1ClickTime = cur->AndroidInput.time;
					}
				}

				if(!Cursor.CursorButton1UpChecked)
				{
					Cursor.CursorButton1UpChecked = true;
					Cursor.CursorButton1DownChecked = false;
				}

				if(!Cursor.long_tap){
					Events::EventCursorButton.CallEvent(2, CURSOR_DOWN);
					Events::EventCursorButton.CallEvent(2, CURSOR_UP);
				}
				else{
					Events::EventCursorButton.CallEvent(1, CURSOR_UP);
				}
				break;
			case ANDROID_INPUT_EVENT_TOUCH_MOVE:
				Cursor.Pos.changed = false;

				if(Cursor.Pos.move_control_x != Cursor.Pos.x || Cursor.Pos.move_control_y != Cursor.Pos.y){
					//DBG_LOG("change %f:%f %f:%f",Cursor.Pos.x,Cursor.Pos.move_control_x,Cursor.Pos.y,Cursor.Pos.move_control_y);
					Cursor.Pos.changed = true;
				}

				Cursor.Pos.last_x = Cursor.Pos.x;
				Cursor.Pos.last_y = Cursor.Pos.y;
				//DBG_LOG("mwl %f %f",Cursor.Pos.last_x,Cursor.Pos.last_y);
				Cursor.Pos.move_control_x = Cursor.Pos.x;
				Cursor.Pos.move_control_y = Cursor.Pos.y;

				Cursor.Pos.x = cur->AndroidInput.x;
				Cursor.Pos.y = cur->AndroidInput.y;

				Cursor.long_tap = false;
				if(cur->AndroidInput.time - Cursor.CursorButton1Time > 500)
				{
					Cursor.long_tap = true;
					Events::EventCursorButton.CallEvent(1, CURSOR_DOWN);
				}
				break;
		}

		_android_input_event_item* prev = cur;
		cur = cur->next;
		delete prev;
	}

	AndroidInputListRoot = AndroidInputItemLast = cur;
#endif
	return true;
}

void cInput::ResetStats()
{
	//Cursor.CursorButton1Down = false;
	//Cursor.CursorButton1Up = false;
	Cursor.Pos.changed = false;
#ifndef ANDROID_SPACE
	//Cursor.CursorButton2Down = false;
	//Cursor.CursorButton2Up = false;
#else
	Cursor.long_tap = false;
#endif
}

void cInput::KeyDown(uint key)
{
	if(key<0 || key>KEY_BUFFER_SIZE)return;
	//DBG_LOG("KeyDown(%d)",key);
	KeyBuffer[key].pressed = true;
}

bool cInput::IsKeyDown(uint key)
{
	if(key<0 || key>KEY_BUFFER_SIZE)return false;

	return KeyBuffer[key].pressed;
}