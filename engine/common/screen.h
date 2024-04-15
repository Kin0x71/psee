#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <engine/defines.h>

#ifdef WINDOWS_SPACE
#include <windows.h>
#elif defined(LINUX_DESCTOP_SPACE)
#include <X11/Xlib.h>
#endif

namespace Screen {

	struct _WINDOW
	{
		#if defined(WINDOWS_SPACE)
		HWND hWnd = 0;
		HDC hDc = 0;
		HINSTANCE hInstance = 0;
		#elif defined(LINUX_DESCTOP_SPACE)
		Display* display = 0;
		#endif
	};

	extern _WINDOW MainWindow;
	extern _WINDOW GameWindow;
	extern int width;
	extern int height;
	extern int posx;
	extern int posy;

	bool CreateGameWindow(int PosX, int PosY, int Width, int Height);
	int InitializeOGL();
	int SetupOGL();

	void UpdateSystemEvents();
	void Clear();
	bool Swap();
};

#endif