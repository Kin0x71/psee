#ifndef _INPUT_CONTROL_H_
#define _INPUT_CONTROL_H_

class cUiBase;

namespace input_control
{
	extern float LastWorldCursorX;
	extern float LastWorldCursorY;
	
	void Init();
	void Update();
};

void input_control_UiConsole_OnEnter(cUiBase* conole,char* data);

#endif