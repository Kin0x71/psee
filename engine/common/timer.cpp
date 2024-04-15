#include "timer.h"
#include <time.h>
#include <stdio.h>
/*
#if defined(WINDOWS_SPACE)
#include <windows.h>
#endif
*/

void cTimer::SetStartTime()
{
#if defined(WINDOWS_SPACE)
	//ProgrammStartTime=(double)timeGetTime()/1000.0f;
	//ProgrammStartTick=clock();
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&ProgrammStartTick);

	ProgrammLastTick = ProgrammStartTick;
	//printf("SetStartTime(%f)\n",gProgrammStartTime);
#elif defined(LINUX_DESCTOP_SPACE)
	timespec mt;
	clock_gettime(CLOCK_REALTIME,&mt);
	ProgrammStartTime=float(mt.tv_sec)+(float(mt.tv_nsec)/1000000000.0f);
	//printf("SetStartTime(%f)\n",gProgrammStartTime);
#endif
}

void cTimer::Update()
{
#if defined(WINDOWS_SPACE)

	//LastFrameTick=ProgrammCurrentTick;

	//ProgrammCurrentTick=clock();//(double)timeGetTime()/1000.0f;

	QueryPerformanceCounter(&ProgrammCurrentTick);
	//printf("%d %d %d %d %d\n",ProgrammCurrentTick.HighPart,ProgrammCurrentTick.LowPart,ProgrammCurrentTick.QuadPart,ProgrammCurrentTick.u.HighPart,ProgrammCurrentTick.u.LowPart);
	ProgrammTime = float(ProgrammCurrentTick.QuadPart - ProgrammStartTick.QuadPart) / float(Frequency.QuadPart);//(float)CLOCKS_PER_SEC;

	DeltaTime = (ProgrammCurrentTick.QuadPart - ProgrammLastTick.QuadPart) / float(Frequency.QuadPart);

	ProgrammLastTick = ProgrammCurrentTick;
	//DeltaTime=(ProgrammCurrentTick-LastFrameTick);//(long)CLOCKS_PER_SEC;
	
	//printf("%lf %lf Timer\n",ProgrammCurrentTick,ProgrammTime);
	//clock_t clock();
	//printf("(%f)\n",DeltaTime);
#elif defined(LINUX_SPACE)
	timespec mt;
	clock_gettime(CLOCK_REALTIME,&mt);
	gProgrammCurrentTime=float((float(mt.tv_sec)+(float(mt.tv_nsec)/1000000000.0f)));
	//printf("%f\n",gProgrammStartTime);
	//printf("%f\n",gProgrammCurrentTime);
	//printf("%f+%f=%f\n",float(mt.tv_sec),(float(mt.tv_nsec)/1000000000.0f),float(float(mt.tv_sec)+(float(mt.tv_nsec)/1000000000.0f)));
	gProgrammTime=float(gProgrammCurrentTime-gProgrammStartTime);
	//printf("%f\n",gProgrammTime);
#endif
}

float cTimer::GetTime()
{
	return ProgrammTime;
}
