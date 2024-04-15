#include <engine/defines.h>

#include <iostream>

#ifdef WINDOWS_SPACE
#include <windows.h>
#endif

#include <engine/engine.h>
#include <engine/common/screen.h>
#include <engine/common/timer.h>

#include "game.h"

extern cTimer Timer;

int main()
{
	Engine::Initialize(new cGame());

	float fps_start_time = Timer.ProgrammTime;
	float t_fps = 0.0f;
	Timer.fps = 1.0f;

	Timer.SetStartTime();

	while(true)
	{
		Timer.Update();

		if(Timer.ProgrammTime - fps_start_time >= 1.0f)
		{
			Timer.fps = t_fps;

			char dtstr[64];
			sprintf(dtstr, "%f %d/%d", Timer.fps, Engine::Renderer.objects_in_view, Engine::pCurrentScene->ObjectList.count);
			
			#ifdef WINDOWS_SPACE
				SetWindowTextA(Screen::MainWindow.hWnd, dtstr);
			#endif

			t_fps = 0.0f;
			fps_start_time = Timer.ProgrammTime;
		}
		t_fps += 1.0f;

		Engine::Update();
	}

	printf("end programm\n");

	//system("pause");
}