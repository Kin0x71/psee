#ifndef _TIMER_H_
#define _TIMER_H_

#include <engine/defines.h>

#ifdef WINDOWS_SPACE
#include <windows.h>
#elif defined(LINUX_DESCTOP_SPACE)

#endif

class cTimer
{
public:
#if defined(WINDOWS_SPACE)
	LARGE_INTEGER Frequency;
	LARGE_INTEGER ProgrammStartTick;
	LARGE_INTEGER ProgrammLastTick;
	LARGE_INTEGER ProgrammCurrentTick;
#elif defined(LINUX_DESCTOP_SPACE)
	double Frequency;
	double ProgrammStartTick;
	double ProgrammLastTick;
	double ProgrammCurrentTick;
#endif

	float ProgrammTime;
	float fps = 0.0f;

	float LastFrameTick;

	float TimeTicksMax;
	float StartTimeCheck;
	float LastFrameTime;
	float CheckFrameTime;
	float DeltaTime;

	int TicksCount;
	
	cTimer()
	{
		TimeTicksMax=0.0f;
		StartTimeCheck=0.0f;
		LastFrameTime=0.0f;
		CheckFrameTime=0.0f;
		DeltaTime=0.0f;

		LastFrameTick=0;
		TicksCount=0;
	}

void SetStartTime();
void Update();
float GetTime();
};

#endif